import os
import typing
from dataclasses import dataclass

import PIL.Image

StrPath = str | os.PathLike[str]


@dataclass
class Frame:
    buf: tuple[int, ...]
    width: int
    height: int

    @property
    def length(self) -> int:
        return len(self.buf)

    @classmethod
    def from_image(cls, image: PIL.Image.Image) -> typing.Self:
        grayscale = image.convert("L")

        return cls(
            buf=typing.cast(tuple[int, ...], grayscale.get_flattened_data()),
            width=grayscale.width,
            height=grayscale.height,
        )

    @classmethod
    def from_images_dir(cls, dir_path: StrPath) -> list[typing.Self]:
        fnames = sorted(
            el for el in os.listdir(dir_path) if el.endswith((".jpg", ".jpeg"))
        )

        frame_buffers: list[typing.Self] = []
        for fname in fnames:
            with PIL.Image.open(os.path.join(dir_path, fname)) as img:
                frame_buffers.append(cls.from_image(img))

        return frame_buffers

    def to_jpeg(self) -> PIL.Image.Image:
        return PIL.Image.frombytes("L", (self.width, self.height), bytes(self.buf))
