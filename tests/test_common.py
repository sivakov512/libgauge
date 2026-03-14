from syrupy.assertion import SnapshotAssertion

from gauge.common import Frame
from tests import utils


def test_frame_from_image_returns_expected_frame(
    fixtures: utils.Fixtures, snap_json: SnapshotAssertion
) -> None:
    with fixtures.open_image("common/sample_1.jpg") as img:
        fb = Frame.from_image(img)

    assert fb == snap_json


def test_frame_from_images_dir_returns_expected_frames(
    fixtures: utils.Fixtures, snap_json: SnapshotAssertion
) -> None:
    frames = Frame.from_images_dir(fixtures.fpath("common"))

    assert frames[0] == snap_json


def test_frame_from_images_dir_returns_expected_frames_count(
    fixtures: utils.Fixtures,
) -> None:
    frames = Frame.from_images_dir(fixtures.fpath("calibration/1"))

    assert len(frames) == 21


def test_frame_to_jpeg_returns_expected_image(fixtures: utils.Fixtures) -> None:
    with fixtures.open_image("common/sample_1.jpg") as original:
        fb = Frame.from_image(original)

        jpeg = fb.to_jpeg()

    assert jpeg.get_flattened_data() == original.get_flattened_data()
