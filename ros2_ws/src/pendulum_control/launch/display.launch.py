from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import Command
from launch_ros.substitutions import FindPackageShare
import os

def generate_launch_description():

    pkg_path = FindPackageShare("pendulum_control").find("pendulum_control")
    rviz_config_file = os.path.join(pkg_path, "rviz", "config.rviz")
    xacro_file = os.path.join(pkg_path, "urdf", "qube_servo.urdf.xacro")

    # Generate robot_description parameter from xacro
    robot_description = {"robot_description": Command(["xacro ", xacro_file])}

    # Robot State Publisher node
    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="screen",
        parameters=[robot_description]
    )

    # Joint state publisher node
    joint_state_publisher_node = Node(
        package="joint_state_publisher_gui",
        executable="joint_state_publisher_gui"
    )
    # RViz node
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=["-d", rviz_config_file]
    )

    return LaunchDescription([
        robot_state_publisher_node,
        joint_state_publisher_node,
        rviz_node
    ])
