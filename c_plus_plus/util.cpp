#include "util.hpp"
#include <cmath>
#include <stdexcept>

float Util::lerp(float i, float j, float k) {
    return (1 - k) * i + j * k;
}

float Util::invlerp(float i, float j, float k) {
    return (k - i) / (j - i);
}

float Util::xRound(float x) {
    return std::round(x * 4) / 4;
}

int Util::angleToPosition(float degrees) {
    if (degrees < -125.0f || degrees > 125.0f) {
        throw std::invalid_argument("Parameter 'degrees' must be a float value between -125.0 and 125.0");
    }
    float x = xRound(degrees);
    float y = invlerp(-125.0f, 125.0f, x);
    return static_cast<int>(lerp(0, 1000, y));
}

float Util::positionToAngle(int position) {
    if (position < 0 || position > 1000) {
        throw std::invalid_argument("Parameter 'position' must be an int value between 0 and 1000");
    }
    return lerp(-125.0f, 125.0f, position / 1000.0f);
}