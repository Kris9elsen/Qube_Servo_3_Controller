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
    
    state_                = QubeState{};
    prev_state_           = QubeState{};
    commanded_effort_     = 0.0;
    prev_time_            = this->now();
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
    this->declare_parameter<double>("encoder_resolution", 4096.0);
    this->declare_parameter<double>("pendulum_encoder_resolution", 2048.0);
    this->declare_parameter<double>("max_effort", 1.5);
    this->declare_parameter<double>("current_sensor_gain", 0.1);
    this->declare_parameter<bool>("enable_diagnostics", true);

    // Qube Servo 3 motor electromechanical parameters (Quanser datasheet)
    this->declare_parameter<double>("motor_resistance",   8.4);   // Rm [Ohm]
    this->declare_parameter<double>("motor_torque_const", 0.042); // Kt [Nm/A]
    this->declare_parameter<double>("back_emf_const",     0.042); // Kb [V·s/rad] (= Kt in SI)

    this->get_parameter("board_type", board_type_);
    this->get_parameter("board_identifier", board_identifier_);
    this->get_parameter("update_rate", update_rate_);
    this->get_parameter("joint_names", joint_names_);
    this->get_parameter("encoder_resolution", encoder_resolution_);
    this->get_parameter("pendulum_encoder_resolution", pendulum_encoder_resolution_);
    this->get_parameter("max_effort", max_effort_);
    this->get_parameter("current_sensor_gain", current_sensor_gain_);
    this->get_parameter("enable_diagnostics", enable_diagnostics_);
    this->get_parameter("motor_resistance",   motor_resistance_);
    this->get_parameter("motor_torque_const", motor_torque_const_);
    this->get_parameter("back_emf_const",     back_emf_const_);
}

bool QubeServo3Driver::initializeHardware()
{
    t_error result;
    
    // Channel assignments
    encoder_channels_[0]     = 0;     // motor encoder
    encoder_channels_[1]     = 1;     // pendulum encoder
    encoder_vel_channels_[0] = 14000; // motor velocity (Quanser hardware estimator)
    encoder_vel_channels_[1] = 14001; // pendulum velocity
    dac_channels_[0]         = 0;
    adc_channels_[0]         = 0;

    result = hil_open(board_type_.c_str(), board_identifier_.c_str(), &card_);
    if (result != 0) {
        handleQuanserError("hil_open", result);
        return false;
    }

    // Zero the DAC before enabling the amplifier
    writeVoltage(0.0);

    t_boolean enable_value[1] = {true};
    result = hil_write_digital(card_, digital_out_channels_, 1, enable_value);
    if (result != 0) {
        handleQuanserError("hil_write_digital (enable amplifier)", result);
    } else {
        RCLCPP_INFO(this->get_logger(), "Motor amplifier enabled");
    }

    // Create a HIL reader task that samples all channels atomically at update_rate_.
    // This guarantees encoder, velocity and ADC readings are all from the same instant,
    // which is critical for accurate velocity estimates and back-EMF compensation.
    result = hil_task_create_reader(
        card_,
        1,                      // samples per period (single-buffered)
        adc_channels_,      1,  // analog in  (current sense)
        encoder_channels_,  2,  // encoders   (motor + pendulum)
        NULL,               0,  // digital in (unused)
        encoder_vel_channels_, 2, // other    (hardware velocity estimates)
        &task_);
    if (result != 0) {
        handleQuanserError("hil_task_create_reader", result);
        hil_close(card_);
        return false;
    }

    result = hil_task_start(task_, SYSTEM_CLOCK_1, update_rate_, -1);
    if (result != 0) {
        handleQuanserError("hil_task_start", result);
        hil_task_delete(task_);
        hil_close(card_);
        return false;
    }

    // Capture encoder offsets so all positions start at zero on init
    motor_encoder_offset_    = 0;
    pendulum_encoder_offset_ = 0;
    hardware_initialized_    = true;

    readSensors();
    motor_encoder_offset_    = state_.encoder_counts[0];
    pendulum_encoder_offset_ = state_.encoder_counts[1];

    RCLCPP_INFO(this->get_logger(), "Quanser hardware initialized successfully");
    RCLCPP_INFO(this->get_logger(), "  Encoder offsets: motor=%d, pendulum=%d",
                motor_encoder_offset_, pendulum_encoder_offset_);

    return true;
}

void QubeServo3Driver::shutdownHardware()
{
    if (!hardware_initialized_) return;

    writeVoltage(0.0);

    hil_task_stop(task_);
    hil_task_delete(task_);

    if (card_) {
        hil_close(card_);
        card_ = nullptr;
    }

    hardware_initialized_ = false;
    RCLCPP_INFO(this->get_logger(), "Quanser hardware shut down");
}

// Read all sensors in a single atomic HIL task sample.
// Populates: encoder positions, hardware velocity estimates, motor current.
bool QubeServo3Driver::readSensors()
{
    if (!hardware_initialized_) return false;

    t_double adc[1];
    t_int32  enc[2];
    t_double vel[2];

    t_error result = hil_task_read(
        task_,
        1,      // samples to read
        adc,    // analog  (current sense)
        enc,    // encoders
        NULL,   // digital (unused)
        vel);   // other   (velocity)

    if (result != 0) {
        handleQuanserError("hil_task_read", result);
        return false;
    }

    enc[0] -= motor_encoder_offset_;
    enc[1] -= pendulum_encoder_offset_;

    std::lock_guard<std::mutex> lock(mutex_);

    // Positions
    state_.encoder_counts[0] = enc[0];
    state_.encoder_counts[1] = enc[1];
    state_.motor_position    = (static_cast<double>(enc[0]) / encoder_resolution_) * 2.0 * M_PI;

    double raw_pendulum      = (static_cast<double>(enc[1]) / pendulum_encoder_resolution_) * 2.0 * M_PI;
    state_.pendulum_position = std::remainder(raw_pendulum - M_PI, 2.0 * M_PI) + M_PI;

    // Velocities — directly from Quanser hardware velocity estimator
    state_.motor_velocity    = (vel[0] / encoder_resolution_)          * 2.0 * M_PI;
    state_.pendulum_velocity = (vel[1] / pendulum_encoder_resolution_) * 2.0 * M_PI;

    // Current
    state_.motor_current = adc[0] / current_sensor_gain_;

    return true;
}

// Convert commanded torque [Nm] to motor terminal voltage [V] using the
// DC motor electromechanical equation:
//
//   V = (τ / Kt) * Rm  +  Kb * ω
//
//   τ  = desired torque      [Nm]
//   Kt = torque constant     [Nm/A]
//   Rm = winding resistance  [Ohm]
//   Kb = back-EMF constant   [V·s/rad]  (numerically equal to Kt in SI)
//   ω  = motor velocity      [rad/s]
//
double QubeServo3Driver::torqueToVoltage(double torque_nm) const
{
    const double resistive_drop = (torque_nm / motor_torque_const_) * motor_resistance_;
    const double back_emf       = back_emf_const_ * state_.motor_velocity;
    return resistive_drop + back_emf;
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
    commanded_effort_ = std::clamp(msg->data[0], -max_effort_, max_effort_);
}

double QubeServo3Driver::unwrapAngle(double current, double previous)
{
    double delta = current - previous;
    if (delta > M_PI)       delta -= 2.0 * M_PI;
    else if (delta < -M_PI) delta += 2.0 * M_PI;
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

    if (!readSensors()) {
        return;
    }

    // Convert commanded torque [Nm] → voltage [V] via electromechanical model
    double voltage;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        voltage = torqueToVoltage(-commanded_effort_);
    }
    writeVoltage(voltage);

    // Publish joint states
    auto joint_state_msg = sensor_msgs::msg::JointState();
    joint_state_msg.header.stamp = current_time;
    joint_state_msg.name         = joint_names_;
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        joint_state_msg.position = {state_.motor_position,  state_.pendulum_position};
        joint_state_msg.velocity = {state_.motor_velocity,  state_.pendulum_velocity};
        joint_state_msg.effort   = {state_.motor_current,   0.0};
    }
    
    joint_state_pub_->publish(joint_state_msg);
    
    if (wrench_pub_->get_subscription_count() > 0) {
        auto wrench_msg = geometry_msgs::msg::WrenchStamped();
        wrench_msg.header.stamp    = current_time;
        wrench_msg.wrench.torque.z = state_.motor_current * motor_torque_const_;
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

    addKV("Motor Position (rad)",      std::to_string(state_.motor_position));
    addKV("Pendulum Position (rad)",   std::to_string(state_.pendulum_position));
    addKV("Motor Velocity (rad/s)",    std::to_string(state_.motor_velocity));
    addKV("Pendulum Velocity (rad/s)", std::to_string(state_.pendulum_velocity));
    addKV("Motor Current (A)",         std::to_string(state_.motor_current));
    addKV("Motor Voltage (V)",         std::to_string(state_.motor_voltage));
    addKV("Encoder Counts (Motor)",    std::to_string(state_.encoder_counts[0]));
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
        } else if (param.get_name() == "motor_resistance") {
            motor_resistance_ = param.as_double();
            RCLCPP_INFO(this->get_logger(), "Updated motor_resistance to %f", motor_resistance_);
        } else if (param.get_name() == "motor_torque_const") {
            motor_torque_const_ = param.as_double();
            RCLCPP_INFO(this->get_logger(), "Updated motor_torque_const to %f", motor_torque_const_);
        } else if (param.get_name() == "back_emf_const") {
            back_emf_const_ = param.as_double();
            RCLCPP_INFO(this->get_logger(), "Updated back_emf_const to %f", back_emf_const_);
        }
    }
    
    return result;
}

} // namespace qube_servo3_driver

#include <rclcpp_components/register_node_macro.hpp>
RCLCPP_COMPONENTS_REGISTER_NODE(qube_servo3_driver::QubeServo3Driver)