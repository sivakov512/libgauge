import math

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


def _center_of_mass(frame: common.Frame) -> tuple[float, float]:
    sum_x = 0
    sum_y = 0
    count = 0

    for y in range(frame.height):
        for x in range(frame.width):
            _, value = frame.pixel(x, y)

            if value == BINARIZE_UP:
                sum_x += x
                sum_y += y
                count += 1

    if count == 0:
        return 0, 0
    return sum_x / count, sum_y / count


def _principal_axis(frame: common.Frame, mx: float, my: float) -> tuple[float, float]:
    cxx = 0.0
    cxy = 0.0
    cyy = 0.0
    count = 0

    # Covariance matrix
    for y in range(frame.height):
        for x in range(frame.width):
            _, value = frame.pixel(x, y)
            if value == BINARIZE_UP:
                dx = x - mx
                dy = y - my
                cxx += dx * dx
                cxy += dx * dy
                cyy += dy * dy
                count += 1
    cxx /= count
    cxy /= count
    cyy /= count

    # lambda1
    trace = cxx + cyy
    det = cxx * cyy - cxy * cxy
    D = trace * trace - 4 * det
    sqrt_D = math.sqrt(max(0.0, D))
    lambda1 = (trace + sqrt_D) / 2.0

    # lambda1 vector
    if abs(cxy) > 1e-6:  # float usually not equal to zero
        vx = cxy
        vy = lambda1 - cxx
    else:
        if cxx > cyy:
            vx, vy = 1.0, 0.0
        else:
            vx, vy = 0.0, 1.0

    # normalize vector length
    length = math.sqrt(vx * vx + vy * vy)

    return vx / length, vy / length


def blob_to_line(frame: common.Frame) -> common.Line:
    mx, my = _center_of_mass(frame)
    vx, vy = _principal_axis(frame, mx, my)
    return common.Line(mx=mx, my=my, vx=vx, vy=vy)
