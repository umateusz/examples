// Copyright 2016 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <chrono>
#include <cinttypes>
#include <memory>
#include "example_interfaces/srv/add_two_ints.hpp"
#include "rclcpp/rclcpp.hpp"

using AddTwoInts = example_interfaces::srv::AddTwoInts;
using namespace std::chrono_literals;

class ClientWithTimer : public rclcpp::Node
{
public:
  ClientWithTimer() : Node("client_with_timer")
  {
    client_ = create_client<AddTwoInts>("add_two_ints");
    while (!client_->wait_for_service(std::chrono::seconds(1))) {
      if (!rclcpp::ok()) {
        RCLCPP_ERROR(get_logger(), "client interrupted while waiting for service to appear.");
        rclcpp::shutdown();
        return;
      }
      RCLCPP_INFO(get_logger(), "waiting for service to appear...");
    }
    timer_ = create_wall_timer(500ms, std::bind(&ClientWithTimer::timer_callback, this));
  }

private:
  void timer_callback()
  {
    RCLCPP_INFO(this->get_logger(), "Hello, world!");
    auto request = std::make_shared<AddTwoInts::Request>();
    request->a = 41;
    request->b = 1;
    auto result_future = client_->async_send_request(
      request, [this, request](rclcpp::Client<AddTwoInts>::SharedFuture response) {
        RCLCPP_INFO(this->get_logger(), "Hello from response handler!");
        auto result = response.get();
        RCLCPP_INFO(
          get_logger(), "result of %" PRId64 " + %" PRId64 " = %" PRId64, request->a, request->b,
          result->sum);
      });
  }
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::Client<AddTwoInts>::SharedPtr client_;
};

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<ClientWithTimer>());
  rclcpp::shutdown();
  return 0;
}
