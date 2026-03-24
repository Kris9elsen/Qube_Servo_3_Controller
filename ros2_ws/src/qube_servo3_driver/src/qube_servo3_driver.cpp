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

// std_msgs/Float64MultiArray is what ros2_control's effort_controller publishes.
// std_msgs/Float64 is used as a fallback for direct publishing.
// We subscribe to Float64MultiArray to be compatible with both the sim PID node and direct commands.
#include <std_msgs/msg/float64_multi_array.hpp>

using namespace std::chrono_literals;
using std::placeholders::_1;

namespace qube_servo3_driver
{

QubeServo3Driver::QubeServo3Driver(const rclcpp::NodeOptions & options) 
: Node("qube_servo3_driver", options),
  hardware_initialized_(false),
  hardware_status_(0)
{
    declareParameters();
    
    state_ = QubeState{};
    prev_state_ = QubeState{};
    commanded_effort_ = 0.0;
    prev_time_ = this->now();
    last_diagnostic_time_ = this->now();
    
    if (!initializeHardware()) {
        RCLCPP_ERROR(this->get_logger(), "Failed to initialize Quanser hardware");
    }
    
    setupInterfaces();
    
    control_timer_ = this->create_wall_timer(
        std::chrono::duration<double>(1.0 / update_rate_),
        std::bind(&QubeServo3Driver::controlLoop, this));
    
    if (enable_diagnostics_) {
        diagnostic_timer_ = this->create_wall_timer(
            1s, std::bind(&QubeServo3Driver::publishDiagnostics, this));
    }
    



    RCLCPP_INFO(this->get_logger(), "Qube Servo 3 Driver initialized with Quanser HIL API");
    RCLCPP_INFO(this->get_logger(), "  Board Type: %s", board_type_.c_str());
    RCLCPP_INFO(this->get_logger(), "  Board ID: %s", board_identifier_.c_str());
    RCLCPP_INFO(this->get_logger(), "  Update Rate: %.1f Hz", update_rate_);
    
    param_callback_ = this->add_on_set_parameters_callback(
        std::bind(&QubeServo3Driver::parametersCallback, this, std::placeholders::_1));
}

QubeServo3Driver::~QubeServo3Driver()
{
    running_ = false;
    shutdownHardware();
    RCLCPP_INFO(this->get_logger(), "Qube Servo 3 Driver shutting down");
}

void QubeServo3Driver::declareParameters()
{
    this->declare_parameter<std::string>("board_type", "q8_usb");
    this->declare_parameter<std::string>("board_identifier", "0");
    this->declare_parameter<double>("update_rate", 1000.0);
    this->declare_parameter<std::vector<std::string>>("joint_names", {"motor", "pendulum"});
    this->declare_parameter<double>("encoder_resolution", 4096.0);         // motor encoder counts/rev
    this->declare_parameter<double>("pendulum_encoder_resolution", 2048.0); // pendulum encoder counts/rev
    this->declare_parameter<double>("max_effort", 1.5);
    this->declare_parameter<double>("effort_to_voltage", 1.0);
    this->declare_parameter<double>("filter_alpha", 0.1);
    this->declare_parameter<double>("current_sensor_gain", 0.1);
    this->declare_parameter<bool>("enable_diagnostics", true);
    
    this->get_parameter("board_type", board_type_);
    this->get_parameter("board_identifier", board_identifier_);
    this->get_parameter("update_rate", update_rate_);
    this->get_parameter("joint_names", joint_names_);
    this->get_parameter("encoder_resolution", encoder_resolution_);
    this->get_parameter("pendulum_encoder_resolution", pendulum_encoder_resolution_);
    this->get_parameter("max_effort", max_effort_);
    this->get_parameter("effort_to_voltage", effort_to_voltage_);
    this->get_parameter("filter_alpha", filter_alpha_);
    this->get_parameter("current_sensor_gain", current_sensor_gain_);
    this->get_parameter("enable_diagnostics", enable_diagnostics_);
}

bool QubeServo3Driver::initializeHardware()
{
    t_error result;
    
    encoder_channels_[0] = 0;
    encoder_channels_[1] = 1;
    encoder_vel_channels_[0] = 14000;
    encoder_vel_channels_[1] = 14001;


    dac_channels_[0] = 0;
    adc_channels_[0] = 0;
    pwm_channels_[0] = 0;
    
    result = hil_open(board_type_.c_str(), board_identifier_.c_str(), &card_);
    if (result != 0) {
        handleQuanserError("hil_open", result);
        return false;
    }
    
    // Initialize DAC to 0 before enabling amplifier
    writeVoltage(0.0);

    t_boolean enable_value[1] = {true};
    result = hil_write_digital(card_, digital_out_channels_, 1, enable_value);
    if (result != 0) {
        handleQuanserError("hil_write_digital (enable amplifier)", result);
    } else {
        RCLCPP_INFO(this->get_logger(), "Motor amplifier enabled");
    }
    
    //std::this_thread::sleep_for(std::chrono::milliseconds(10000)); // Short delay to ensure hardware is ready
    

    motor_encoder_offset_ = 0;
    pendulum_encoder_offset_ = 0;


    hardware_initialized_ = true;
    readEncoders();

    motor_encoder_offset_ = state_.encoder_counts[0];
    pendulum_encoder_offset_ = state_.encoder_counts[1];


    
    RCLCPP_INFO(this->get_logger(), "Quanser hardware initialized successfully");
    
    return true;
}

void QubeServo3Driver::shutdownHardware()
{
    if (!hardware_initialized_) return;
    
    writeVoltage(0.0);
    
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
    
    t_int32 counts[2];
    t_error result = hil_read_encoder(card_, encoder_channels_, 2, counts);
    if (result != 0) {
        handleQuanserError("hil_read_encoder", result);
        return false;
    }

    counts[0] -= motor_encoder_offset_;
    counts[1] -= pendulum_encoder_offset_;
    
    std::lock_guard<std::mutex> lock(mutex_);

    state_.encoder_counts[0] = counts[0];
    state_.encoder_counts[1] = counts[1];
    state_.motor_position    = (static_cast<double>(counts[0]) / encoder_resolution_) * 2.0 * M_PI;

    // Use separate resolution for pendulum encoder, and wrap to [-pi, pi]
    double raw_pendulum = (static_cast<double>(counts[1]) / pendulum_encoder_resolution_) * 2.0 * M_PI;
    state_.pendulum_position = std::remainder(raw_pendulum - M_PI, 2.0 * M_PI) + M_PI;
    
    return true;
}

bool QubeServo3Driver::readCurrent()
{
    if (!hardware_initialized_) return false;
    
    t_error result = hil_read_analog(card_, adc_channels_, 1, adc_values_);
    if (result != 0) {
        handleQuanserError("hil_read_analog", result);
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    state_.motor_current = adc_values_[0] / current_sensor_gain_;
    
    return true;
}

void QubeServo3Driver::writeVoltage(double voltage)
{
    if (!hardware_initialized_) return;
    
    voltage = std::clamp(voltage, -15.0, 15.0);
    
    t_double voltages[1] = {voltage};
    t_error result = hil_write_analog(card_, dac_channels_, 1, voltages);
    if (result != 0) {
        handleQuanserError("hil_write_analog", result);
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    state_.motor_voltage = voltage;
}

void QubeServo3Driver::setupInterfaces()
{
    joint_state_pub_ = this->create_publisher<sensor_msgs::msg::JointState>(
        "/joint_states", rclcpp::QoS(10).reliable());
    
    wrench_pub_ = this->create_publisher<geometry_msgs::msg::WrenchStamped>(
        "/wrench", rclcpp::QoS(10).reliable());
    
    if (enable_diagnostics_) {
        diagnostic_pub_ = this->create_publisher<diagnostic_msgs::msg::DiagnosticArray>(
            "/diagnostics", rclcpp::QoS(10).reliable());
    }
    
    effort_sub_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
        "/effort_controller/commands", 
        rclcpp::QoS(10).reliable(),
        std::bind(&QubeServo3Driver::effortCallback, this, _1));
}

void QubeServo3Driver::effortCallback(const std_msgs::msg::Float64MultiArray::SharedPtr msg)
{
    if (msg->data.empty()) return;
    std::lock_guard<std::mutex> lock(mutex_);
    // First element is the motor effort
    commanded_effort_ = std::clamp(msg->data[0], -max_effort_, max_effort_);
}

void QubeServo3Driver::calculateVelocities(double dt)
{
    if (!hardware_initialized_) return;
    
    t_double counts[2];
    t_error result = hil_read_other(card_, encoder_vel_channels_, 2, counts);
    if (result != 0) {
        handleQuanserError("hil_read_other", result);
        return;
    }


    if (dt <= 0.0) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    state_.motor_velocity    = (static_cast<double>(counts[0]) / encoder_resolution_) * 2.0 * M_PI;
    state_.pendulum_velocity = (static_cast<double>(counts[1]) / pendulum_encoder_resolution_) * 2.0 * M_PI;

    //double delta_motor     = state_.motor_position - prev_state_.motor_position;
    //double raw_motor_vel   = delta_motor / dt;
    
    //double delta_pendulum    = unwrapAngle(state_.pendulum_position, prev_state_.pendulum_position);
    //double raw_pendulum_vel  = delta_pendulum / dt;
    
    //state_.motor_velocity    = filter_alpha_ * raw_motor_vel    + (1.0 - filter_alpha_) * prev_state_.motor_velocity;
    //state_.pendulum_velocity = filter_alpha_ * raw_pendulum_vel + (1.0 - filter_alpha_) * prev_state_.pendulum_velocity;
    
    prev_state_ = state_;
}

double QubeServo3Driver::unwrapAngle(double current, double previous)
{
    double delta = current - previous;
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
    hardware_status_ = (card_ != nullptr) ? 0 : -2;
}

void QubeServo3Driver::controlLoop()
{
    if (!hardware_initialized_) {
        RCLCPP_WARN_THROTTLE(this->get_logger(), *this->get_clock(), 5000, 
                             "Hardware not initialized");
        return;
    }

    auto current_time = this->now();
    double dt = (current_time - prev_time_).seconds();

    if (!readEncoders()) {
        return;
    }

    readCurrent();
    calculateVelocities(dt);

    double voltage;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        voltage = -commanded_effort_ * effort_to_voltage_;
    }
    writeVoltage(voltage);

    // Publish joint states
    auto joint_state_msg = sensor_msgs::msg::JointState();
    joint_state_msg.header.stamp = current_time;
    joint_state_msg.name = joint_names_;
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        joint_state_msg.position = {state_.motor_position, state_.pendulum_position};
        joint_state_msg.velocity = {state_.motor_velocity, state_.pendulum_velocity};
        joint_state_msg.effort   = {state_.motor_current, 0.0};
    }
    
    joint_state_pub_->publish(joint_state_msg);
    
    if (wrench_pub_->get_subscription_count() > 0) {
        auto wrench_msg = geometry_msgs::msg::WrenchStamped();
        wrench_msg.header.stamp = current_time;
        wrench_msg.wrench.torque.z = state_.motor_current * 0.1;
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
    status.name        = "Qube Servo 3 Hardware";
    status.hardware_id = board_type_ + "_" + board_identifier_;
    
    if (hardware_initialized_ && hardware_status_ == 0) {
        status.level   = diagnostic_msgs::msg::DiagnosticStatus::OK;
        status.message = "Hardware OK";
    } else if (hardware_initialized_ && hardware_status_ != 0) {
        status.level   = diagnostic_msgs::msg::DiagnosticStatus::WARN;
        status.message = "Hardware warning";
    } else {
        status.level   = diagnostic_msgs::msg::DiagnosticStatus::ERROR;
        status.message = "Hardware not initialized";
    }
    
    auto addKV = [&](const std::string& key, const std::string& value) {
        diagnostic_msgs::msg::KeyValue kv;
        kv.key   = key;
        kv.value = value;
        status.values.push_back(kv);
    };

    addKV("Motor Position (rad)",    std::to_string(state_.motor_position));
    addKV("Pendulum Position (rad)", std::to_string(state_.pendulum_position));
    addKV("Motor Velocity (rad/s)",  std::to_string(state_.motor_velocity));
    addKV("Motor Current (A)",       std::to_string(state_.motor_current));
    addKV("Motor Voltage (V)",       std::to_string(state_.motor_voltage));
    addKV("Encoder Counts (Motor)",  std::to_string(state_.encoder_counts[0]));
    addKV("Encoder Counts (Pendulum)", std::to_string(state_.encoder_counts[1]));
    
    diag_msg.status.push_back(status);
    diagnostic_pub_->publish(diag_msg);
}

void QubeServo3Driver::handleQuanserError(const char* function, int result)
{
    char error_message[256];
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

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(qube_servo3_driver::QubeServo3Driver)