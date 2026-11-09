"""Execute the actual workflow Bash with native tools isolated behind a closed PATH."""
import json
import os
from pathlib import Path
import plistlib
import re
import shutil
import subprocess
import sys
import unittest

import yaml
from helper_support import Fixture, source

ROOT = Path(__file__).resolve().parents[1]
INTAKE = "293a7940fbd547e9e3d399e327525e0ec8288f35"
PRODUCT = "Keychain Access"
PACKAGE = Path("test/Plugins") / (PRODUCT + ".bundle")
CONTENTS = PACKAGE / "Contents"
WORKFLOW = ROOT / ".github/workflows/release.yml"

STUB = r'''#!PYTHON
import json, os, pathlib, plistlib, sys
tool = pathlib.Path(sys.argv[0]).name
args = sys.argv[1:]
with open(os.environ["NATIVE_LOG"], "a") as out:
    out.write(json.dumps([tool, *args]) + "\n")
if os.environ.get("FAIL_TOOL") == tool:
    sys.exit(23)
contents = pathlib.Path("test/Plugins/Keychain Access.bundle/Contents")
if tool == "xcodebuild":
    expected = "CONFIGURATION_BUILD_DIR=" + str(pathlib.Path.cwd() / "test/Plugins")
    assert expected in args, args
    assert ["-configuration", "Default"] == args[4:6], args
    assert "ARCHS=arm64 x86_64" in args
    assert "MACOSX_DEPLOYMENT_TARGET=11.5" in args
    assert "CODE_SIGNING_ALLOWED=NO" in args
    for relative in ["MacOS/Keychain Access", "Info.plist",
                     "Resources/English.lproj/InfoPlist.strings", "_CodeSignature/CodeResources"]:
        assert not (contents / relative).exists(), relative
    if not os.environ.get("STALE_BUILD"):
        (contents / "MacOS").mkdir(parents=True, exist_ok=True)
        (contents / "MacOS/Keychain Access").write_bytes(b"fresh-native-output")
        (contents / "Info.plist").write_bytes(plistlib.dumps({
            "CFBundleShortVersionString": os.environ["SOURCE_VERSION"]
        }, fmt=plistlib.FMT_BINARY))
        (contents / "Resources/English.lproj/InfoPlist.strings").write_bytes(
            pathlib.Path("English.lproj/InfoPlist.strings").read_text().encode("utf-16"))
elif tool == "lipo":
    arches = os.environ.get("DYLIB_ARCHES" if args[-1].endswith(".dylib") else "ARCHES",
                            "arm64 x86_64")
    print(arches if args[0] == "-archs" else "Architectures in the fat file: " + arches)
elif tool == "security":
    if args[0] == "find-identity":
        print("" if os.environ.get("MISSING_IDENTITY") else
              '  1) ABCDEF "Developer ID Application: Offline Fixture"')
elif tool == "codesign":
    if "--verify" in args and os.environ.get("FAIL_VERIFY"):
        sys.exit(7)
    if "--verify" not in args:
        (contents / "_CodeSignature").mkdir(exist_ok=True)
        (contents / "_CodeSignature/CodeResources").write_bytes(b"fresh-signature")
elif tool in ("ditto", "hdiutil"):
    pathlib.Path(args[-1]).write_bytes(b"offline-package")
elif tool == "xcrun":
    if args[:2] == ["notarytool", "submit"]:
        print(json.dumps({"id": "offline-id", "status": os.environ.get("NOTARY_STATUS", "Accepted")}))
        sys.exit(int(os.environ.get("NOTARY_EXIT", "0")))
    if args[:2] == ["stapler", "staple"] and os.environ.get("FAIL_STAPLE"):
        sys.exit(8)
elif tool == "plutil":
    print(json.load(sys.stdin)[args[1]])
elif tool == "base64":
    import base64
    pathlib.Path(args[args.index("-o")+1]).write_bytes(base64.b64decode(sys.stdin.read()))
elif tool == "python3":
    if "release.py" in args[0]:
        if os.environ.get("FAIL_PUBLICATION"):
            sys.exit(9)
        print("offline publication gate reached")
    else:
        os.execv(sys.executable, [sys.executable, *args])
else:
    sys.exit("Unexpected native stub: " + tool)
'''


def original(path):
    return subprocess.check_output(["git", "show", f"{INTAKE}:{path}"], cwd=ROOT)


class NativeWorkflowTests(Fixture):
    def setUp(self):
        super().setUp()
        self.cwd = self.root / "checkout"
        self.cwd.mkdir()
        self.bin = self.root / "tools"
        self.bin.mkdir()
        self.log = self.root / "native.jsonl"
        for tool in ("xcodebuild", "lipo", "security", "codesign", "ditto", "hdiutil",
                     "xcrun", "plutil", "base64", "python3"):
            target = self.bin / tool
            target.write_text(STUB.replace("PYTHON", sys.executable))
            target.chmod(0o755)
        # Only these ordinary tools may escape the stub directory. No native
        # tool, xcrun fallback, shell profile, or developer tools enter PATH.
        for tool in ("rm", "ls", "grep", "head", "sed", "sort"):
            (self.bin / tool).symlink_to(shutil.which(tool))
        self.steps = yaml.safe_load(WORKFLOW.read_text())["jobs"]["build-macos"]["steps"]
        self.version = source.read(ROOT)
        self.env = {
            "PATH": str(self.bin),
            "HOME": str(self.cwd),
            "RUNNER_TEMP": str(self.cwd),
            "NATIVE_LOG": str(self.log),
            "SOURCE_VERSION": self.version,
            "GITHUB_ENV": str(self.cwd / "github-env"),
            "KEYCHAIN_PATH": str(self.cwd / "offline.keychain"),
            "BUNDLE_PATH": str(PACKAGE),
            "LC_ALL": "C",
            "PYTHONDONTWRITEBYTECODE": "1",
            "CERT_BASE64": "b2ZmbGluZSBmaXh0dXJl",
            "CERT_PASSWORD": "offline-only",
            "KEYCHAIN_PASSWORD": "offline-only",
            "APPLE_ID": "offline@example.invalid",
            "TEAM_ID": "OFFLINEONLY",
            "APP_PASSWORD": "offline-only",
            "GITHUB_SHA": "a" * 40,
            "SELECTED_SHA": "a" * 40,
            "GH_TOKEN": "offline-only",
            "GITHUB_REPOSITORY": "miyako/4d-plugin-keychain",
            "ZIP_PATH": f"{PRODUCT}-{self.version}.zip",
            "DMG_PATH": f"{PRODUCT}-{self.version}.dmg",
        }
        shutil.copytree(ROOT / ".github/scripts", self.cwd / ".github/scripts",
                        ignore=shutil.ignore_patterns("__pycache__"))
        self.sources(self.cwd)
        (self.cwd / "a").mkdir()
        (self.cwd / "a/libjsoncpp.a").write_bytes(b"offline-prebuilt")
        self.contents = self.cwd / CONTENTS
        for suffix, data in {
            "MacOS/Keychain Access": b"stale-executable",
            "Info.plist": b"stale-plist",
            "Resources/English.lproj/InfoPlist.strings": b"stale-localization",
            "_CodeSignature/CodeResources": b"stale-signature",
            "Resources/manifest.json": b'{"fixture":true}',
            "Resources/constants.xlf": b"<fixture/>",
            "Windows64/Keychain Access.4DX": b"preserved-windows-stub",
            "QuickLook/Thumbnail.png": b"preserved-thumbnail",
            "MacOS/runtime.dylib": b"preserved-runtime",
        }.items():
            path = self.contents / suffix
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(data)
        (self.cwd / "constants.xlf").write_bytes(b"<fixture/>")
        for parent in ("test/Plugins/Default/Keychain Access.bundle/Contents",
                       "test/Plugins/Keychain Access.build", "build/Keychain Access.build"):
            directory = self.cwd / parent
            directory.mkdir(parents=True, exist_ok=True)
        sibling = self.cwd / "test/Plugins/Default/unrelated-resource"
        sibling.write_bytes(b"keep")

    def run_step(self, name, success=True):
        step = next(s for s in self.steps if s.get("name") == name)
        script = step["run"]
        values = {
            "env.PRODUCT_NAME": PRODUCT,
            "env.BASE_DIR": "test",
            "env.MACOS_DEPLOYMENT_TARGET": "11.5",
            "needs.version.outputs.version": self.version,
            "github.sha": "a" * 40,
            "steps.preflight.outputs.version": self.version,
            "steps.preflight.outputs.tag": "v" + self.version,
            "env.ZIP_PATH": f"{PRODUCT}-{self.version}.zip",
            "env.DMG_PATH": f"{PRODUCT}-{self.version}.dmg",
        }
        script = re.sub(r"\$\{\{\s*(.*?)\s*\}\}", lambda m: values[m[1]], script)
        result = subprocess.run(["/bin/bash", "--noprofile", "--norc", "-eo", "pipefail", "-c", script],
                                cwd=self.cwd, env=self.env, capture_output=True, text=True)
        if success:
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        elif success is False:
            self.assertNotEqual(result.returncode, 0, result.stdout + result.stderr)
        return result

    def pipeline(self):
        names = ("initial clean source and publication preflight", "check prebuilt libraries",
                 "import Developer ID certificate", "store notarytool credentials",
                 "build macOS plugin (universal)", "verify universal binary", "codesign",
                 "package, notarize & staple")
        for name in names:
            result = self.run_step(name, success=None)
            if result.returncode:
                return result
        step = next((step for step in self.steps
                     if "release.py publish" in step.get("run", "")), None)
        self.assertIsNotNone(step, "Workflow must invoke the create-only publication helper")
        return self.run_step(step["name"], success=None)

    def build(self):
        return self.run_step("build macOS plugin (universal)")

    def test_build_fresh_canonical_outputs_preserves_resources(self):
        self.build()
        self.run_step("verify universal binary")
        self.assertEqual((self.contents / "MacOS/Keychain Access").read_bytes(), b"fresh-native-output")
        self.assertEqual((self.contents / "Windows64/Keychain Access.4DX").read_bytes(),
                         b"preserved-windows-stub")
        self.assertEqual((self.contents / "QuickLook/Thumbnail.png").read_bytes(), b"preserved-thumbnail")
        self.assertEqual((self.contents / "MacOS/runtime.dylib").read_bytes(), b"preserved-runtime")
        self.assertTrue((self.cwd / "test/Plugins/Default/unrelated-resource").exists())
        self.assertFalse((self.cwd / "build/Keychain Access.build").exists())
        self.assertFalse((self.cwd / "test/Plugins/Keychain Access.build").exists())

    def test_misdirected_build_cannot_reuse_stale_binary(self):
        self.env["STALE_BUILD"] = "1"
        self.build()
        self.run_step("verify universal binary", success=False)

    def test_empty_fresh_native_executable_rejected(self):
        self.build()
        (self.contents / "MacOS/Keychain Access").write_bytes(b"")
        self.run_step("verify universal binary", success=False)

    def test_cleanup_failure_stops_before_xcode(self):
        (self.contents / "MacOS/Keychain Access").unlink()
        (self.contents / "MacOS/Keychain Access").mkdir()
        self.run_step("build macOS plugin (universal)", success=False)
        calls = [json.loads(line) for line in self.log.read_text().splitlines()]
        self.assertFalse(any(call[0] == "xcodebuild" for call in calls))

    def test_native_build_failure_is_not_swallowed(self):
        self.env["FAIL_TOOL"] = "xcodebuild"
        self.run_step("build macOS plugin (universal)", success=False)

    def test_architecture_tokens_are_exact(self):
        self.build()
        for arches in ("arm64e x86_64", "arm64 x86_64h", "arm64 x86_64 i386"):
            with self.subTest(arches=arches):
                self.env["ARCHES"] = arches
                self.run_step("verify universal binary", success=False)

    def test_additional_architectures_allowed(self):
        self.build()
        self.env["ARCHES"] = "arm64 x86_64 arm64e"
        self.run_step("verify universal binary")
        # The original dylib policy required the two slices but did not ban i386.
        self.env["DYLIB_ARCHES"] = "arm64 x86_64 i386"
        self.run_step("verify universal binary")

    def test_dylib_requires_exact_universal_tokens(self):
        self.build()
        for arches in ("arm64e x86_64", "arm64 x86_64h"):
            with self.subTest(arches=arches):
                self.env["DYLIB_ARCHES"] = arches
                self.run_step("verify universal binary", success=False)

    def test_missing_copied_resources_fail(self):
        self.build()
        for suffix in ("Resources/manifest.json", "Resources/constants.xlf"):
            with self.subTest(resource=suffix):
                path = self.contents / suffix
                saved = path.read_bytes()
                path.unlink()
                self.run_step("verify universal binary", success=False)
                path.write_bytes(saved)

    def test_signing_requires_strict_verification(self):
        self.build()
        self.run_step("codesign")
        calls = [json.loads(line) for line in self.log.read_text().splitlines()]
        verify = next(call for call in calls if call[0] == "codesign" and "--verify" in call)
        self.assertIn("--strict", verify)
        self.assertIn("--deep", verify)
        self.env["FAIL_VERIFY"] = "1"
        self.run_step("codesign", success=False)

    def test_notary_acceptance_and_staple_succeed(self):
        self.run_step("package, notarize & staple")
        self.assertTrue((self.cwd / f"{PRODUCT}-{self.version}.zip").exists())
        self.assertTrue((self.cwd / f"{PRODUCT}-{self.version}.dmg").exists())
        calls = [json.loads(line) for line in self.log.read_text().splitlines()]
        submits = [call for call in calls if call[:3] == ["xcrun", "notarytool", "submit"]]
        self.assertEqual(len(submits), 2)
        self.assertEqual(calls[-1][:3], ["xcrun", "stapler", "staple"])

    def test_notary_nonzero_accepted_is_failure(self):
        self.env.update(NOTARY_EXIT="5", NOTARY_STATUS="Accepted")
        self.run_step("package, notarize & staple", success=False)
        self.assertNotIn('"stapler"', self.log.read_text())

    def test_notary_invalid_and_missing_status_are_failure(self):
        for status in ("Invalid", ""):
            with self.subTest(status=status):
                self.env["NOTARY_STATUS"] = status
                self.run_step("package, notarize & staple", success=False)
        self.assertNotIn('"stapler"', self.log.read_text())

    def test_staple_failure_is_failure(self):
        self.env["FAIL_STAPLE"] = "1"
        self.run_step("package, notarize & staple", success=False)

    def test_native_pipeline_uses_fresh_signed_output(self):
        self.build()
        self.run_step("verify universal binary")
        self.run_step("codesign")
        self.run_step("package, notarize & staple")
        self.assertEqual((self.contents / "MacOS/Keychain Access").read_bytes(),
                         b"fresh-native-output")
        self.assertEqual((self.contents / "_CodeSignature/CodeResources").read_bytes(),
                         b"fresh-signature")
        calls = [json.loads(line) for line in self.log.read_text().splitlines()]
        tools = [call[0] for call in calls]
        self.assertLess(tools.index("xcodebuild"), tools.index("codesign"))
        self.assertLess(tools.index("codesign"), tools.index("ditto"))
        self.assertLess(tools.index("ditto"), tools.index("xcrun"))

    def test_complete_native_pipeline_reaches_only_fake_publication_after_staple(self):
        result = self.pipeline()
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        calls = [json.loads(line) for line in self.log.read_text().splitlines()]
        publish = next(i for i, call in enumerate(calls)
                       if call[0] == "python3" and "release.py" in call[1]
                       and call[2] == "publish")
        staple = next(i for i, call in enumerate(calls)
                      if call[:3] == ["xcrun", "stapler", "staple"])
        self.assertGreater(publish, staple)
        self.assertEqual((self.contents / "_CodeSignature/CodeResources").read_bytes(),
                         b"fresh-signature")

    def test_credential_sign_package_arch_notary_and_staple_failures_never_publish(self):
        cases = [
            {"FAIL_TOOL": tool} for tool in
            ("base64", "security", "xcrun", "xcodebuild", "lipo", "codesign", "ditto", "hdiutil")
        ] + [{"MISSING_IDENTITY": "1"}, {"FAIL_VERIFY": "1"},
             {"STALE_BUILD": "1"}, {"ARCHES": "arm64e x86_64"},
             {"DYLIB_ARCHES": "arm64 x86_64h"},
             {"NOTARY_EXIT": "5", "NOTARY_STATUS": "Accepted"},
             {"NOTARY_STATUS": "Invalid"}, {"NOTARY_STATUS": ""},
             {"FAIL_STAPLE": "1"}]
        original_env = dict(self.env)
        for case in cases:
            with self.subTest(case=case):
                self.env = {**original_env, **case}
                self.log.unlink(missing_ok=True)
                result = self.pipeline()
                self.assertNotEqual(result.returncode, 0, result.stdout)
                calls = [json.loads(line) for line in self.log.read_text().splitlines()]
                self.assertFalse(any(call[0] == "python3" and "release.py" in call[1]
                                     and call[2] == "publish"
                                     for call in calls))

    def test_missing_original_prebuilt_dependency_stops_before_signing_or_publish(self):
        (self.cwd / "a/libjsoncpp.a").unlink()
        result = self.pipeline()
        self.assertNotEqual(result.returncode, 0)
        calls = [json.loads(line) for line in self.log.read_text().splitlines()]
        self.assertFalse(any(call[0] in ("security", "xcodebuild", "codesign") for call in calls))
        self.assertFalse(any(call[0] == "python3" and "publish" in call for call in calls))

    def test_missing_empty_and_drifted_fresh_metadata_reject(self):
        self.build()
        for relative in ("Info.plist", "Resources/English.lproj/InfoPlist.strings"):
            path = self.contents / relative
            saved = path.read_bytes()
            for raw in (None, b"", saved.replace(self.version.encode(),
                                               b"900.800.700")):
                # Binary plist version lengths are serialized: construct genuine drift.
                if raw not in (None, b"") and relative == "Info.plist":
                    raw = plistlib.dumps({"CFBundleShortVersionString": "900.800.700"})
                elif raw not in (None, b""):
                    raw = ('CFBundleShortVersionString="900.800.700";\r'
                           'CFBundleGetInfoString="Keychain Access version 900.800.700, '
                           'Copyright 2022 miyako.";\r').encode("utf-16")
                if raw is None:
                    path.unlink()
                else:
                    path.write_bytes(raw)
                result = subprocess.run(
                    [str(self.bin / "python3"), ".github/scripts/source_version.py",
                     "--verify-bundle", str(PACKAGE)], cwd=self.cwd, env=self.env,
                    text=True, capture_output=True)
                self.assertNotEqual(result.returncode, 0)
                path.write_bytes(saved)


class OriginalContractTests(unittest.TestCase):
    def test_intake_anchor_documents_B_and_actual_mac_only_topology(self):
        bump = original(".github/workflows/bump-version.yml").decode()
        self.assertIn("MARKETING_VERSION/CURRENT_PROJECT_VERSION AND Info.plist", bump)
        self.assertIn('git tag "v${NEW}"', bump)
        release = yaml.safe_load(original(".github/workflows/release.yml"))
        self.assertEqual(release["name"], "Build & Release")
        self.assertEqual(set(release["jobs"]), {"version", "build-macos"})
        self.assertEqual(release["jobs"]["build-macos"]["runs-on"], "macos-latest")
        publish = release["jobs"]["build-macos"]["steps"][-1]
        self.assertEqual(set(publish["with"]), {"tag_name", "name", "files"})

    def test_native_project_and_prebuilt_inputs_unchanged(self):
        # Current version values legitimately advance in the genuine Bump proof.
        path = "Keychain Access.xcodeproj/project.pbxproj"
        strip_versions = lambda data: re.sub(
            rb"((?:MARKETING_VERSION|CURRENT_PROJECT_VERSION) = )\d+\.\d+\.\d+;",
            rb"\1VERSION;", data)
        self.assertEqual(strip_versions((ROOT / path).read_bytes()), strip_versions(original(path)))
        for path in (
            "Keychain Access.vcxproj", "manifest.json", "constants.xlf", "a/libjsoncpp.a",
            str(CONTENTS / "Info.plist"),
            str(CONTENTS / "Resources/English.lproj/InfoPlist.strings"),
            str(CONTENTS / "Windows64/Keychain Access.4DX"),
            str(CONTENTS / "QuickLook/Thumbnail.png"),
        ):
            with self.subTest(path=path):
                self.assertEqual((ROOT / path).read_bytes(), original(path))

    def test_secret_import_and_credentials_scripts_preserved(self):
        old = yaml.safe_load(original(".github/workflows/release.yml"))["jobs"]["build-macos"]["steps"]
        new = yaml.safe_load(WORKFLOW.read_text())["jobs"]["build-macos"]["steps"]
        for name in ("import Developer ID certificate", "store notarytool credentials",
                     "check prebuilt libraries", "codesign"):
            actual = dict(next(s for s in new if s.get("name") == name))
            actual["run"] = actual["run"].replace("  # shellcheck disable=SC2013\n", "").replace(
                "# shellcheck disable=SC2013\n", "")
            self.assertEqual(actual, next(s for s in old if s.get("name") == name))

    def test_exact_independent_manual_workflow_contract(self):
        workflows = [
            yaml.load(path.read_text(), Loader=yaml.BaseLoader)
            for path in (ROOT / ".github/workflows").glob("*.yml")
        ]
        self.assertEqual(len(workflows), 2)
        build = next(workflow for workflow in workflows if workflow["name"] == "Build")
        bump = next(workflow for workflow in workflows if workflow["name"] != "Build")
        self.assertEqual(build["on"], {"workflow_dispatch": ""})
        self.assertEqual(set(bump["on"]), {"workflow_dispatch"})
        inputs = bump["on"]["workflow_dispatch"]["inputs"]
        self.assertEqual(set(inputs), {"mode"})
        self.assertEqual(inputs["mode"]["type"], "choice")
        self.assertEqual(inputs["mode"]["required"], "true")
        self.assertEqual(inputs["mode"]["default"], "patch")
        self.assertEqual(inputs["mode"]["options"], ["patch", "minor", "major"])
        self.assertEqual(build["concurrency"], {
            "group": "release-publication-${{ github.repository }}",
            "cancel-in-progress": "false",
        })
        for workflow in workflows:
            for job in workflow["jobs"].values():
                for step in job["steps"]:
                    if step.get("uses", "").startswith("actions/checkout@"):
                        self.assertEqual(step["with"]["ref"], "${{ github.sha }}")
        combined = "\n".join(path.read_text() for path in (ROOT / ".github/workflows").glob("*.yml"))
        self.assertNotIn("workflow run", combined)
        self.assertNotIn("repository_dispatch", combined)
        self.assertNotIn("workflow_run", combined)


if __name__ == "__main__":
    unittest.main()
