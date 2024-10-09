#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <chrono>
#include "xarm.hpp"

#ifdef _WIN32
#include <Windows.h>
#else
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#endif

void sleep_ms(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

//#include <hidapi/hidapi.h>
#include "hidapi-win/include/hidapi.h"

Controller::Controller(const std::string& com_port, bool debug) : debug(debug), is_serial(false), device(nullptr), usb_recv_event(false) {
    if (com_port.substr(0, 3) == "COM") {
        // Serial port implementation
        is_serial = true;
        // Implement serial port opening logic here
    } else if (com_port.substr(0, 3) == "USB") {
        // USB HID implementation
        is_serial = false;
        hid_init();
        hid_device* hid_dev = hid_open(0x0483, 0x5750, nullptr);
        if (!hid_dev) {
            throw std::runtime_error("Failed to open USB HID device");
        }
        device = hid_dev;
        hid_set_nonblocking(hid_dev, 1);
        if (debug) {
            wchar_t wstr[64];
            hid_get_serial_number_string(hid_dev, wstr, 64);
            std::wcout << L"Serial number: " << wstr << std::endl;
        }
    } else {
        throw std::invalid_argument("com_port parameter incorrect.");
    }
}

Controller::~Controller() {
    if (is_serial) {
        // Close serial port
    } else {
        hid_close(static_cast<hid_device*>(device));
        hid_exit();
    }
}

void Controller::setPosition(int servo, int position, int duration, bool wait) {
    std::vector<uint8_t> data = {1, static_cast<uint8_t>(duration & 0xff), static_cast<uint8_t>((duration & 0xff00) >> 8)};
    
    if (position < 0 || position > 1000) {
        throw std::invalid_argument("Parameter 'position' must be between 0 and 1000.");
    }
    
    data.push_back(static_cast<uint8_t>(servo));
    data.push_back(static_cast<uint8_t>(position & 0xff));
    data.push_back(static_cast<uint8_t>((position & 0xff00) >> 8));
    
    send(CMD_SERVO_MOVE, data);
    
    if (wait) {
        sleep_ms(duration);
    }
}

void Controller::setPosition(const Servo& servo, int duration, bool wait) {
    setPosition(servo.servo_id, servo.position, duration, wait);
}

void Controller::setPosition(const std::vector<std::pair<int, int>>& servos, int duration, bool wait) {
    std::vector<uint8_t> data = {static_cast<uint8_t>(servos.size()), static_cast<uint8_t>(duration & 0xff), static_cast<uint8_t>((duration & 0xff00) >> 8)};
    
    for (const auto& servo : servos) {
        if (servo.second < 0 || servo.second > 1000) {
            throw std::invalid_argument("Parameter 'position' must be between 0 and 1000.");
        }
        data.push_back(static_cast<uint8_t>(servo.first));
        data.push_back(static_cast<uint8_t>(servo.second & 0xff));
        data.push_back(static_cast<uint8_t>((servo.second & 0xff00) >> 8));
    }
    
    send(CMD_SERVO_MOVE, data);
    
    if (wait) {
        sleep_ms(duration);
    }
}

int Controller::getPosition(int servo, bool degrees) {
    std::vector<uint8_t> data = {1, static_cast<uint8_t>(servo)};
    send(CMD_GET_SERVO_POSITION, data);
    
    auto recv_data = recv(CMD_GET_SERVO_POSITION);
    if (!recv_data.empty()) {
        int position = (recv_data[3] << 8) | recv_data[2];
        return degrees ? Util::positionToAngle(position) : position;
    } else {
        throw std::runtime_error("Function 'getPosition' recv error.");
    }
}

int Controller::getPosition(const Servo& servo, bool degrees) {
    return getPosition(servo.servo_id, degrees);
}

void Controller::getPosition(std::vector<Servo>& servos, bool degrees) {
    std::vector<uint8_t> data = {static_cast<uint8_t>(servos.size())};
    for (const auto& servo : servos) {
        data.push_back(servo.servo_id);
    }
    
    send(CMD_GET_SERVO_POSITION, data);
    
    auto recv_data = recv(CMD_GET_SERVO_POSITION);
    if (!recv_data.empty()) {
        for (size_t i = 0; i < recv_data[0]; ++i) {
            int position = (recv_data[i*3+3] << 8) | recv_data[i*3+2];
            servos[i].position = degrees ? Util::positionToAngle(position) : position;
        }
    } else {
        throw std::runtime_error("Function 'getPosition' recv error.");
    }
}

void Controller::servoOff(int servo) {
    send(CMD_SERVO_STOP, {1, static_cast<uint8_t>(servo)});
}

void Controller::servoOff(const Servo& servo) {
    servoOff(servo.servo_id);
}

void Controller::servoOff(const std::vector<int>& servos) {
    std::vector<uint8_t> data = {static_cast<uint8_t>(servos.size())};
    for (int servo : servos) {
        data.push_back(static_cast<uint8_t>(servo));
    }
    send(CMD_SERVO_STOP, data);
}

void Controller::servoOff() {
    send(CMD_SERVO_STOP, {6, 1, 2, 3, 4, 5, 6});
}

float Controller::getBatteryVoltage() {
    send(CMD_GET_BATTERY_VOLTAGE);
    auto data = recv(CMD_GET_BATTERY_VOLTAGE);
    if (!data.empty()) {
        return ((data[1] << 8) | data[0]) / 1000.0f;
    }
    return 0.0f;
}

void Controller::send(uint8_t cmd, const std::vector<uint8_t>& data) {
    if (debug) {
        std::cout << "Send Data (" << data.size() << "): ";
        for (uint8_t byte : data) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    if (is_serial) {
        // Implement serial send logic
    } else {
        std::vector<uint8_t> report_data = {0, SIGNATURE, SIGNATURE, static_cast<uint8_t>(data.size() + 2), cmd};
        report_data.insert(report_data.end(), data.begin(), data.end());
        usb_recv_event = false;
        hid_write(static_cast<hid_device*>(device), report_data.data(), report_data.size());
    }
}

std::vector<uint8_t> Controller::recv(uint8_t cmd) {
    if (is_serial) {
        // Implement serial receive logic
    } else {
        std::vector<uint8_t> buffer(64);
        int res = hid_read_timeout(static_cast<hid_device*>(device), buffer.data(), buffer.size(), 50);
        if (res > 0 && buffer[0] == SIGNATURE && buffer[1] == SIGNATURE && buffer[3] == cmd) {
            size_t length = buffer[2];
            std::vector<uint8_t> data(buffer.begin() + 4, buffer.begin() + 4 + length);
            if (debug) {
                std::cout << "Recv Data: ";
                for (uint8_t byte : data) {
                    std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
                }
                std::cout << std::dec << std::endl;
            }
            return data;
        }
    }
    return {};
}

void Controller::usbEventHandler(const std::vector<uint8_t>& data, int event_type) {
    input_report = data;
    usb_recv_event = true;
    if (debug) {
        std::cout << "USB Recv Data: ";
        for (uint8_t byte : data) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
        }
        std::cout << std::dec << std::endl;
    }
}