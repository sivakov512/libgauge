import json
import typing
from dataclasses import dataclass
from pathlib import Path

from PIL import Image, ImageFile

from gauge import common

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
