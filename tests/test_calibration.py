import pytest
from syrupy.assertion import SnapshotAssertion

from gauge import calibration, common
from tests import utils

EXPECTED_WIDTH = utils.FRAME_WIDTH
EXPECTED_HEIGHT = utils.FRAME_HEIGHT


@pytest.mark.parametrize(
    ("source_images_dir",),
    [
        ["calibration/1"],
    ],
)
def test_calculate_background(
    source_images_dir: str,
    fixtures: utils.Fixtures,
    snap_json: SnapshotAssertion,
    save_example: utils.ExampleSaver,
) -> None:
    frames = common.Frame.from_images_dir(fixtures.fpath(source_images_dir))

    got = calibration.calculate_background(frames)
    save_example(got)

    assert got == snap_json


@pytest.mark.parametrize(
    (
        "source_image_path",
        "background_path",
    ),
    [
        (
            "calibration/1/767591132.jpg",
            "calibration/1/background.json",
        ),
        (
            "calibration/1/12756622577.jpg",
            "calibration/1/background.json",
        ),
    ],
)
def test_subtract_background(
    source_image_path: str,
    background_path: str,
    fixtures: utils.Fixtures,
    snap_json: SnapshotAssertion,
    save_example: utils.ExampleSaver,
) -> None:
    with fixtures.open_image(source_image_path) as img:
        frame = common.Frame.from_image(img)
    bg = fixtures.load_frame(background_path)

    got = calibration.subtract_background(frame, bg)
    save_example(got)

    assert got == snap_json


@pytest.mark.parametrize(
    "frame_path",
    [
        "calibration/1/767591132_subtracted.json",
        "calibration/1/12756622577_subtracted.json",
    ],
)
def test_binarize(
    frame_path: str,
    fixtures: utils.Fixtures,
    snap_json: SnapshotAssertion,
    save_example: utils.ExampleSaver,
) -> None:
    frame = fixtures.load_frame(frame_path)

    got = calibration.binarize(frame)
    save_example(utils.unbinarize(got))

    assert got == snap_json


@pytest.mark.parametrize(
    "frame_path",
    [
        "calibration/1/767591132_binarized.json",
        "calibration/1/12756622577_binarized.json",
    ],
)
def test_extract_largest_blob_returns_expected(
    frame_path: str,
    fixtures: utils.Fixtures,
    snap_json: SnapshotAssertion,
    save_example: utils.ExampleSaver,
) -> None:
    frame = fixtures.load_frame(frame_path)

    got = calibration.extract_largest_blob(frame)

    assert got is not None
    save_example(utils.unbinarize(got))

    assert got == snap_json


@pytest.mark.parametrize(
    ("frame_path", "source_image_path"),
    [
        ("calibration/1/767591132_largest.json", "calibration/1/767591132.jpg"),
        ("calibration/1/12756622577_largest.json", "calibration/1/12756622577.jpg"),
    ],
)
def test_blob_to_line(
    frame_path: str,
    source_image_path: str,
    fixtures: utils.Fixtures,
    snap_json: SnapshotAssertion,
    save_example: utils.ExampleSaver,
) -> None:
    frame = fixtures.load_frame(frame_path)

    got = calibration.blob_to_line(frame)

    save_example(utils.draw_line(utils.unbinarize(frame), got), "on_largest")
    source_image = fixtures.open_image(source_image_path)
    save_example(utils.draw_line(source_image, got), "on_source")

    assert got == snap_json
