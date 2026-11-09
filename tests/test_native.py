"""Selective output invalidation and symlink prevalidation without native execution."""

import shutil

from helper_support import Fixture, cwd
import native


class NativePreparationTests(Fixture):
    def test_selective_cleanup_preserves_all_siblings_and_packaged_resources(self):
        protected = ("Windows64/Keychain Access.4DX", "MacOS/runtime.dylib",
                     "Resources/manifest.json", "Resources/constants.xlf", "QuickLook/Thumbnail.png")
        paths = []
        for bundle in native.OUTPUT_BUNDLES:
            for suffix in protected:
                path = bundle / "Contents" / suffix
                self.write(path, b"preserved")
                paths.append(path)
        for path in native.FILES:
            self.write(path, b"stale generated")
        for path in native.DIRECTORIES:
            self.write(path / "intermediate", b"stale generated")
        self.write("test/Plugins/Default/keep", b"keep")
        self.write("build/unrelated/keep", b"keep")
        with cwd(self.root):
            native.prepare()
            native.prepare()
        for path in paths:
            self.assertEqual((self.root / path).read_bytes(), b"preserved")
        self.assertEqual((self.root / "test/Plugins/Default/keep").read_bytes(), b"keep")
        self.assertEqual((self.root / "build/unrelated/keep").read_bytes(), b"keep")
        self.assertFalse(any((self.root / path).exists()
                             for path in (*native.FILES, *native.DIRECTORIES)))

    def test_symlink_leaf_or_parent_anywhere_fails_before_first_deletion(self):
        for where in (native.FILES[-1], native.DIRECTORIES[-1],
                      native.OUTPUT_BUNDLES[-1]):
            with self.subTest(where=where):
                first = self.write(native.FILES[0], b"must remain")
                destination = self.write("outside/keep", b"untouched")
                link = self.root / where
                if link.is_dir():
                    shutil.rmtree(link)
                link.parent.mkdir(parents=True, exist_ok=True)
                link.symlink_to(destination)
                with cwd(self.root), self.assertRaisesRegex(ValueError, "symlink"):
                    native.prepare()
                self.assertEqual(first.read_bytes(), b"must remain")
                self.assertEqual(destination.read_bytes(), b"untouched")
                link.unlink()

    def test_allowlist_exact_names_and_directory_boundaries(self):
        for path in (*native.FILES, *(path / "nested/file" for path in native.DIRECTORIES)):
            self.assertTrue(native.generated(str(path)))
        for path in ("build/other/file", "test/Plugins/Default/keep",
                     "test/Plugins/Keychain Access.bundle/Contents/MacOS/runtime.dylib",
                     "test/Plugins/Keychain Access.bundle/Contents/Resources/constants.xlf",
                     "test/Plugins/Keychain Access.bundle/Contents/_CodeSignature-evil/file"):
            self.assertFalse(native.generated(path))
