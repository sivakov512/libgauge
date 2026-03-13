import json
import typing
from dataclasses import dataclass
from pathlib import Path

from PIL import Image, ImageFile

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
