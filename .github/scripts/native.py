"""Invalidate only proven generated outputs, preserving all packaged resources."""

import argparse
from pathlib import Path
import shutil


PRODUCT = "Keychain Access"
BASE = Path("test/Plugins")
BUNDLE = BASE / (PRODUCT + ".bundle")
OUTPUT_BUNDLES = (BUNDLE, BASE / "Default" / (PRODUCT + ".bundle"))
FILES = tuple(bundle / relative for bundle in OUTPUT_BUNDLES for relative in (
    f"Contents/MacOS/{PRODUCT}", "Contents/Info.plist",
    "Contents/Resources/English.lproj/InfoPlist.strings",
))
DIRECTORIES = (
    *(bundle / "Contents/_CodeSignature" for bundle in OUTPUT_BUNDLES),
    BASE / (PRODUCT + ".build"), BASE / "Default" / (PRODUCT + ".build"),
    Path("build") / (PRODUCT + ".build"), Path("build/XCBuildData"),
)


def generated(path):
    return (path in set(map(str, FILES))
            or any(path.startswith(str(directory) + "/") for directory in DIRECTORIES))


def prepare():
    root = Path.cwd().resolve()
    for path in (*FILES, *DIRECTORIES):
        for parent in (path, *path.parents):
            if parent == Path("."):
                break
            if parent.is_symlink():
                raise ValueError(f"Refusing symlink output: {parent}")
        if not path.resolve().is_relative_to(root):
            raise ValueError(f"Output escapes checkout: {path}")
    for path in FILES:
        path.unlink(missing_ok=True)
    for path in DIRECTORIES:
        if path.exists():
            shutil.rmtree(path)


if __name__ == "__main__":
    argparse.ArgumentParser(description=__doc__).parse_args()
    prepare()
