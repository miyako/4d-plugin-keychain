"""Actual JSON helpers, local wrappers and complete unchanged SDK; synthetic host only."""
import hashlib
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[2]
SOURCE = Path(os.environ.get("OWNERSHIP_SOURCE_ROOT", ROOT)).resolve()
SDK_BLOBS = {
    "4DPluginAPI.c": "62f745e91d81ef2c3c901c656ea8e175d3b173e8",
    "4DPluginAPI.h": "528bf1d2552df08d6a8b1bc95011a9b72934ad5c",
    "EntryPoints.h": "da428d5f6ca23b0a30371c7288a01d29aa3c466a",
    "Flags.h": "7a5a35539f27430a3fd9cb711d6b789051316471",
    "PrivateTypes.h": "a3098cb662cdbc43ba574fdef72a059c4c6d1cd4",
    "PublicTypes.h": "9ae20b001bff9128f690fff3e6c3f873b6238333",
}
CASES = (
    "get-a", "get-b", "get-n", "get-o", "get-c",
    "mismatch-a", "mismatch-b", "mismatch-n", "mismatch-o", "mismatch-c",
    "get-failure", "has-failure", "read-failure",
    "stringify", "stringify-mismatch", "stringify-handle-failure",
    "stringify-failure", "stringify-read-failure", "setters",
    "create-value-failure", "create-key-failure", "getter-create-failure",
    "set-failure", "scalar", "arrays",
)


@unittest.skipUnless(sys.platform == "darwin", "macOS synthetic framework host")
class OwnershipTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temp = tempfile.TemporaryDirectory(prefix="keychain-ownership-")
        cls.binary = Path(cls.temp.name) / "ownership"
        api = SOURCE / "4D Plugin API"
        for name, wanted in SDK_BLOBS.items():
            data = (api / name).read_bytes()
            actual = hashlib.sha1(f"blob {len(data)}\0".encode() + data).hexdigest()
            if actual != wanted:
                cls.temp.cleanup()
                raise AssertionError(f"Re-audit changed SDK: {name}")
        command = [
            "clang++", "-std=c++14", "-x", "objective-c++",
            "-include", "Carbon/Carbon.h", "-I", str(api),
            "-I", str(api / "Classes"), "-I", str(SOURCE),
            "-fsanitize=address,undefined", "-fno-sanitize=alignment",
            "-fno-omit-frame-pointer", "-g",
            "-Wno-deprecated-declarations", "-Wno-return-type-c-linkage",
            "-Wno-ignored-attributes", "-Wno-writable-strings",
            str(api / "4DPluginAPI.c"),
            *map(str, sorted((api / "Classes").glob("*.cpp"))),
            str(SOURCE / "support/4DPlugin-JSON.cpp"),
            str(ROOT / "tests/ownership/host.mm"),
            "-framework", "Cocoa", "-framework", "Carbon",
            "-o", str(cls.binary),
        ]
        result = subprocess.run(command, capture_output=True, text=True)
        if result.returncode:
            cls.temp.cleanup()
            raise AssertionError(result.stdout + result.stderr)
        linked = subprocess.run(
            ["otool", "-L", str(cls.binary)], capture_output=True, text=True, check=True,
        ).stdout
        symbols = subprocess.run(
            ["nm", "-u", str(cls.binary)], capture_output=True, text=True, check=True,
        ).stdout
        if "Security.framework" in linked or any(
            line.split()[-1].startswith("_Sec") for line in symbols.splitlines() if line.split()
        ):
            cls.temp.cleanup()
            raise AssertionError("Credential platform boundary must not be linked")

    @classmethod
    def tearDownClass(cls):
        cls.temp.cleanup()

    def run_case(self, case):
        for fixture in ("ASCII", "\u65e5", "\U0001f600", ""):
            for empty in ("allocated", "null"):
                with self.subTest(case=case, fixture=fixture, empty=empty):
                    result = subprocess.run(
                        [str(self.binary), case, fixture, empty],
                        env={**os.environ, "ASAN_OPTIONS": "detect_leaks=0",
                             "UBSAN_OPTIONS": "halt_on_error=1"},
                        capture_output=True, text=True,
                    )
                    self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
                    self.assertIn("strings=0 handles=0 refs=0", result.stdout)


def make_test(case):
    def test(self):
        self.run_case(case)
    return test


for case in CASES:
    setattr(OwnershipTests, "test_" + case.replace("-", "_"), make_test(case))


if __name__ == "__main__":
    unittest.main()
