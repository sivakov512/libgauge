from gauge import common

BINARIZE_THRESHOLD = 20
BINARIZE_UP = 1
BINARIZE_DOWN = 0


def calculate_background_frame(frames: list[common.Frame]) -> common.Frame:
    bg_frame = list(frames[0].buf)

    for frame in frames[1:]:
        for i in range(frame.length):
            bg_frame[i] = max(bg_frame[i], frame.buf[i])

    return common.Frame(
        buf=tuple(bg_frame),
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
