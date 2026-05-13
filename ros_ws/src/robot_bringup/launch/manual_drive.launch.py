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
    mecanum_config = package_config('mecanum_drive')
    ddsm_config = package_config('ddsm_controller')

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
            Node(
                package='mecanum_drive',
                executable='mecanum_drive_node',
                name='mecanum_drive_node',
                output='screen',
                parameters=[mecanum_config],
            ),
            Node(
                package='ddsm_controller',
                executable='ddsm_controller_node',
                name='ddsm_controller_node',
                output='screen',
                parameters=[ddsm_config],
            ),
        ]
    )
