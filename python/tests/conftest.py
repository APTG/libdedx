import os
import sys
from pathlib import Path


def _candidate_library_paths(repo_root: Path) -> list[Path]:
    if sys.platform.startswith("linux"):
        lib_names = ["libdedx.so"]
    elif sys.platform == "darwin":
        lib_names = ["libdedx.dylib"]
    elif os.name == "nt":
        lib_names = ["dedx.dll", "libdedx.dll"]
    else:
        lib_names = ["libdedx.so"]

    candidates = []
    for build_dir in ("build", "build-debug", "build-release", "build-coverage"):
        for lib_name in lib_names:
            candidates.append(repo_root / build_dir / "src" / lib_name)
            candidates.append(repo_root / build_dir / "libdedx" / lib_name)
    return candidates


def pytest_configure() -> None:
    if os.environ.get("LIBDEDX_SO"):
        return

    repo_root = Path(__file__).resolve().parents[2]
    for candidate in _candidate_library_paths(repo_root):
        if candidate.exists():
            os.environ["LIBDEDX_SO"] = str(candidate)
            return
