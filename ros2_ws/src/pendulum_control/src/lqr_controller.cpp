#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include <cmath>

class LQR_Controller : public rclcpp::Node
{
public:
  LQR_Controller() : Node("lqr_controller")
  {
    // ── LQR gain vector K = [K_arm_pos, K_pend_pos, K_arm_vel, K_pend_vel] ──
    // Tune these offline via MATLAB/Python lqr() on your linearised model.
    // Default values are reasonable starting points; expect to retune.
    k1_ = this->declare_parameter("k1", -1.0);   // arm position gain
    k2_ = this->declare_parameter("k2", 30.0);   // pendulum angle gain
    k3_ = this->declare_parameter("k3", -1.5);   // arm velocity gain
    k4_ = this->declare_parameter("k4",  5.0);   // pendulum angular velocity gain
    // Integral gain on arm-position error (adds a 5th state to reject DC disturbances)
    k5_ = this->declare_parameter("k5", -0.5);

    // Swing-up parameters (unchanged from PID version)
    mu_              = this->declare_parameter("mu", 50.0);
    swing_threshold_ = this->declare_parameter("swing_threshold", 0.175); // ~10 deg
    command_sign_    = this->declare_parameter("command_sign", 1.0);
    max_command_     = std::abs(this->declare_parameter("max_command", 10.0));

    // Pendulum physical parameters
    mp_ = this->declare_parameter("mp", 0.024);    // pendulum mass [kg]
    Lp_ = this->declare_parameter("Lp", 0.129);    // distance to CoM [m]
    Jp_ = this->declare_parameter("Jp", 1.33e-4);  // moment of inertia [kg·m²]

    // Desired arm position setpoint [rad]
    set_point_ = this->declare_parameter("pos", 0.0);

    arm_integral_ = 0.0;
    last_time_    = this->now();

    joint_state_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
        "/joint_states", 10,
        std::bind(&LQR_Controller::joint_state_callback, this, std::placeholders::_1));

    effort_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
        "/effort_controller/commands", 10);
  }

private:
  // Wrap pendulum angle: 0 = upright, ±π = hanging
  double wrap_pendulum(double raw)
  {
    double a = raw - set_point_;
    a = std::fmod(a + M_PI, 2.0 * M_PI);
    if (a < 0) a += 2.0 * M_PI;
    return a - M_PI;
  }

  double compute_energy(double alpha, double alpha_dot)
  {
    const double g = 9.81;
    // E = 0 at upright equilibrium
    return 0.5 * Jp_ * alpha_dot * alpha_dot
           + mp_ * g * Lp_ * (std::cos(alpha) - 1.0);
  }

  void joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr msg)
  {
    if (msg->position.size() < 2 || msg->velocity.size() < 2) return;

    static bool first_call = true;
    if (first_call)
    {
      last_time_ = this->now();
      first_call = false;
      return;
    }

    const double T = 0.001; // sample period [s]

    double motor_pos   = msg->position[0];
    double pend_raw    = msg->position[1];
    double motor_vel   = msg->velocity[0];
    double alpha_dot   = msg->velocity[1];

    double alpha = wrap_pendulum(pend_raw);

    double voltage = 0.0;

    if (std::abs(alpha) <= swing_threshold_)
    {
      // ── BALANCE MODE: full-state LQR ─────────────────────────────────────
      //
      // State vector x = [arm_error, alpha, arm_vel, alpha_dot, arm_integral]
      //   arm_error = motor_pos - set_point_  (error in arm position)
      //   alpha     = pendulum angle from upright (0 = upright)
      //
      // Control law: u = -K * x
      //   = -(k1*arm_error + k2*alpha + k3*arm_vel + k4*alpha_dot + k5*integral)

      double arm_error = motor_pos - set_point_;

      // Integrate arm position error (trapezoidal, with anti-windup clamp)
      arm_integral_ = std::clamp(
          arm_integral_ + arm_error * T,
          -2.0, 2.0);

      voltage = -(k1_ * arm_error
                + k2_ * alpha
                + k3_ * motor_vel
                + k4_ * alpha_dot
                + k5_ * arm_integral_);
    }
    else
    {
      // ── SWING-UP MODE: energy pumping ────────────────────────────────────
      arm_integral_ = 0.0; // reset integrator

      double E = compute_energy(alpha, alpha_dot);
      // Pump energy toward E_ref = 0 (upright), damp arm motion gently
      voltage = -mu_ * std::copysign(1.0, alpha_dot * std::cos(alpha)) * E;
    }

    voltage *= command_sign_;
    voltage  = std::clamp(voltage, -max_command_, max_command_);

    std_msgs::msg::Float64MultiArray effort_msg;
    effort_msg.data.push_back(voltage);
    effort_pub_->publish(effort_msg);
  }

  // LQR gains
  double k1_, k2_, k3_, k4_, k5_;
  // Swing-up gains
  double mu_, swing_threshold_, command_sign_, max_command_;
  // Physical params
  double mp_, Lp_, Jp_;
  // State
  double set_point_;
  double arm_integral_;
  rclcpp::Time last_time_;
  // ROS interfaces
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_state_sub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr effort_pub_;
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<LQR_Controller>());
  rclcpp::shutdown();
  return 0;
}
