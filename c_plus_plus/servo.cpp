#include "servo.hpp"
#include <stdexcept>

Servo::Servo(int servo_id, int position) : servo_id(servo_id) {
    setPosition(position);
}

Servo::Servo(int servo_id, float angle) : servo_id(servo_id) {
    setAngle(angle);
}

int Servo::getPosition() const {
    return position;
}

void Servo::setPosition(int pos) {
    if (pos < 0 || pos > 1000) {
        throw std::invalid_argument("Position parameter must be between 0 and 1000");
    }
    position = pos;
    updateAngleFromPosition();
}

float Servo::getAngle() const {
    return angle;
}

void Servo::setAngle(float degrees) {
    if (degrees < -125.0f || degrees > 125.0f) {
        throw std::invalid_argument("Degrees parameter must be between -125.0 and 125.0");
    }
    angle = degrees;
    updatePositionFromAngle();
}

void Servo::updateAngleFromPosition() {
    angle = Util::positionToAngle(position);
}

void Servo::updatePositionFromAngle() {
    position = Util::angleToPosition(angle);
}