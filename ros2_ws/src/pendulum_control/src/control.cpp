#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

class PID_Controller : public rclcpp::Node {

public:
  PID_Controller() : Node("pid_controller") {
    kp_ = this->declare_parameter("kp", 10.0);
    ki_ = this->declare_parameter("ki", 0.5);
    kd_ = this->declare_parameter("kd", 0.1);

    joint_state_sub_ = this->create_subscription<sensor_msgs::msg::JointState>(
        "/joint_states", 10,
        std::bind(&PID_Controller::joint_state_callback, this, std::placeholders::_1));

    effort_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
        "/effort_controller/commands", 10);

    // Control loop
    timer_ = this->create_wall_timer(
        std::chrono::milliseconds(1), 
        std::bind(&PID_Controller::control_loop, this));

    // Target position
    set_points_ = this->declare_parameter("pos", 3.14);

    integral_ = 0.0;
    prev_error_ = 0.0;
    actual_positions_ = {0.0, 0.0};
    actual_velocities_ = {0.0, 0.0};

    last_time_ = this->now();
  }

private:
  // Updates joint state when /joint_states arrives
  void joint_state_callback(const sensor_msgs::msg::JointState::SharedPtr msg) {
    actual_positions_ = std::vector<double>(msg->position.begin(), msg->position.end());
    actual_velocities_ = std::vector<double>(msg->velocity.begin(), msg->velocity.end());
  }

  // Controll loop calculates PID and publishes effort
  void control_loop() {
    auto now = this->now();
    double dt = (now - last_time_).seconds();
    last_time_ = now;

    if (dt <= 0.0) return;

    if (actual_positions_.size() < 2) return;

    std_msgs::msg::Float64MultiArray effort_msg;
    
    double theta1 = actual_positions_[0];
    double error = (set_points_- 0.01 * theta1) - actual_positions_[1];



    // Propotional
    double p_term = kp_ * error;

    // Integral width anti windup
    integral_ += error * dt;
    integral_ = std::clamp(integral_, -5.0, 5.0);
    double i_term = ki_ * integral_;

    // Derivative
    double derivative = (error - prev_error_) / dt;
    double d_term = kd_ * actual_velocities_[1]; // Using actual velocity for derivative term

    prev_error_ = error;

    effort_msg.data.push_back(p_term + i_term + d_term);

    effort_pub_->publish(effort_msg);
  }

  // PID gains
  double kp_, ki_, kd_;

  // State
  double set_points_;
  std::vector<double> actual_positions_;
  std::vector<double> actual_velocities_;
  double integral_;
  double prev_error_;
  rclcpp::Time last_time_;

  // ROS interface
  rclcpp::Subscription<sensor_msgs::msg::JointState>::SharedPtr joint_state_sub_;
  rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr effort_pub_;
  rclcpp::TimerBase::SharedPtr timer_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<PID_Controller>());
  rclcpp::shutdown();

  return 0;
}
