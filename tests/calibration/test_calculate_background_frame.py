from gauge import calibration, common
from tests import utils


def test_returns_expected_frame(fixtures: utils.Fixtures) -> None:
    expected_bg = fixtures.read_json("calibration/1/background.json", list[int])
    frames = common.Frame.from_images_dir(fixtures.fpath("calibration/1"))

    bg = calibration.calculate_background_frame(frames)

    # img = bg.to_jpeg()
    # img.save(fixtures.fpath("calibration/1/examples/background.jpg"))

    assert bg.buf == tuple(expected_bg)
    assert bg.width == 320
    assert bg.height == 240
