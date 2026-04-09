#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include <cmath>

class PID_Controller : public rclcpp::Node
{
public:
  PID_Controller() : Node("pid_controller")
  {
    // Balance (LQR + PID) gains
    kp_ = this->declare_parameter("kp", 10.0);
    ki_ = this->declare_parameter("ki", 0.5);
    kd_ = this->declare_parameter("kd", 0.1);
    km_ = this->declare_parameter("km", 0.05);
    kmd_ = this->declare_parameter("kmd", 0.01);

    // Swing-up gains
    mu_ = this->declare_parameter("mu", 50.0);                            // energy pumping gain [V/J]
    swing_threshold_ = this->declare_parameter("swing_threshold", 0.175); // ~10 deg in rad

    // Pendulum physical params (adjust to match your hardware)
    mp_ = this->declare_parameter("mp", 0.024);   // pendulum mass [kg]
    Lp_ = this->declare_parameter("Lp", 0.129);   // half-length to CoM [m]
    Jp_ = this->declare_parameter("Jp", 1.33e-4); // moment of inertia [kg·m²]

    set_point_ = this->declare_parameter("pos", 0.0); // arm angle setpoint

    integral_ = 0.0;
    prev_error_ = 0.0;
    last_time_ = this->now();

    joint_state_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
        "/joint_states", 10,
        std::bind(&PID_Controller::joint_state_callback, this, std::placeholders::_1));

    effort_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
        "/effort_controller/commands", 10);
  }

private:
  // Wrap pendulum angle so that 0 = upright (top), ±π = hanging down
  // Assumes encoder gives 0 at bottom, counts up/down from there
  double wrap_pendulum(double raw)
  {

    double a = raw - set_point_;
    // wrap to [-π, π]
    a = std::fmod(a + M_PI, 2.0 * M_PI);
    if (a < 0)
      a += 2.0 * M_PI;
    return a - M_PI;
  }

  double compute_energy(double alpha, double alpha_dot)
  {
    // Total mechanical energy relative to upright equilibrium
    // E = 0 at upright, E = -2*mp*g*Lp at hanging down
    const double g = 9.81;
    return 0.5 * Jp_ * alpha_dot * alpha_dot + mp_ * g * Lp_ * (std::cos(alpha) - 1.0);
    // At upright: cos(0)-1 = 0  → E_ref = 0 ✓
  }

  void joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr msg)
  {
    if (msg->position.size() < 2 || msg->velocity.size() < 2)
      return;

    static bool first_call = true;
    if (first_call)
    {
      last_time_ = this->now();
      first_call = false;
      return;
    }

    const double T = 0.001;

    double motor_pos = msg->position[0];
    double pendulum_raw = msg->position[1];
    double motor_vel = msg->velocity[0];
    double alpha_dot = msg->velocity[1];

    // Wrap so 0 = upright
    double alpha = wrap_pendulum(pendulum_raw);

    double voltage = 0.0;

    if (std::abs(alpha) <= swing_threshold_)
    {
      // PID on pendulum angle (alpha = 0 at upright)
      double error = 0.0 - alpha; // want alpha = 0 (upright)
      double p_term = kp_ * error;
      integral_ = std::clamp(
          integral_ + (error + prev_error_) * T / 2.0, -1.0, 1.0);
      double i_term = ki_ * integral_;
      double d_term = kd_ * -alpha_dot;

      // Motor state feedback (damps arm oscillation)
      double m_term = -km_ * motor_pos;
      double md_term = -kmd_ * motor_vel;

      voltage = p_term + i_term + d_term + m_term + md_term;

      prev_error_ = error;
    }
    else
    {
      // ── SWING-UP MODE: energy pumping ───────────────────────────────────
      integral_ = 0.0; // reset integrator so it doesn't wind up
      prev_error_ = 0.0;

      double m_term = km_ * motor_pos;
      double md_term = kmd_ * motor_vel / 100.0;

      double E = compute_energy(alpha, alpha_dot);
      // E_ref = 0 (upright equilibrium), so error = E - 0 = E
      voltage = -mu_ * std::copysign(1.0, alpha_dot * std::cos(alpha)) * E + m_term + md_term;
    }

    // Hard clamp — protect the motor
    voltage = std::clamp(voltage, -10.0, 10.0);

    std_msgs::msg::Float64MultiArray effort_msg;
    effort_msg.data.push_back(voltage);
    effort_pub_->publish(effort_msg);
  }

  // Balance gains
  double kp_, ki_, kd_, km_, kmd_;
  // Swing-up gains
  double mu_, swing_threshold_;
  // Physical params
  double mp_, Lp_, Jp_;
  // State
  double set_point_;
  double integral_;
  double prev_error_;
  rclcpp::Time last_time_;
  // ROS interfaces
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_state_sub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr effort_pub_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PID_Controller>());
  rclcpp::shutdown();
  return 0;
}