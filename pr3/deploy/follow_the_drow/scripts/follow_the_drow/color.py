from std_msgs.msg import ColorRGBA


def color(representation: str) -> ColorRGBA:
    if representation == "black":
        return ColorRGBA(r=0, g=0, b=0, a=1.0)
    elif representation == "blue":
        return ColorRGBA(r=0, g=0, b=1, a=1.0)
    elif representation == "green":
        return ColorRGBA(r=0, g=1, b=0, a=1.0)
    elif representation == "cyan":
        return ColorRGBA(r=0, g=1, b=1, a=1.0)
    elif representation == "red":
        return ColorRGBA(r=1, g=0, b=0, a=1.0)
    elif representation == "magenta":
        return ColorRGBA(r=1, g=0, b=1, a=1.0)
    elif representation == "yellow":
        return ColorRGBA(r=1, g=1, b=0, a=1.0)
    elif representation == "white":
        return ColorRGBA(r=1, g=1, b=1, a=1.0)
    else:
        raise RuntimeError(f"Bad Color value '{representation}'!")
