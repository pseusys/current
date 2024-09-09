#pragma once

#include <string>

#include "std_msgs/ColorRGBA.h"


enum Color {
    Black,
    Blue,
    Green,
    Cyan,
    Red,
    Magenta,
    Yellow,
    White
};

const std::string getStringFromColor(const Color color);
const Color getColorFromString(const std::string string);
const std_msgs::ColorRGBA getRGBAFromColor(const Color color);
