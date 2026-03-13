from pathlib import Path

import pytest

from . import utils


@pytest.fixture
def project_root() -> Path:
    return Path(__file__).parent.parent


@pytest.fixture
def fixtures(project_root: Path) -> utils.Fixtures:
    return utils.Fixtures(root=project_root / "tests/fixtures")
