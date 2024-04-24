#include <rclcpp/rclcpp.hpp>
#include <example_interfaces/action/fibonacci.hpp>

class FibonacciActionClient : public rclcpp::Node
{
public:
    FibonacciActionClient()
        : Node("fibonacci_action_client")
    {
        action_client_ = rclcpp_action::create_client<example_interfaces::action::Fibonacci>(
            this->get_node_base_interface(),
            this->get_node_graph_interface(),
            this->get_node_logging_interface(),
            this->get_node_waitables_interface(),
            "fibonacci");

        send_goal();
    }

private:
    void send_goal()
    {
        using GoalHandle = rclcpp_action::ClientGoalHandle<example_interfaces::action::Fibonacci>;

        auto goal_msg = example_interfaces::action::Fibonacci::Goal();
        goal_msg.order = 10; // Set the Fibonacci sequence order

        auto send_goal_options = rclcpp_action::Client<example_interfaces::action::Fibonacci>::SendGoalOptions();
       send_goal_options.goal_response_callback =
    [this](const GoalUUID &, std::shared_ptr<const example_interfaces::action::Fibonacci::Goal> goal)
    {
        if (!goal)
        {
            RCLCPP_ERROR(this->get_logger(), "Goal was rejected by server");
        }
        else
        {
            RCLCPP_INFO(this->get_logger(), "Goal accepted by server, waiting for result...");
        }
    };


        action_client_->async_send_goal(goal_msg, send_goal_options);
    }

    rclcpp_action::Client<example_interfaces::action::Fibonacci>::SharedPtr action_client_;
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<FibonacciActionClient>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
