import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch_ros.actions import Node


def package_config(package_name):
    return os.path.join(
        get_package_share_directory(package_name),
        'config',
        'config.yaml',
    )


def generate_launch_description():
    joy_config = package_config('joy_driver')

    return LaunchDescription(
        [
            Node(
                package='joy',
                executable='joy_node',
                name='joy_node',
                output='screen',
                parameters=[joy_config],
            ),
            Node(
                package='joy_driver',
                executable='joy_driver_node',
                name='joy_driver_node',
                output='screen',
                parameters=[joy_config],
            ),
        ]
    )
