import math

from gauge import calibration, common

RADIAL_SCAN_STEP = math.radians(0.5)


def _normalize_angle(angle: float, direction: int | None) -> float:
    while angle > math.pi:
        angle -= 2 * math.pi
    while angle < -math.pi:
        angle += 2 * math.pi

    if direction is not None:
        if direction > 0 and angle < 0:
            angle += 2 * math.pi
        elif direction < 0 and angle > 0:
            angle -= 2 * math.pi

    return angle


def scan_angle(frame: common.Frame, ca_data: calibration.CalibrationData) -> float:
    scan_diff = _normalize_angle(
        ca_data.angle_max - ca_data.angle_min, ca_data.direction
    )

    angle_step = RADIAL_SCAN_STEP * ca_data.direction

    best_angle = ca_data.angle_min
    best_score = -1

    angle = ca_data.angle_min
    steps = int(abs(scan_diff) / RADIAL_SCAN_STEP) + 1
    for _ in range(steps):
        score = 0
        cos_a = math.cos(angle)
        sin_a = math.sin(angle)

        for r in range(int(ca_data.length)):
            x = int(ca_data.pivot_x + r * cos_a)
            y = int(ca_data.pivot_y + r * sin_a)
            if 0 <= x < frame.width and 0 <= y < frame.height:
                _, value = frame.pixel(x, y)
                score += 255 - value

        if score > best_score:
            best_score = score
            best_angle = angle

        print(f"angle={math.degrees(angle):.2f} score={score}")
        angle += angle_step

    return _normalize_angle(best_angle, None)
