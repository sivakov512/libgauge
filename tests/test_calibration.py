import pytest

from gauge import calibration, common
from tests import utils

EXPECTED_WIDTH = utils.FRAME_WIDTH
EXPECTED_HEIGHT = utils.FRAME_HEIGHT


@pytest.mark.parametrize(
    ("source_images_dir", "exp_path", "update_artifacts", "example_jpeg_path"),
    [
        (
            "calibration/1",
            "calibration/1/background.json",
            False,
            "calibration/1/examples/background.jpg",
        ),
    ],
)
def test_calculate_background(
    source_images_dir: str,
    exp_path: str,
    update_artifacts: bool,
    example_jpeg_path: str,
    fixtures: utils.Fixtures,
) -> None:
    frames = common.Frame.from_images_dir(fixtures.fpath(source_images_dir))
    expected_bg = fixtures.read_json(exp_path, list[int])

    bg = calibration.calculate_background(frames)

    if update_artifacts:
        fixtures.save_image(example_jpeg_path, bg.to_jpeg())

    assert bg.buf == tuple(expected_bg)
    assert bg.width == EXPECTED_WIDTH
    assert bg.height == EXPECTED_HEIGHT


@pytest.mark.parametrize(
    (
        "source_image_path",
        "background_path",
        "exp_path",
        "update_artifacts",
        "example_jpeg_path",
    ),
    [
        (
            "calibration/1/767591132.jpg",
            "calibration/1/background.json",
            "calibration/1/subtracted_767591132.json",
            False,
            "calibration/1/examples/subtracted_767591132.jpg",
        ),
        (
            "calibration/1/12756622577.jpg",
            "calibration/1/background.json",
            "calibration/1/subtracted_12756622577.json",
            False,
            "calibration/1/examples/subtracted_12756622577.jpg",
        ),
    ],
)
def test_subtract_background(
    source_image_path: str,
    background_path: str,
    exp_path: str,
    update_artifacts: bool,
    example_jpeg_path: str,
    fixtures: utils.Fixtures,
) -> None:
    with fixtures.open_image(source_image_path) as img:
        frame = common.Frame.from_image(img)
    bg = fixtures.read_frame(background_path)
    expected_frame = fixtures.read_json(exp_path, list[int])

    got = calibration.subtract_background(frame, bg)

    if update_artifacts:
        fixtures.write_json(exp_path, got.buf, tuple[int, ...])
        fixtures.save_image(example_jpeg_path, got.to_jpeg())

    assert got.buf == tuple(expected_frame)
    assert bg.width == EXPECTED_WIDTH
    assert bg.height == EXPECTED_HEIGHT


@pytest.mark.parametrize(
    ("frame_path", "exp_path", "update_artifacts", "example_jpeg_path"),
    [
        (
            "calibration/1/subtracted_767591132.json",
            "calibration/1/binarized_767591132.json",
            False,
            "calibration/1/examples/binarized_767591132.jpg",
        ),
        (
            "calibration/1/subtracted_12756622577.json",
            "calibration/1/binarized_12756622577.json",
            False,
            "calibration/1/examples/binarized_12756622577.jpg",
        ),
    ],
)
def test_binarize(
    frame_path: str,
    exp_path: str,
    update_artifacts: bool,
    example_jpeg_path: str,
    fixtures: utils.Fixtures,
) -> None:
    frame = fixtures.read_frame(frame_path)
    expected_buf = tuple(fixtures.read_json(exp_path, list[int]))

    got = calibration.binarize(frame)

    if update_artifacts:
        fixtures.write_json(exp_path, got.buf, tuple[int, ...])
        jpeg_friendly = common.Frame(
            buf=tuple(255 if el == calibration.BINARIZE_UP else el for el in got.buf),
            width=got.width,
            height=got.height,
        )
        fixtures.save_image(example_jpeg_path, jpeg_friendly.to_jpeg())

    assert got.buf == expected_buf


@pytest.mark.parametrize(
    ("frame_path", "exp_path", "update_artifacts", "example_jpeg_path"),
    [
        (
            "calibration/1/binarized_767591132.json",
            "calibration/1/largest_blob_767591132.json",
            False,
            "calibration/1/examples/largest_blob_767591132.jpg",
        ),
        (
            "calibration/1/binarized_12756622577.json",
            "calibration/1/largest_blob_12756622577.json",
            False,
            "calibration/1/examples/largest_blob_12756622577.jpg",
        ),
    ],
)
def test_extract_largest_blob_returns_expected(
    frame_path: str,
    exp_path: str,
    update_artifacts: bool,
    example_jpeg_path: str,
    fixtures: utils.Fixtures,
) -> None:
    frame = fixtures.read_frame(frame_path)
    expected_buf = tuple(fixtures.read_json(exp_path, list[int]))

    got = calibration.extract_largest_blob(frame)

    assert got is not None

    if update_artifacts:
        fixtures.write_json(exp_path, got.buf, tuple[int, ...])
        jpeg_friendly = common.Frame(
            buf=tuple(255 if el == calibration.BINARIZE_UP else el for el in got.buf),
            width=got.width,
            height=got.height,
        )
        fixtures.save_image(example_jpeg_path, jpeg_friendly.to_jpeg())

    assert got.buf == expected_buf
