import os
from pathlib import Path


def _candidate_library_paths(repo_root: Path) -> list[Path]:
    return [
        repo_root / "build" / "libdedx" / "libdedx.so",
        repo_root / "build-debug" / "libdedx" / "libdedx.so",
        repo_root / "build-release" / "libdedx" / "libdedx.so",
        repo_root / "build-coverage" / "libdedx" / "libdedx.so",
    ]


def pytest_configure() -> None:
    if os.environ.get("LIBDEDX_SO"):
        return

    repo_root = Path(__file__).resolve().parents[2]
    for candidate in _candidate_library_paths(repo_root):
        if candidate.exists():
            os.environ["LIBDEDX_SO"] = str(candidate)
            return
