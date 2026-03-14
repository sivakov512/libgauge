from gauge import common

BINARIZE_THRESHOLD = 20
BINARIZE_UP = 1
BINARIZE_DOWN = 0


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
