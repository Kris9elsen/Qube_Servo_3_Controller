cmake_minimum_required(VERSION 3.8)
project(qube_servo3_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_components REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(diagnostic_msgs REQUIRED)
find_package(tf2 REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

# Find Quanser SDK
set(QUANSER_ROOT "/opt/quanser" CACHE PATH "Root directory of Quanser SDK")

find_path(QUANSER_INCLUDE_DIR
  NAMES hil.h
  PATHS
    ${QUANSER_ROOT}/include
    ${QUANSER_ROOT}/hil_sdk/include
    /usr/include/quanser
    /usr/local/quanser/include
    /opt/quanser/hil_sdk/include
  REQUIRED
)

find_library(QUANSER_HIL_LIB
  NAMES hil libhil.a
  PATHS
    ${QUANSER_ROOT}/lib
    ${QUANSER_ROOT}/hil_sdk/lib
    /usr/lib/quanser
    /usr/local/quanser/lib
    /opt/quanser/hil_sdk/lib
  REQUIRED
)

find_library(QUANSER_RUNTIME_LIB
  NAMES quanser_runtime libquanser_runtime.a
  PATHS
    ${QUANSER_ROOT}/lib
    ${QUANSER_ROOT}/hil_sdk/lib
    /usr/lib/quanser
    /usr/local/quanser/lib
    /opt/quanser/hil_sdk/lib
  REQUIRED
)

find_library(QUANSER_COMMON_LIB
  NAMES quanser_common libquanser_common.a
  PATHS
    ${QUANSER_ROOT}/lib
    ${QUANSER_ROOT}/hil_sdk/lib
    /usr/lib/quanser
    /usr/local/quanser/lib
    /opt/quanser/hil_sdk/lib
  REQUIRED
)

if(NOT QUANSER_INCLUDE_DIR OR NOT QUANSER_HIL_LIB)
  message(FATAL_ERROR "Quanser SDK not found. Please install Quanser SDK or set QUANSER_ROOT")
endif()

message(STATUS "Quanser SDK found: ${QUANSER_INCLUDE_DIR}")

include_directories(include ${QUANSER_INCLUDE_DIR})

# Create library
add_library(qube_servo3_driver SHARED
  src/qube_servo3_driver.cpp
)

# Link dependencies
target_include_directories(qube_servo3_driver PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${QUANSER_INCLUDE_DIR}
)

ament_target_dependencies(qube_servo3_driver
  rclcpp
  rclcpp_components
  std_msgs
  sensor_msgs
  geometry_msgs
  diagnostic_msgs
  tf2
  tf2_geometry_msgs
)

target_link_libraries(qube_servo3_driver
  ${QUANSER_HIL_LIB}
  ${QUANSER_RUNTIME_LIB}
  ${QUANSER_COMMON_LIB}
  rt
  pthread
  dl
  m
)

# Register component
rclcpp_components_register_node(qube_servo3_driver
  PLUGIN "qube_servo3_driver::QubeServo3Driver"
  EXECUTABLE qube_servo3_driver_node
)

# Install
install(TARGETS
  qube_servo3_driver
  EXPORT export_${PROJECT_NAME}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(DIRECTORY include/
  DESTINATION include
)

install(DIRECTORY launch/
  DESTINATION share/${PROJECT_NAME}/launch
)


# Install config files
install(DIRECTORY config/
  DESTINATION share/${PROJECT_NAME}/config
)

ament_package()