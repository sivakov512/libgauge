from gauge import common

BINARIZE_THRESHOLD = 20
BINARIZE_UP = 1
BINARIZE_DOWN = 0

NEIGHBORHOOD_8 = ((-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (1, -1), (-1, 1), (1, 1))


def calculate_background(frames: list[common.Frame]) -> common.Frame:
    buf = list(frames[0].buf)

    for frame in frames[1:]:
        for i in range(frame.length):
            buf[i] = max(buf[i], frame.buf[i])

    return common.Frame(
        buf=tuple(buf),
        width=frames[0].width,
        height=frames[0].height,
    )


def subtract_background(frame: common.Frame, bg: common.Frame) -> common.Frame:
    buf = [0] * frame.length

    for i in range(frame.length):
        buf[i] = abs(frame.buf[i] - bg.buf[i])

    return common.Frame(
        buf=tuple(buf),
        width=frame.width,
        height=frame.height,
    )


def binarize(frame: common.Frame) -> common.Frame:
    buf = [0] * frame.length

    for i in range(frame.length):
        buf[i] = BINARIZE_UP if frame.buf[i] > BINARIZE_THRESHOLD else BINARIZE_DOWN

    return common.Frame(
        buf=tuple(buf),
        width=frame.width,
        height=frame.height,
    )


def _flood_fill(
    frame: common.Frame, x: int, y: int, label: int, labels: list[int]
) -> int:
    stack = [(x, y)]
    size = 0

    while stack:
        x, y = stack.pop()
        index, value = frame.pixel(x, y)

        if labels[index] != BINARIZE_DOWN or value == BINARIZE_DOWN:
            continue

        labels[index] = label
        size += 1

        for dx, dy in NEIGHBORHOOD_8:
            nx, ny = x + dx, y + dy
            if 0 <= nx < frame.width and 0 <= ny < frame.height:
                stack.append((nx, ny))

    return size


def extract_largest_blob(frame: common.Frame) -> common.Frame | None:
    labels = [BINARIZE_DOWN] * frame.length
    label = 1
    sizes: dict[int, int] = {}

    for y in range(frame.height):
        for x in range(frame.width):
            index, value = frame.pixel(x, y)

            if value == BINARIZE_UP and labels[index] == BINARIZE_DOWN:
                size = _flood_fill(frame, x, y, label, labels)
                sizes[label] = size
                label += 1

    if not sizes:
        return None

    max_label = max(sizes, key=lambda k: sizes[k])

    buf = [
        BINARIZE_UP if labels[i] == max_label else BINARIZE_DOWN
        for i in range(frame.length)
    ]

    return common.Frame(
        buf=tuple(buf),
        width=frame.width,
        height=frame.height,
    )
