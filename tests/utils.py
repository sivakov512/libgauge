import json
import typing
from dataclasses import dataclass
from pathlib import Path

import orjson
import pytest
import syrupy.extensions.json
import syrupy.extensions.single_file
import syrupy.types
from PIL import Image, ImageFile

from gauge import calibration, common

FRAME_WIDTH = 320
FRAME_HEIGHT = 240

_JSON_T = typing.TypeVar("_JSON_T")


@dataclass
class Fixtures:
    root: Path

    def fpath(self, path: str) -> Path:
        return self.root / path

    def open_image(self, path: str) -> ImageFile.ImageFile:
        return Image.open(self.fpath(path))

    def save_image(self, path: str, image: Image.Image) -> None:
        image.save(self.fpath(path))

    def read_json(self, path: str, _type: type[_JSON_T]) -> _JSON_T:
        return typing.cast(_JSON_T, json.loads(self.fpath(path).read_text()))

    def write_json(self, path: str, data: _JSON_T, _type: type[_JSON_T]) -> None:
        _ = self.fpath(path).write_text(json.dumps(data))

    def read_frame(self, path: str) -> common.Frame:
        return common.Frame(
            buf=tuple(self.read_json(path, list[int])),
            width=FRAME_WIDTH,
            height=FRAME_HEIGHT,
        )

    def load_frame(self, path: str) -> common.Frame:
        data = self.read_json(path, dict[str, list[int] | int])
        return common.Frame(
            buf=tuple(typing.cast(list[int], data["buf"])),
            width=typing.cast(int, data["width"]),
            height=typing.cast(int, data["height"]),
        )


def unbinarize(frame: common.Frame) -> common.Frame:
    return common.Frame(
        buf=tuple(255 if el == calibration.BINARIZE_UP else el for el in frame.buf),
        width=frame.width,
        height=frame.height,
    )


@dataclass
class ExampleSaver:
    _request: pytest.FixtureRequest

    def __call__(self, frame: common.Frame) -> None:
        if not self._request.config.getoption("--save-examples"):
            return
        test_file = self._request.path
        examples_dir = test_file.parent / "__examples__" / test_file.stem
        examples_dir.mkdir(parents=True, exist_ok=True)
        safe_name = self._request.node.name.replace("/", "")  # pyright: ignore[reportUnknownVariableType, reportUnknownMemberType]

        frame.to_jpeg().save(examples_dir / f"{safe_name}.jpg")


@typing.final
class JSONSnapshotExtension(syrupy.extensions.json.JSONSnapshotExtension):
    _write_mode = syrupy.extensions.single_file.WriteMode.BINARY

    @typing.override
    def serialize(
        self,
        data: syrupy.types.SerializableData,  # pyright: ignore[reportAny]
        **kwargs: typing.Any,  # pyright: ignore[reportAny]
    ) -> syrupy.types.SerializedData:
        return orjson.dumps(data)

    @typing.override
    def matches(
        self,
        *,
        serialized_data: syrupy.types.SerializedData,
        snapshot_data: syrupy.types.SerializedData,
    ) -> bool:
        return bool(orjson.loads(serialized_data) == orjson.loads(snapshot_data))  # pyright: ignore[reportAny]
