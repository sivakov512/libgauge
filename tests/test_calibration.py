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

    example = utils.draw_line(utils.unbinarize(frame), got)
    example = utils.draw_point(example, got.mx, got.my)
    save_example(example, "on_largest")
    source_image = fixtures.open_image(source_image_path)
    example = utils.draw_line(source_image, got)
    example = utils.draw_point(example, got.mx, got.my)
    save_example(example, "on_source")

    assert got == snap_json


@pytest.mark.parametrize(
    ("line1_path", "line2_path", "source_images_path"),
    [
        (
            "calibration/1/767591132_line.json",
            "calibration/1/12756622577_line.json",
            ("calibration/1/767591132.jpg", "calibration/1/12756622577.jpg"),
        ),
    ],
)
def test_intersect(
    line1_path: str,
    line2_path: str,
    source_images_path: tuple[str, ...],
    fixtures: utils.Fixtures,
    snap_json: SnapshotAssertion,
    save_example: utils.ExampleSaver,
) -> None:
    line1 = fixtures.load_line(line1_path)
    line2 = fixtures.load_line(line2_path)

    got = calibration.intersect(line1, line2)

    assert got is not None

    for i, source_image_path in enumerate(source_images_path):
        source_image = fixtures.open_image(source_image_path)
        save_example(utils.draw_point(source_image, *got), f"on_{i}")

    assert got == snap_json


@pytest.mark.parametrize(
    ("line1_path", "line2_path"),
    [
        (
            "calibration/1/767591132_line.json",
            "calibration/1/12756622577_line.json",
        ),
    ],
)
def test_intersect_doesnt_depend_on_order(
    line1_path: str,
    line2_path: str,
    fixtures: utils.Fixtures,
) -> None:
    line1 = fixtures.load_line(line1_path)
    line2 = fixtures.load_line(line2_path)

    got = calibration.intersect(line1, line2)
    got_reversed = calibration.intersect(line2, line1)

    assert got == got_reversed


@pytest.mark.parametrize(
    ("source_images_dir", "source_images_path"),
    [
        [
            "calibration/1",
            ("calibration/1/767591132.jpg", "calibration/1/12756622577.jpg"),
        ],
    ],
)
def test_calculate(
    source_images_dir: str,
    source_images_path: tuple[str, ...],
    fixtures: utils.Fixtures,
    snap_json: SnapshotAssertion,
    save_example: utils.ExampleSaver,
) -> None:
    frames = common.Frame.from_images_dir(fixtures.fpath(source_images_dir))

    got = calibration.calculate(frames)

    assert got is not None

    for i, source_image_path in enumerate(source_images_path):
        source_image = fixtures.open_image(source_image_path)
        save_example(utils.draw_calibration_data(source_image, got), f"on_{i}")

    assert got == snap_json
