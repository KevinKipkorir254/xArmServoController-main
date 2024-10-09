#ifndef SERVO_HPP
#define SERVO_HPP

#include "util.hpp"

class Servo {
public:
    Servo(int servo_id, int position = 500);
    Servo(int servo_id, float angle);

    int getPosition() const;
    void setPosition(int position);
    float getAngle() const;
    void setAngle(float degrees);

    int servo_id;
    int position;
    float angle;

private:
    void updateAngleFromPosition();
    void updatePositionFromAngle();
};

#endif // SERVO_HPP