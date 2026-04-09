#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

class PID_Controller : public rclcpp::Node {
public:
  PID_Controller() : Node("pid_controller") {
    kp_  = this->declare_parameter("kp",  10.0);
    ki_  = this->declare_parameter("ki",   0.5);
    kd_  = this->declare_parameter("kd",   0.1);
    km_  = this->declare_parameter("km",  0.05);
    kmd_ = this->declare_parameter("kmd", 0.01);
    set_point_ = this->declare_parameter("pos", 3.14);

    integral_   = 0.0;
    prev_error_ = 0.0;
    last_time_  = this->now();

    joint_state_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
        "/joint_states", 10,
        std::bind(&PID_Controller::joint_state_callback, this, std::placeholders::_1));

    effort_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
        "/effort_controller/commands", 10);
  }

private:
  void joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {
    // Sanity checks
    if (msg->position.size() < 2 || msg->velocity.size() < 2) return;

    auto now = this->now();
    double dt = (now - last_time_).seconds();
    last_time_ = now;

    if (dt <= 0.0 || dt > 0.1) return;  // Skip first tick or stale data

    // Unpack state
    double motor_pos     = msg->position[0];
    double pendulum_pos  = msg->position[1];
    double motor_vel     = msg->velocity[0];
    double pendulum_vel  = msg->velocity[1];

    // PID on pendulum position
    double error = set_point_ - pendulum_pos;

    double p_term = kp_ * error;

    integral_ = std::clamp(integral_ + error * dt, -5.0, 5.0);
    double i_term = ki_ * integral_;

    double d_term  = kd_ * -pendulum_vel;

    // Motor state feedback terms
    double m_term  = -km_  * motor_pos;
    double md_term = -kmd_ * motor_vel;

    // Publish effort
    std_msgs::msg::Float64MultiArray effort_msg;
    effort_msg.data.push_back(p_term + i_term + d_term + m_term + md_term);
    effort_pub_->publish(effort_msg);

    prev_error_ = error;
  }

  // Gains
  double kp_, ki_, kd_, km_, kmd_;

  // State
  double set_point_;
  double integral_;
  double prev_error_;
  rclcpp::Time last_time_;

  // ROS interfaces
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_state_sub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr effort_pub_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PID_Controller>());
  rclcpp::shutdown();
  return 0;
}