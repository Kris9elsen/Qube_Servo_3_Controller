from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import TimerAction
from launch.substitutions import Command
from launch_ros.substitutions import FindPackageShare
import os


def generate_launch_description():

    pkg_path = FindPackageShare("pendulum_control").find("pendulum_control")

    rviz_config_file = os.path.join(pkg_path, "rviz", "config.rviz")
    xacro_file = os.path.join(pkg_path, "urdf", "qube_servo.urdf.xacro")
    controllers_file = os.path.join(pkg_path, "config", "controllers.yaml")

    # Generate robot_description from xacro
    robot_description = {
        "robot_description": Command(["xacro ", xacro_file])
    }

    # --- Robot State Publisher ---
    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="screen",
        parameters=[robot_description]
    )

    # --- ros2_control node (this runs your hardware driver) ---
    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[
            robot_description,
            controllers_file
        ],
        output="screen"
    )

    # --- Controllers ---
    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster"],
        output="screen"
    )

    effort_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["effort_controller"],
        output="screen"
    )

    # --- RViz ---
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=["-d", rviz_config_file]
    )

    return LaunchDescription([

        robot_state_publisher_node,

        control_node,

        # Wait for controller_manager to start
        TimerAction(period=2.0, actions=[joint_state_broadcaster_spawner]),
        TimerAction(period=3.0, actions=[effort_controller_spawner]),

        rviz_node,
    ])