#ifndef XARM_HPP
#define XARM_HPP

#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <stdexcept>
#include <cstdint>
#include "servo.hpp"
#include "util.hpp"

class Controller {
public:
    static const uint8_t SIGNATURE = 0x55;
    static const uint8_t CMD_SERVO_MOVE = 0x03;
    static const uint8_t CMD_GET_BATTERY_VOLTAGE = 0x0f;
    static const uint8_t CMD_SERVO_STOP = 0x14;
    static const uint8_t CMD_GET_SERVO_POSITION = 0x15;

    Controller(const std::string& com_port, bool debug = false);
    ~Controller();

    void setPosition(int servo, int position, int duration = 1000, bool wait = false);
    void setPosition(const Servo& servo, int duration = 1000, bool wait = false);
    void setPosition(const std::vector<std::pair<int, int>>& servos, int duration = 1000, bool wait = false);
    
    int getPosition(int servo, bool degrees = false);
    int getPosition(const Servo& servo, bool degrees = false);
    void getPosition(std::vector<Servo>& servos, bool degrees = false);

    void servoOff(int servo);
    void servoOff(const Servo& servo);
    void servoOff(const std::vector<int>& servos);
    void servoOff();

    float getBatteryVoltage();

private:
    bool debug;
    bool is_serial;
    void* device;  // This will be either SerialPort* or hid_device* based on the connection type
    std::vector<uint8_t> input_report;
    bool usb_recv_event;

    void send(uint8_t cmd, const std::vector<uint8_t>& data = {});
    std::vector<uint8_t> recv(uint8_t cmd);
    void usbEventHandler(const std::vector<uint8_t>& data, int event_type);
};

#endif // CONTROLLER_HPP