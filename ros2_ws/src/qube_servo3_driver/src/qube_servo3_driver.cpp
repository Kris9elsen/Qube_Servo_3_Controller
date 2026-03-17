#include "qube_servo3_driver/qube_servo3_driver.hpp"

#include <rclcpp_components/register_node_macro.hpp>
#include <tf2/LinearMath/Quaternion.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>

using namespace std::chrono_literals;
using std::placeholders::_1;

namespace qube_servo3_driver
{

QubeServo3Driver::QubeServo3Driver(const rclcpp::NodeOptions & options) 
: Node("qube_servo3_driver", options),
  hardware_initialized_(false),
  hardware_status_(0)
{
    // Declare and get parameters
    declareParameters();
    
    // Initialize state variables
    state_ = QubeState{};
    prev_state_ = QubeState{};
    commanded_effort_ = 0.0;
    prev_time_ = this->now();
    last_diagnostic_time_ = this->now();
    
    // Initialize Quanser hardware
    if (!initializeHardware()) {
        RCLCPP_ERROR(this->get_logger(), "Failed to initialize Quanser hardware");
    }
    
    // Setup ROS interfaces
    setupInterfaces();
    
    // Create control timer
    control_timer_ = this->create_wall_timer(
        std::chrono::duration<double>(1.0 / update_rate_),
        std::bind(&QubeServo3Driver::controlLoop, this));
    
    // Create diagnostic timer
    if (enable_diagnostics_) {
        diagnostic_timer_ = this->create_wall_timer(
            1s, std::bind(&QubeServo3Driver::publishDiagnostics, this));
    }
    
    RCLCPP_INFO(this->get_logger(), "Qube Servo 3 Driver initialized with Quanser HIL API");
    RCLCPP_INFO(this->get_logger(), "  Board Type: %s", board_type_.c_str());
    RCLCPP_INFO(this->get_logger(), "  Board ID: %s", board_identifier_.c_str());
    RCLCPP_INFO(this->get_logger(), "  Update Rate: %.1f Hz", update_rate_);
    
    // Add parameter callback
    param_callback_ = this->add_on_set_parameters_callback(
        std::bind(&QubeServo3Driver::parametersCallback, this, std::placeholders::_1));
}

QubeServo3Driver::~QubeServo3Driver()
{
    running_ = false;
    
    // Shutdown hardware
    shutdownHardware();
    
    RCLCPP_INFO(this->get_logger(), "Qube Servo 3 Driver shutting down");
}

void QubeServo3Driver::declareParameters()
{
    // Declare parameters with default values
    this->declare_parameter<std::string>("board_type", "q8_usb");
    this->declare_parameter<std::string>("board_identifier", "0");
    this->declare_parameter<double>("update_rate", 1000.0);  // 1kHz for real hardware
    this->declare_parameter<std::vector<std::string>>("joint_names", {"motor", "pendulum"});
    this->declare_parameter<double>("encoder_resolution", 4096.0);  // counts per rev
    this->declare_parameter<double>("max_effort", 1.5);  // Nm
    this->declare_parameter<double>("effort_to_voltage", 1.0);  // Nm/V
    this->declare_parameter<double>("filter_alpha", 0.1);  // Low-pass filter coefficient
    this->declare_parameter<double>("current_sensor_gain", 0.1);  // V/A
    this->declare_parameter<bool>("enable_diagnostics", true);
    
    // Get parameters
    this->get_parameter("board_type", board_type_);
    this->get_parameter("board_identifier", board_identifier_);
    this->get_parameter("update_rate", update_rate_);
    this->get_parameter("joint_names", joint_names_);
    this->get_parameter("encoder_resolution", encoder_resolution_);
    this->get_parameter("max_effort", max_effort_);
    this->get_parameter("effort_to_voltage", effort_to_voltage_);
    this->get_parameter("filter_alpha", filter_alpha_);
    this->get_parameter("current_sensor_gain", current_sensor_gain_);
    this->get_parameter("enable_diagnostics", enable_diagnostics_);
}

bool QubeServo3Driver::initializeHardware()
{
    t_error result;
    
    // Set channel configurations for Qube Servo 3
    encoder_channels_[0] = 0;  // Motor encoder
    encoder_channels_[1] = 1;  // Pendulum encoder
    
    dac_channels_[0] = 0;      // DAC 0 for motor voltage
    adc_channels_[0] = 0;      // ADC 0 for current sensor
    pwm_channels_[0] = 0;      // PWM 0 for motor (if using PWM instead of DAC)
    
    // Open the board using HIL API
    result = hil_open(board_type_.c_str(), board_identifier_.c_str(), &card_);
    if (result != 0) {
        handleQuanserError("hil_open", result);
        return false;
    }
    
    // Set encoder quadrature mode for both encoders - FIXED
    t_encoder_quadrature_mode modes[2] = {ENCODER_QUADRATURE_4X, ENCODER_QUADRATURE_4X};
    result = hil_set_encoder_quadrature_mode(card_, encoder_channels_, 2, modes);
    if (result != 0) {
        handleQuanserError("hil_set_encoder_quadrature_mode", result);
    }
    
    // Reset encoders to zero
    
    // Initialize DAC to 0
    writeVoltage(0.0);
    
    hardware_initialized_ = true;
    RCLCPP_INFO(this->get_logger(), "Quanser hardware initialized successfully");
    
    return true;
}

void QubeServo3Driver::shutdownHardware()
{
    if (!hardware_initialized_) return;
    
    // Set motor voltage to 0
    writeVoltage(0.0);
    
    // Close the board using HIL API
    if (card_) {
        hil_close(card_);
        card_ = nullptr;
    }
    
    hardware_initialized_ = false;
    RCLCPP_INFO(this->get_logger(), "Quanser hardware shut down");
}

bool QubeServo3Driver::readEncoders()
{
    if (!hardware_initialized_) return false;
    
    t_error result;
    t_int32 counts[2];  // Changed from t_uint32 to t_int32
    
    // Read both encoders using HIL API
    result = hil_read_encoder(card_, encoder_channels_, 2, counts);
    if (result != 0) {
        handleQuanserError("hil_read_encoder", result);
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Store raw counts
    state_.encoder_counts[0] = counts[0];
    state_.encoder_counts[1] = counts[1];
    
    // Convert counts to radians
    state_.motor_position = (static_cast<double>(counts[0]) / encoder_resolution_) * 2.0 * M_PI;
    state_.pendulum_position = (static_cast<double>(counts[1]) / encoder_resolution_) * 2.0 * M_PI;
    
    return true;
}

bool QubeServo3Driver::readCurrent()
{
    if (!hardware_initialized_) return false;
    
    t_error result;
    
    // Read ADC for current sensor using HIL API
    result = hil_read_analog(card_, adc_channels_, 1, adc_values_);
    if (result != 0) {
        handleQuanserError("hil_read_analog", result);
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    // Convert voltage to current using sensor gain
    state_.motor_current = adc_values_[0] / current_sensor_gain_;
    
    return true;
}

void QubeServo3Driver::writeVoltage(double voltage)
{
    if (!hardware_initialized_) return;
    
    t_error result;
    
    // Clamp voltage to safe limits
    voltage = std::clamp(voltage, -12.0, 12.0);
    
    // Write to DAC using HIL API
    t_double voltages[1] = {voltage};
    result = hil_write_analog(card_, dac_channels_, 1, voltages);
    if (result != 0) {
        handleQuanserError("hil_write_analog", result);
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    state_.motor_voltage = voltage;
}

void QubeServo3Driver::setupInterfaces()
{
    // Publisher for joint states
    joint_state_pub_ = this->create_publisher<sensor_msgs::msg::JointState>(
        "/joint_states", rclcpp::QoS(10).reliable());
    
    // Publisher for wrench data
    wrench_pub_ = this->create_publisher<geometry_msgs::msg::WrenchStamped>(
        "/wrench", rclcpp::QoS(10).reliable());
    
    // Publisher for diagnostics
    if (enable_diagnostics_) {
        diagnostic_pub_ = this->create_publisher<diagnostic_msgs::msg::DiagnosticArray>(
            "/diagnostics", rclcpp::QoS(10).reliable());
    }
    
    // Subscriber for effort commands
    effort_sub_ = this->create_subscription<std_msgs::msg::Float64>(
        "/effort_controller/commands", 
        rclcpp::QoS(10).reliable(),
        std::bind(&QubeServo3Driver::effortCallback, this, _1));
}

void QubeServo3Driver::effortCallback(const std_msgs::msg::Float64::SharedPtr msg)
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Clip effort to safe limits
    commanded_effort_ = std::clamp(
        msg->data,
        -max_effort_,
        max_effort_
    );
}

void QubeServo3Driver::calculateVelocities(double dt)
{
    if (dt <= 0) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Calculate motor velocity
    double delta_motor = state_.motor_position - prev_state_.motor_position;
    double raw_motor_vel = delta_motor / dt;
    
    // Calculate pendulum velocity with unwrapping
    double delta_pendulum = unwrapAngle(state_.pendulum_position, prev_state_.pendulum_position);
    double raw_pendulum_vel = delta_pendulum / dt;
    
    // Apply low-pass filter
    state_.motor_velocity = filter_alpha_ * raw_motor_vel + 
                            (1.0 - filter_alpha_) * prev_state_.motor_velocity;
    state_.pendulum_velocity = filter_alpha_ * raw_pendulum_vel + 
                               (1.0 - filter_alpha_) * prev_state_.pendulum_velocity;
    
    // Update previous state
    prev_state_ = state_;
}

double QubeServo3Driver::unwrapAngle(double current, double previous)
{
    double delta = current - previous;
    
    // Unwrap angle to handle jumps due to 2π wrapping
    if (delta > M_PI) {
        delta -= 2.0 * M_PI;
    } else if (delta < -M_PI) {
        delta += 2.0 * M_PI;
    }
    
    return delta;
}

void QubeServo3Driver::checkHardwareStatus()
{
    if (!hardware_initialized_) {
        hardware_status_ = -1;
        return;
    }
    
    // Simplified - just check if card is valid
    if (card_ != nullptr) {
        hardware_status_ = 0;
    } else {
        hardware_status_ = -2;
    }
}

void QubeServo3Driver::controlLoop()
{
    auto current_time = this->now();
    double dt = (current_time - prev_time_).seconds();
    
    if (!hardware_initialized_) {
        RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 5000, 
                            "Hardware not initialized");
        return;
    }
    
    // Read encoders
    if (!readEncoders()) {
        return;
    }
    
    // Read current sensor
    readCurrent();
    
    // Calculate velocities
    calculateVelocities(dt);
    
    // Send commanded effort to motor
    {
        std::lock_guard<std::mutex> lock(mutex_);
        double voltage = commanded_effort_ * effort_to_voltage_;
        writeVoltage(voltage);
    }
    
    // Create and publish joint state message
    auto joint_state_msg = sensor_msgs::msg::JointState();
    joint_state_msg.header.stamp = current_time;
    joint_state_msg.name = joint_names_;
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        joint_state_msg.position = {state_.motor_position, state_.pendulum_position};
        joint_state_msg.velocity = {state_.motor_velocity, state_.pendulum_velocity};
        joint_state_msg.effort = {state_.motor_current * 0.1, 0.0};  // Approximate torque from current
    }
    
    joint_state_pub_->publish(joint_state_msg);
    
    // Publish wrench data
    if (wrench_pub_->get_subscription_count() > 0) {
        auto wrench_msg = geometry_msgs::msg::WrenchStamped();
        wrench_msg.header.stamp = current_time;
        wrench_msg.wrench.torque.z = state_.motor_current * 0.1;  // Torque from current
        wrench_pub_->publish(wrench_msg);
    }
    
    prev_time_ = current_time;
}

void QubeServo3Driver::publishDiagnostics()
{
    if (!enable_diagnostics_ || !diagnostic_pub_) return;
    
    checkHardwareStatus();
    
    auto diag_msg = diagnostic_msgs::msg::DiagnosticArray();
    diag_msg.header.stamp = this->now();
    
    diagnostic_msgs::msg::DiagnosticStatus status;
    status.name = "Qube Servo 3 Hardware";
    status.hardware_id = board_type_ + "_" + board_identifier_;
    
    if (hardware_initialized_ && hardware_status_ == 0) {
        status.level = diagnostic_msgs::msg::DiagnosticStatus::OK;
        status.message = "Hardware OK";
    } else if (hardware_initialized_ && hardware_status_ != 0) {
        status.level = diagnostic_msgs::msg::DiagnosticStatus::WARN;
        status.message = "Hardware warning";
    } else {
        status.level = diagnostic_msgs::msg::DiagnosticStatus::ERROR;
        status.message = "Hardware not initialized";
    }
    
    // Add key-value pairs
    diagnostic_msgs::msg::KeyValue kv;
    
    kv.key = "Motor Position (rad)";
    kv.value = std::to_string(state_.motor_position);
    status.values.push_back(kv);
    
    kv.key = "Pendulum Position (rad)";
    kv.value = std::to_string(state_.pendulum_position);
    status.values.push_back(kv);
    
    kv.key = "Motor Velocity (rad/s)";
    kv.value = std::to_string(state_.motor_velocity);
    status.values.push_back(kv);
    
    kv.key = "Motor Current (A)";
    kv.value = std::to_string(state_.motor_current);
    status.values.push_back(kv);
    
    kv.key = "Motor Voltage (V)";
    kv.value = std::to_string(state_.motor_voltage);
    status.values.push_back(kv);
    
    kv.key = "Encoder Counts (Motor)";
    kv.value = std::to_string(state_.encoder_counts[0]);
    status.values.push_back(kv);
    
    kv.key = "Encoder Counts (Pendulum)";
    kv.value = std::to_string(state_.encoder_counts[1]);
    status.values.push_back(kv);
    
    diag_msg.status.push_back(status);
    
    diagnostic_pub_->publish(diag_msg);
}

void QubeServo3Driver::handleQuanserError(const char* function, int result)
{
    char error_message[256];
    
    // Correct usage: locale (NULL), error_code, buffer, buffer_size
    msg_get_error_message(NULL, result, error_message, sizeof(error_message));
    
    RCLCPP_ERROR(this->get_logger(), "Quanser error in %s: %s (code: %d)", 
                 function, error_message, result);
}

rcl_interfaces::msg::SetParametersResult 
QubeServo3Driver::parametersCallback(const std::vector<rclcpp::Parameter> &parameters)
{
    rcl_interfaces::msg::SetParametersResult result;
    result.successful = true;
    
    for (const auto &param : parameters) {
        if (param.get_name() == "max_effort") {
            max_effort_ = param.as_double();
            RCLCPP_INFO(this->get_logger(), "Updated max_effort to %f", max_effort_);
        } else if (param.get_name() == "effort_to_voltage") {
            effort_to_voltage_ = param.as_double();
            RCLCPP_INFO(this->get_logger(), "Updated effort_to_voltage to %f", effort_to_voltage_);
        } else if (param.get_name() == "filter_alpha") {
            filter_alpha_ = param.as_double();
            RCLCPP_INFO(this->get_logger(), "Updated filter_alpha to %f", filter_alpha_);
        }
    }
    
    return result;
}

} // namespace qube_servo3_driver

// Register the component with the correct constructor signature
#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(qube_servo3_driver::QubeServo3Driver)