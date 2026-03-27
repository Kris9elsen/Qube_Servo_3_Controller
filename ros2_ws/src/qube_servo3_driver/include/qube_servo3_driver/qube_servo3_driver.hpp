#ifndef QUBE_SERVO3_DRIVER_HPP
#define QUBE_SERVO3_DRIVER_HPP

#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joint_state.hpp>
#include <std_msgs/msg/float64.hpp>
#include <std_msgs/msg/float64_multi_array.hpp>
#include <geometry_msgs/msg/wrench_stamped.hpp>
#include <diagnostic_msgs/msg/diagnostic_array.hpp>

// Quanser SDK headers - HIL API
#include "hil.h"
#include "quanser_messages.h"

#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <cmath>
#include <memory>

namespace qube_servo3_driver
{

struct QubeState
{
    double   motor_position;      // rad
    double   motor_velocity;      // rad/s
    double   pendulum_position;   // rad
    double   pendulum_velocity;   // rad/s
    double   motor_current;       // A
    double   motor_voltage;       // V
    uint32_t encoder_counts[2];   // raw counts
};

class QubeServo3Driver : public rclcpp::Node
{
public:
    QubeServo3Driver(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());
    ~QubeServo3Driver();

private:
    // ROS2 interfaces
    void setupInterfaces();
    void effortCallback(const std_msgs::msg::Float64MultiArray::SharedPtr msg);
    void publishJointStates();
    void publishDiagnostics();
    void controlLoop();

    // Quanser hardware communication - HIL API
    bool initializeHardware();
    void shutdownHardware();
    bool readEncoders();
    bool readCurrent();
    bool readSensors();
    void writeVoltage(double voltage);

    // Utility functions
    void   calculateVelocities(double dt);
    double unwrapAngle(double current, double previous);
    void   checkHardwareStatus();
    double torqueToVoltage(double torque) const;
    double voltageToTorque(double voltage);

    // Parameters
    std::string              board_type_;
    std::string              board_identifier_;
    double                   update_rate_;
    std::vector<std::string> joint_names_;
    double                   encoder_resolution_;           // motor encoder counts/rev
    double                   pendulum_encoder_resolution_;  // pendulum encoder counts/rev
    double                   max_effort_;
    double                   effort_to_voltage_;
    double                   filter_alpha_;
    double                   current_sensor_gain_;
    bool                     enable_diagnostics_;
    double                   motor_resistance_;
    double                   motor_torque_const_;
    double                   back_emf_const_;
    t_task                   task_;

    // State variables
    QubeState      state_;
    QubeState      prev_state_;
    double         commanded_effort_;
    rclcpp::Time   prev_time_;
    rclcpp::Time   last_diagnostic_time_;

    // Thread safety
    std::mutex        mutex_;
    std::atomic<bool> running_;

    // Quanser hardware handles - HIL API
    t_card card_;

    // Channel configuration
    t_uint32 encoder_channels_[2];
    t_uint32 encoder_vel_channels_[2];
    t_uint32 dac_channels_[1];
    t_uint32 adc_channels_[1];
    t_uint32 pwm_channels_[1];
    t_uint32 encoder_counts_[2];
    t_double adc_values_[1];
    t_uint32 digital_out_channels_[1];

    // Encoder offset
    t_int32 motor_encoder_offset_;
    t_int32 pendulum_encoder_offset_;

    // Hardware status
    bool hardware_initialized_;
    int  hardware_status_;

    // ROS2 publishers and subscribers
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr        joint_state_pub_;
    rclcpp::Publisher<geometry_msgs::msg::WrenchStamped>::SharedPtr   wrench_pub_;
    rclcpp::Publisher<diagnostic_msgs::msg::DiagnosticArray>::SharedPtr diagnostic_pub_;
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr  effort_sub_;
    rclcpp::TimerBase::SharedPtr                                       control_timer_;
    rclcpp::TimerBase::SharedPtr                                       diagnostic_timer_;

    // Parameter callback
    OnSetParametersCallbackHandle::SharedPtr param_callback_;
    rcl_interfaces::msg::SetParametersResult
    parametersCallback(const std::vector<rclcpp::Parameter> &parameters);

    // Declare parameters
    void declareParameters();

    // Error handling
    void handleQuanserError(const char* function, int result);
};

} // namespace qube_servo3_driver

#endif // QUBE_SERVO3_DRIVER_HPP