#include "xarm.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <iomanip>

int main() {
    try {
        Controller arm("USB");

        // Turn off all motors
        std::cout << "Turning off all motors..." << std::endl;
        /*
        for (int i = 1; i <= 6; ++i) {
            arm.setMotorPower(i, false);
        }
        */
        std::cout << "All motors turned off." << std::endl;

        const int iterations = 100;
        const std::chrono::milliseconds interval(10); // 10ms for 100Hz

        for (int i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();

            std::vector<int> positions;
            for (int servo = 1; servo <= 6; ++servo) {
                positions.push_back(arm.getPosition(servo));
            }

            std::cout << "Iteration " << std::setw(3) << i + 1 << " - Positions: ";
            for (int pos : positions) {
                std::cout << std::setw(5) << pos << " ";
            }
            std::cout << std::endl;

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

            if (duration < interval) {
                std::this_thread::sleep_for(interval - duration);
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Press Enter to exit...";
    std::cin.get();

    return 0;
}