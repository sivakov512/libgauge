from gauge.common import Frame
from tests import utils


def test_from_image_returns_expected_frame(fixtures: utils.Fixtures) -> None:
    expected_frame = fixtures.read_json("common/sample_grayscaled_1.json", list[int])

    with fixtures.open_image("common/sample_1.jpg") as img:
        fb = Frame.from_image(img)

    assert fb.buf == tuple(expected_frame)
    assert fb.width == 320
    assert fb.height == 240


def test_from_images_dir_returns_expected_frames(
    fixtures: utils.Fixtures,
) -> None:
    expected_frame = fixtures.read_json("common/sample_grayscaled_1.json", list[int])

    frames = Frame.from_images_dir(fixtures.fpath("common"))

    assert frames[0].buf == tuple(expected_frame)
    assert frames[0].width == 320
    assert frames[0].height == 240


def test_from_images_dir_returns_expected_frames_count(
    fixtures: utils.Fixtures,
) -> None:
    frames = Frame.from_images_dir(fixtures.fpath("calibration/1"))

    assert len(frames) == 21


def test_to_jpeg_returns_expected_image(fixtures: utils.Fixtures) -> None:
    with fixtures.open_image("common/sample_1.jpg") as original:
        fb = Frame.from_image(original)

        jpeg = fb.to_jpeg()

        assert jpeg.get_flattened_data() == original.get_flattened_data()
