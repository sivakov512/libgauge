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

RED_PIXEL = (255, 0, 0)
GREEN_PIXEL = (0, 255, 0)

LINE_SIDE_THICKNESS = 1
POINT_THICKNESS = 3

_JSON_T = typing.TypeVar("_JSON_T")


@dataclass
class Fixtures:
    root: Path

    def fpath(self, path: str) -> Path:
        return self.root / path

    def open_image(self, path: str) -> ImageFile.ImageFile:
        return Image.open(self.fpath(path))

    def read_json(self, path: str, _type: type[_JSON_T]) -> _JSON_T:
        return typing.cast(_JSON_T, json.loads(self.fpath(path).read_text()))

    def load_frame(self, path: str) -> common.Frame:
        data = self.read_json(path, dict[str, list[int] | int])
        return common.Frame(
            buf=tuple(typing.cast(list[int], data["buf"])),
            width=typing.cast(int, data["width"]),
            height=typing.cast(int, data["height"]),
        )

    def load_line(self, path: str) -> common.Line:
        data = self.read_json(path, dict[str, float])
        return common.Line(mx=data["mx"], my=data["my"], vx=data["vx"], vy=data["vy"])


def unbinarize(frame: common.Frame) -> common.Frame:
    return common.Frame(
        buf=tuple(255 if el == calibration.BINARIZE_UP else el for el in frame.buf),
        width=frame.width,
        height=frame.height,
    )


def draw_line(frame: common.Frame | Image.Image, line: common.Line) -> Image.Image:
    img = (frame.to_jpeg() if isinstance(frame, common.Frame) else frame).convert("RGB")

    for offset in range(-LINE_SIDE_THICKNESS, LINE_SIDE_THICKNESS + 1):
        for t in range(-max(frame.width, frame.height), max(frame.width, frame.height)):
            x = int(line.mx + t * line.vx + offset * (-line.vy))
            y = int(line.my + t * line.vy + offset * line.vx)
            if 0 <= x < frame.width and 0 <= y <= frame.height:
                img.putpixel((x, y), RED_PIXEL)

    for dy in range(-POINT_THICKNESS, POINT_THICKNESS + 1):
        for dx in range(-POINT_THICKNESS, POINT_THICKNESS + 1):
            x = int(line.mx + dx)
            y = int(line.my + dy)
            if 0 <= x < frame.width and 0 <= y <= frame.height:
                img.putpixel((x, y), GREEN_PIXEL)

    return img


def draw_point(frame: common.Frame | Image.Image, x: float, y: float) -> Image.Image:
    img = (frame.to_jpeg() if isinstance(frame, common.Frame) else frame).convert("RGB")

    for dy in range(-POINT_THICKNESS, POINT_THICKNESS + 1):
        for dx in range(-POINT_THICKNESS, POINT_THICKNESS + 1):
            draw_x = int(x + dx)
            draw_y = int(y + dy)
            if 0 <= draw_x < frame.width and 0 <= draw_y <= frame.height:
                img.putpixel((draw_x, draw_y), GREEN_PIXEL)

    return img


@dataclass
class ExampleSaver:
    _request: pytest.FixtureRequest

    def __call__(
        self,
        frame: common.Frame | Image.Image | typing.Any,
        name_postfix: str | None = None,
    ) -> None:
        if not self._request.config.getoption("--save-examples"):
            return
        test_file = self._request.path
        examples_dir = test_file.parent / "__examples__" / test_file.stem
        examples_dir.mkdir(parents=True, exist_ok=True)
        safe_name = self._request.node.name.replace("/", "")  # pyright: ignore[reportUnknownVariableType, reportUnknownMemberType]

        if name_postfix is not None:
            safe_name = f"{safe_name}_{name_postfix}"

        if isinstance(frame, common.Frame):
            frame.to_jpeg().save(examples_dir / f"{safe_name}.jpg")
        elif isinstance(frame, Image.Image):
            frame.save(examples_dir / f"{safe_name}.jpg")
        else:
            raise TypeError(f"Unsupported frame type: {type(frame)}")  # pyright: ignore[reportAny]


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
