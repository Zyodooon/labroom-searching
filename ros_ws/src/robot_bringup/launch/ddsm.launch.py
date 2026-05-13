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
    return LaunchDescription(
        [
            Node(
                package='ddsm_controller',
                executable='ddsm_controller_node',
                name='ddsm_controller_node',
                output='screen',
                parameters=[package_config('ddsm_controller')],
            ),
        ]
    )
