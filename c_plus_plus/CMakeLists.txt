cmake_minimum_required(VERSION 3.10)

project(XArmController)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the hidapi include directory
include_directories(${CMAKE_SOURCE_DIR}/hidapi-win/include)

# Find the HIDAPI library
find_library(HIDAPI_LIBRARY
    NAMES hidapi hidapi-libusb
    PATHS ${CMAKE_SOURCE_DIR}/hidapi-win/x64
    NO_DEFAULT_PATH
)

if(NOT HIDAPI_LIBRARY)
    message(FATAL_ERROR "HIDAPI library not found")
endif()

# Add your source files
add_executable(xarm_controller
    example.cpp
    servo.cpp
    util.cpp
    xarm.cpp
)

# Link against HIDAPI
target_link_libraries(xarm_controller PRIVATE ${HIDAPI_LIBRARY})

# If you're on Windows, you might need these additional libraries
if(WIN32)
    target_link_libraries(xarm_controller PRIVATE setupapi)
endif()