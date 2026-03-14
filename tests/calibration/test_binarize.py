import pytest

from gauge import calibration, common
from tests import utils


@pytest.mark.parametrize(
    ("frame_path", "exp_path", "_ex_jpeg"),
    [
        (
            "calibration/1/subtracted_767591132.json",
            "calibration/1/binarized_767591132.json",
            "calibration/1/examples/binarized_767591132.jpg",
        ),
        (
            "calibration/1/subtracted_12756622577.json",
            "calibration/1/binarized_12756622577.json",
            "calibration/1/examples/binarized_12756622577.jpg",
        ),
    ],
)
def test_returns_expected_frame(
    frame_path: str, exp_path: str, _ex_jpeg: str, fixtures: utils.Fixtures
) -> None:
    frame = common.Frame(
        buf=tuple(fixtures.read_json(frame_path, list[int])), width=320, height=240
    )
    expected_buf = tuple(fixtures.read_json(exp_path, list[int]))

    got = calibration.binarize(frame)

    # with open(fixtures.fpath(exp_path), "w") as f:
    #     json.dump(got.buf, f)
    # img = got.to_jpeg()
    # img.save(fixtures.fpath(_ex_jpeg))

    assert got.buf == expected_buf
