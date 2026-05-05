cmake_minimum_required(VERSION 3.15)
project(QuanserTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(QUANSER_SDK_DIR "C:/Program Files/Quanser/Quanser SDK")

add_executable(quanser_test main.cpp)


target_include_directories(quanser_test PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${QUANSER_SDK_DIR}/include"
)
# Libraries
target_link_directories(quanser_test PRIVATE
    "${QUANSER_SDK_DIR}/lib/win64"
)

target_link_libraries(quanser_test
    hil
    quanser_runtime
    quanser_common
    quanser_devices
    quanser_communications
)