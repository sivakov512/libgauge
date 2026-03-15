import pytest
from syrupy.assertion import SnapshotAssertion

from gauge import common, scanning
from tests import utils

CA_DATA_1_PATH = "set/1/ca_data.json"


@pytest.mark.parametrize(
    "source_image_path, ca_data_path",
    [
        ("set/1/767591132.jpg", CA_DATA_1_PATH),
        ("set/1/1356622581.jpg", CA_DATA_1_PATH),
        ("set/1/1956622578.jpg", CA_DATA_1_PATH),
        ("set/1/2556622581.jpg", CA_DATA_1_PATH),
        ("set/1/3156622580.jpg", CA_DATA_1_PATH),
        ("set/1/3756622578.jpg", CA_DATA_1_PATH),
        ("set/1/4356622579.jpg", CA_DATA_1_PATH),
        ("set/1/4956622579.jpg", CA_DATA_1_PATH),
        ("set/1/5556622577.jpg", CA_DATA_1_PATH),
        ("set/1/6156622580.jpg", CA_DATA_1_PATH),
        ("set/1/6756622581.jpg", CA_DATA_1_PATH),
        ("set/1/7356622579.jpg", CA_DATA_1_PATH),
        ("set/1/7956622579.jpg", CA_DATA_1_PATH),
        ("set/1/8556622580.jpg", CA_DATA_1_PATH),
        ("set/1/9156622578.jpg", CA_DATA_1_PATH),
        ("set/1/9756622581.jpg", CA_DATA_1_PATH),
        ("set/1/10356622581.jpg", CA_DATA_1_PATH),
        ("set/1/10956622583.jpg", CA_DATA_1_PATH),
        ("set/1/11556622580.jpg", CA_DATA_1_PATH),
        ("set/1/12156622581.jpg", CA_DATA_1_PATH),
        ("set/1/12756622577.jpg", CA_DATA_1_PATH),
    ],
)
def test_scan_scale(
    source_image_path: str,
    ca_data_path: str,
    fixtures: utils.Fixtures,
    snap_json: SnapshotAssertion,
    save_example: utils.ExampleSaver,
) -> None:
    source_image = fixtures.open_image(source_image_path)
    frame = common.Frame.from_image(source_image)
    ca_data = fixtures.load_ca_data(ca_data_path)

    angle = scanning.scan_angle(frame, ca_data)

    example = utils.draw_vector(frame, (ca_data.pivot_x, ca_data.pivot_y), angle)
    example = utils.draw_point(example, ca_data.pivot_x, ca_data.pivot_y)
    save_example(example)

    assert angle == snap_json
