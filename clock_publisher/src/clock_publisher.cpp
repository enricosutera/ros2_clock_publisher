#include "rosgraph_msgs/msg/clock.hpp"
#include "rclcpp/rclcpp.hpp"

// and methods of the rclcpp::Node class.
class ClockPublisher : public rclcpp::Node
{
public:
  ClockPublisher() : Node("clock_publisher")
  {
    double clock_frequency = this->declare_parameter<double>("clock_frequency", 10.0);
    real_time_factor = this->declare_parameter<double>("real_time_factor", 10.0);

    publisher_ = this->create_publisher<rosgraph_msgs::msg::Clock>("/clock", 10);

    int timer_nanoseconds = int(((1.0 / clock_frequency) / real_time_factor) * std::pow(10, 9));
    start = this->get_clock()->now();

    timer_ = this->create_wall_timer(std::chrono::nanoseconds(timer_nanoseconds),
                                     std::bind(&ClockPublisher::timer_callback, this));
  }

private:
  double real_time_factor{1.0};
  rclcpp::Time start;
  rclcpp::Publisher<rosgraph_msgs::msg::Clock>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;

  void timer_callback()
  {
    auto clock_msg = rosgraph_msgs::msg::Clock();
    auto current_time = (this->get_clock()->now() - start)*real_time_factor;
    clock_msg.clock.set__sec(std::floor(current_time.seconds()));
    clock_msg.clock.set__nanosec((current_time.seconds()-std::floor(current_time.seconds()))*std::pow(10, 9));
    publisher_->publish(clock_msg);
  }
};

int main(int argc, char *argv[])
{
  rclcpp::init(argc, argv);

  rclcpp::spin(std::make_shared<ClockPublisher>());

  rclcpp::shutdown();
  return 0;
}