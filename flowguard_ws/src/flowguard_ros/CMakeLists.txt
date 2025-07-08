cmake_minimum_required(VERSION 3.8)
project(flowguard_ros)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

add_executable(flow_publisher src/flow_publisher.cpp)
ament_target_dependencies(flow_publisher rclcpp std_msgs)

add_executable(flow_alarm src/flow_alarm.cpp)
ament_target_dependencies(flow_alarm rclcpp std_msgs)

install(TARGETS
  flow_publisher
  flow_alarm
  DESTINATION lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
