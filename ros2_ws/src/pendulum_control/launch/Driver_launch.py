from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import TimerAction
from launch.substitutions import Command
from launch_ros.substitutions import FindPackageShare
import os

def generate_launch_description():
    pkg_path    = FindPackageShare("pendulum_control").find("pendulum_control")
    driver_path = FindPackageShare("qube_servo3_driver").find("qube_servo3_driver")

    rviz_config_file = os.path.join(pkg_path, "rviz", "config.rviz")
    xacro_file       = os.path.join(pkg_path, "urdf", "qube_servo.urdf.xacro")

    robot_description = {"robot_description": Command(["xacro ", xacro_file])}

    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="screen",
        parameters=[robot_description]
    )

    joint_state_publisher_node = Node(
        package="joint_state_publisher",
        executable="joint_state_publisher",
        name="joint_state_publisher",
        parameters=[{
            'source_list': ['/joint_states']
        }]
    )

    qube_driver_node = Node(
        package="qube_servo3_driver",
        executable="qube_servo3_driver_node",
        name="qube_servo3_driver",
        output="screen",
        parameters=[{
            'board_type':                  'qube_servo3_usb',
            'board_identifier':            '0',
            'update_rate':                 1000.0,
            'joint_names':                 ['rotor_joint', 'pendulum_joint'],
            'encoder_resolution':          2048.0,
            'pendulum_encoder_resolution': 2048.0,
            'max_effort':                  1.5,
            'effort_to_voltage':           1.0,
            'filter_alpha':                0.1,
            'current_sensor_gain':         0.1,
            'enable_diagnostics':          True,
        }]
    )

    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=["-d", rviz_config_file]
    )

    pid_control_node = Node(
        package="pendulum_control",
        executable="control",
        name="pid_controller",
        parameters=[{
            "pos": 3.14,
            "kp":  60.0,
            "ki":  12.0,
            "kd":  0.5,
        }]
    )

    return LaunchDescription([
        robot_state_publisher_node,
        joint_state_publisher_node,
        qube_driver_node,
        rviz_node,
        TimerAction(period=2.0, actions=[pid_control_node]),
    ])