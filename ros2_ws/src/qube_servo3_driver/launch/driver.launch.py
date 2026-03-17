from launch import LaunchDescription, actions
from launch_ros.actions import Node
from launch.actions import ExecuteProcess, TimerAction, SetEnvironmentVariable, LogInfo
from launch.substitutions import Command, LaunchConfiguration
from launch_ros.substitutions import FindPackageShare
from launch.conditions import IfCondition
import os

def generate_launch_description():
    pkg_path = FindPackageShare("qube_servo3_driver").find("qube_servo3_driver")
    
    # Use the URDF from your simulation - point to your existing URDF
    # or create a new one for real hardware
    xacro_file = os.path.join(pkg_path, "urdf", "qube_servo3_real.urdf.xacro")
    
    # Generate robot_description urdf from xacro
    robot_description = {"robot_description": Command(["xacro ", xacro_file])}

    # Robot State Publisher node (same as simulation)
    robot_state_publisher_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="screen",
        parameters=[robot_description]
    )

    # Joint State Publisher (publishes transforms)
    joint_state_publisher_node = Node(
        package="joint_state_publisher",
        executable="joint_state_publisher",
        name="joint_state_publisher",
        parameters=[{
            'source_list': ['/joint_states']
        }]
    )

    # Qube Servo 3 Driver (replaces Gazebo)
    qube_driver_node = Node(
        package="qube_servo3_driver",
        executable="qube_servo3_driver_node",
        name="qube_servo3_driver",
        output="screen",
        parameters=[{
            'board_type': 'qube_servo3_usb',
            'board_identifier': '0',
            'update_rate': 1000.0,
            'joint_names': ['rotor_joint', 'pendulum_joint'],
            'encoder_resolution': 4096.0,
            'max_effort': 1.5,
            'effort_to_voltage': 1.0,
            'filter_alpha': 0.1,
            'current_sensor_gain': 0.1,
            'enable_diagnostics': True,
        }],
        remappings=[
            ('/effort_controller/commands', '/effort_controller/commands'),
            ('/joint_states', '/joint_states'),
        ]
    )

    # Joint state broadcaster controller (same as simulation)
    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster"],
        output="screen"
    )

    # Effort controller (same as simulation)
    effort_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["effort_controller"],
        output="screen"
    )

    # RViz node (same as simulation)
    rviz_config_file = os.path.join(pkg_path, "rviz", "config.rviz")  # You can copy this from pendulum_control
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="screen",
        arguments=["-d", rviz_config_file],
        condition=IfCondition(LaunchConfiguration('rviz', default='true'))
    )

    # Add a diagnostic node to check hardware status
    diagnostic_node = Node(
        package="diagnostic_aggregator",
        executable="aggregator_node",
        name="diagnostic_aggregator",
        output="screen"
    )

    return LaunchDescription([
        # Log start message
        LogInfo(msg=["Starting Qube Servo 3 Real Hardware Interface..."]),
        
        # Start the driver immediately
        qube_driver_node,
        
        # Start robot state publisher
        robot_state_publisher_node,
        
        # Start joint state publisher
        joint_state_publisher_node,
        
        # Start diagnostic aggregator
        #diagnostic_node,
        
        # Wait for driver to initialize, then spawn controllers (like in simulation)
        TimerAction(period=2.0, actions=[joint_state_broadcaster_spawner]),
        TimerAction(period=3.0, actions=[effort_controller_spawner]),
        
        # Start RViz with a delay
        TimerAction(period=4.0, actions=[rviz_node]),
        
        # Log completion
        TimerAction(period=5.0, actions=[LogInfo(msg=["Qube Servo 3 Real Hardware Interface Started!"])]),
    ])