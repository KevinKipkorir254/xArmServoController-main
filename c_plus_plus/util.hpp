#ifndef UTIL_HPP
#define UTIL_HPP

class Util {
public:
    static float lerp(float i, float j, float k);
    static float invlerp(float i, float j, float k);
    static float xRound(float x);
    static int angleToPosition(float degrees);
    static float positionToAngle(int position);
};

#endif // UTIL_HPP