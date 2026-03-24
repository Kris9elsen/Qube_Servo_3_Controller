#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include <cmath>

class QubeServoController : public rclcpp::Node {

public:
  QubeServoController() : Node("qube_servo_controller") {
    // Balance control gains (pendulum stabilization)
    kp_balance_ = this->declare_parameter("kp_balance", 40.0);
    kd_balance_ = this->declare_parameter("kd_balance", 5.0);

    // Swing-up control gains
    kp_swingup_ = this->declare_parameter("kp_swingup", 80.0);

    // Base position control
    kp_base_ = this->declare_parameter("kp_base", 20.0);
    kd_base_ = this->declare_parameter("kd_base", 2.0);

    // Damping
    kd_damp_ = this->declare_parameter("kd_damp", 8.0);

    // Balance threshold (radians from upright)
    balance_threshold_ = this->declare_parameter("balance_threshold", 0.3);

    joint_state_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
        "/joint_states", 10,
        std::bind(&QubeServoController::joint_state_callback, this, std::placeholders::_1));

    effort_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
        "/effort_controller/commands", 10);

    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(1), 
        std::bind(&QubeServoController::control_loop, this));

    base_setpoint_ = this->declare_parameter("base_setpoint", 0.0);

    prev_error_base_ = 0.0;
    actual_positions_ = {0.0, 0.0};
    actual_velocities_ = {0.0, 0.0};
    mode_ = ControlMode::SWING_UP;

    last_time_ = this->now();
  }

private:
  enum ControlMode {
    SWING_UP,
    BALANCE
  };

  void joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {
    actual_positions_ = std::vector<double>(msg->position.begin(), msg->position.end());
    actual_velocities_ = std::vector<double>(msg->velocity.begin(), msg->velocity.end());
  }

  void control_loop() {
    auto now = this->now();
    double dt = (now - last_time_).seconds();
    last_time_ = now;

    if (dt <= 0.0 || dt > 0.1) return;
    if (actual_positions_.size() < 2) return;

    // Extract state
    double theta_base = actual_positions_[0];      // Base angle
    double theta_pend = actual_positions_[1];      // Pendulum angle
    double omega_base = actual_velocities_[0];     // Base angular velocity
    double omega_pend = actual_velocities_[1];     // Pendulum angular velocity

    // Normalize pendulum angle to [-π, π]
    double pend_angle_norm = normalize_angle(theta_pend);

    // Check if pendulum is balanced (near upright: π or -π)
    double angle_from_upright = std::abs(normalize_angle(pend_angle_norm - M_PI));
    bool is_balanced = angle_from_upright < balance_threshold_;

    double effort = 0.0;

    if (is_balanced) {
      mode_ = ControlMode::BALANCE;
      effort = balance_control(theta_base, theta_pend, omega_base, omega_pend, pend_angle_norm, dt);
    } else {
      mode_ = ControlMode::SWING_UP;
      effort = swingup_control(theta_pend, omega_pend, pend_angle_norm);
    }

    // Limit output
    effort = std::clamp(effort, -10.0, 10.0);

    std_msgs::msg::Float64MultiArray effort_msg;
    effort_msg.data.push_back(effort);
    effort_pub_->publish(effort_msg);

    RCLCPP_DEBUG(this->get_logger(),
        "Mode: %s | o_pend: %.3f | w_pend: %.3f | Effort: %.3f",
        (mode_ == ControlMode::BALANCE ? "BALANCE" : "SWING_UP"),
        pend_angle_norm, omega_pend, effort);
  }

  // Swing-up control using energy method (Acrobot strategy)
  double swingup_control(double theta_pend, double omega_pend, double pend_angle_norm) {
    // Energy-based swing-up: accelerate pendulum in direction it's moving
    // Desired energy: E = mgl(1 - cos(θ))
    double desired_energy = 1.0; // Normalize to ~1.0 for upright
    double current_energy = std::cos(pend_angle_norm);

    // Torque to increase energy
    double effort = kp_swingup_ * (desired_energy - current_energy) * std::sin(pend_angle_norm);

    // Damp the swing to smooth it
    effort -= kd_damp_ * omega_pend;

    return effort;
  }

  // Balance control using LQR-inspired gains
  double balance_control(double theta_base, double theta_pend, 
                        double omega_base, double omega_pend, 
                        double pend_angle_norm, double dt) {
    
    // Pendulum error from upright (π)
    double pend_error = normalize_angle(pend_angle_norm - M_PI);

    // Balance: keep pendulum upright
    double balance_effort = kp_balance_ * pend_error + kd_balance_ * omega_pend;

    // Base position control (secondary)
    double base_error = base_setpoint_ - theta_base;
    double p_base = kp_base_ * base_error;
    double d_base = -kd_base_ * omega_base;

    // Base position is less critical, so weight it lower
    double base_effort = 0.2 * (p_base + d_base);

    // Velocity damping for stability
    double damp_effort = -kd_damp_ * omega_base;

    double total_effort = balance_effort + base_effort + damp_effort;

    return total_effort;
  }

  // Normalize angle to [-π, π]
  double normalize_angle(double angle) {
    while (angle > M_PI) angle -= 2.0 * M_PI;
    while (angle < -M_PI) angle += 2.0 * M_PI;
    return angle;
  }

  // Control gains
  double kp_balance_, kd_balance_;  // Pendulum stabilization
  double kp_swingup_;               // Swing-up energy control
  double kp_base_, kd_base_;        // Base position control
  double kd_damp_;                  // Damping
  double balance_threshold_;        // Threshold for switching to balance mode

  // State
  double base_setpoint_;
  std::vector<double> actual_positions_;
  std::vector<double> actual_velocities_;
  double prev_error_base_;
  rclcpp::Time last_time_;
  ControlMode mode_;

  // ROS interface
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_state_sub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr effort_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<QubeServoController>());
  rclcpp::shutdown();

  return 0;
}