import pytest

from gauge import calibration, common
from tests import utils


@pytest.mark.parametrize(
    ("img_fname", "exp_fname", "_ex_jpeg"),
    [
        (
            "calibration/1/767591132.jpg",
            "calibration/1/subtracted_767591132.json",
            "calibration/1/examples/subtracted_767591132.jpg",
        ),
        (
            "calibration/1/12756622577.jpg",
            "calibration/1/subtracted_12756622577.json",
            "calibration/1/examples/subtracted_12756622577.jpg",
        ),
    ],
)
def test_returns_expected_frame(
    img_fname: str, exp_fname: str, _ex_jpeg: str, fixtures: utils.Fixtures
) -> None:
    with fixtures.open_image(img_fname) as img:
        frame = common.Frame.from_image(img)
    bg = common.Frame(
        buf=tuple(fixtures.read_json("calibration/1/background.json", list[int])),
        width=frame.width,
        height=frame.height,
    )
    expected_frame = fixtures.read_json(exp_fname, list[int])

    got = calibration.subtract_background(frame, bg)

    # with open(fixtures.fpath(exp_fname), "w") as f:
    #     json.dump(got.buf, f)
    # img = got.to_jpeg()
    # img.save(fixtures.fpath(ex_jpeg))

    assert got.buf == tuple(expected_frame)
    assert bg.width == 320
    assert bg.height == 240
