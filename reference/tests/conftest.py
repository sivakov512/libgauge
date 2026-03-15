from pathlib import Path

import pytest
from syrupy.assertion import SnapshotAssertion

from . import utils


@pytest.fixture
def snap_json(snapshot: SnapshotAssertion) -> SnapshotAssertion:
    return snapshot.use_extension(utils.JSONSnapshotExtension)


@pytest.fixture
def project_root() -> Path:
    return Path(__file__).parent.parent


@pytest.fixture
def fixtures(project_root: Path) -> utils.Fixtures:
    return utils.Fixtures(root=project_root / "tests/fixtures")


def pytest_addoption(parser: pytest.Parser) -> None:
    parser.addoption("--save-examples", action="store_true", default=False)


@pytest.fixture
def save_example(request: pytest.FixtureRequest) -> utils.ExampleSaver:
    return utils.ExampleSaver(request)
