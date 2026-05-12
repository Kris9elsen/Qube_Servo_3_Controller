from launch import LaunchDescription, actions
from launch_ros.actions import Node
from launch.actions import ExecuteProcess, TimerAction, SetEnvironmentVariable
from launch.substitutions import Command
from launch_ros.substitutions import FindPackageShare
import os
import math

def generate_launch_description():
    pkg_path = FindPackageShare("pendulum_control").find("pendulum_control")
    rviz_config_file = os.path.join(pkg_path, "rviz", "config.rviz")
    world_file = os.path.join(pkg_path, "worlds", "world.sdf")
    xacro_file = os.path.join(pkg_path, "urdf", "qube_servo.urdf.xacro")

    # Generate robot_description urdf from xacro
    robot_description = {"robot_description": Command(["xacro ", xacro_file])}

    set_gz_ip = SetEnvironmentVariable("GZ_IP", "127.0.0.1")

    set_gz_plugin_path = SetEnvironmentVariable(
        "GZ_SIM_SYSTEM_PLUGIN_PATH",
        "/opt/ros/jazzy/lib"
    )

    # Resource for qube models in gazebo
    set_gz_resource_path = SetEnvironmentVariable(
        "GZ_SIM_RESOURCE_PATH",
        os.path.join(pkg_path, "..")
    )

    # Robot State Publisher node
    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="screen",
        parameters=[robot_description, {"use_sim_time": True}]
    )

    # Gazebo Server
    gazebo_server = ExecuteProcess(
        cmd=["gz", "sim", "-s", "-r", world_file],
        output="screen"
    )

    # Gazebo GUI 
    gazebo_gui = ExecuteProcess(
        cmd=["gz", "sim", "-g"],
        output="screen"
    )

    # Spawn the robot in Gazebo
    spawn_robot = Node(
        package="ros_gz_sim",
        executable="create",
        arguments=[
            "-name", "qube_servo",
            "-topic", "robot_description",
            "-x", "0.0",
            "-y", "0.0",
            "-z", "0.1",
        ],
        output="screen"
    )

    # Bridge between Gazebo and ROS 2
    ros_gz_bridge = Node(
        package="ros_gz_bridge",
        executable="parameter_bridge",
        arguments=[
            "/clock@rosgraph_msgs/msg/Clock[gz.msgs.Clock",
        ],
        output="screen"
    )

    # Joint state broadcaster controller
    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster", "--controller-manager", "/controller_manager", "--unload-on-kill",],
        output="screen"
    )

    # Effort controller
    effort_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["effort_controller", "--controller-manager", "/controller_manager", "--unload-on-kill",],
        output="screen"
    )

    # RViz node
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=["-d", rviz_config_file]
    )

    # LQR Control Node
    lqr_control_node = Node(
        package="pendulum_control",
        executable="lqr_control",
        name="lqr_controller",
        parameters=[{
            "pos": 3.14,
            "k1":  2.2, 
            "k2":  25.6,
            "k3":  0.7,
            "k4":  2.4,
            "k5": 0.0,
            'mu':  60.0,   # swing-up gain, tune this
            'swing_threshold': 3.14159 / 8.0,  # ~10 deg, switch point
            'mp':  0.024,              # pendulum mass [kg]
            'Lp':  0.129,              # CoM half-length [m]
            'Jp':  1.33e-4,            # moment of inertia [kg·m²]
            'motor_const': 0.04704,      # motor torque constant [N·m/V]
        }],
        arguments=["--ros-args", "-p", "use_sim_time:=true"]
    )

    return LaunchDescription([
        set_gz_ip,
        set_gz_plugin_path,
        set_gz_resource_path,
        robot_state_publisher_node,
        gazebo_server,
        ros_gz_bridge,
        TimerAction(period=2.0, actions=[gazebo_gui]),
        TimerAction(period=4.0, actions=[spawn_robot]),
        TimerAction(period=10.0, actions=[joint_state_broadcaster_spawner]),
        TimerAction(period=12.0, actions=[effort_controller_spawner]),
        TimerAction(period=13.0, actions=[lqr_control_node]),
        rviz_node,
    ])
