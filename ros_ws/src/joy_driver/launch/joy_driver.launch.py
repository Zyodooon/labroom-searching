import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    pkg_share = get_package_share_directory('joy_driver')
    config_file = os.path.join(pkg_share, 'config', 'config.yaml')

    return LaunchDescription([
        Node(
            package='joy',
            executable='joy_node',
            name='joy_node',
            output='screen',
            parameters=[config_file]
        ),
        Node(
            package='joy_driver',
            executable='joy_driver_node',
            name='joy_driver_node',
            output='screen',
            parameters=[config_file]
        ),
    ])
