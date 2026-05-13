# CODEX.md

## Purpose
- This repository contains a ROS 2 Jazzy workspace for a mecanum-drive robot on Ubuntu 24.04.
- The current development order is:
  1. Manual driving with a PS4 controller
  2. Stable mecanum wheel control through DDSM motors
  3. Hokuyo lidar integration
  4. Autonomous navigation on top of the existing `cmd_vel` pipeline

## Workspace Layout
- `ros_ws/` is the ROS 2 workspace root.
- `ros_ws/src/joy_driver` converts joystick input into `geometry_msgs/msg/Twist`.
- `ros_ws/src/mecanum_drive` converts `cmd_vel` into four wheel RPM targets.
- `ros_ws/src/ddsm_controller` sends four wheel targets to the motor bus.
- `archive/` contains experiments and reference scripts. Do not treat it as production code.

## Build And Test
- Build from `ros_ws/`.
- Required base commands:
  - `source /opt/ros/jazzy/setup.bash`
  - `colcon build --symlink-install`
  - `colcon test`
  - `colcon test-result --verbose`

## Session Start Rule
- Start every session by reading `CODEX.md` first and then `WORKING.md`.
- Use `WORKING.md` as the running handoff note for what was done and what to do next.

## Runtime Flow
- Manual driving stack:
  - `joy_node` publishes `/joy`
  - `joy_driver_node` publishes `/cmd_vel`
  - `mecanum_drive_node` publishes `/wheel_rpm_targets`
  - `ddsm_controller_node` consumes `/wheel_rpm_targets`
- Lidar should be integrated later as an additional source of perception and navigation, not as a replacement for the control pipeline.

## Topic And Parameter Conventions
- Use `/cmd_vel` as the common velocity interface.
- Use `/wheel_rpm_targets` for four-wheel target RPM values ordered as:
  - front_left
  - front_right
  - rear_left
  - rear_right
- Keep hardware-specific values in YAML parameters:
  - joystick axis mapping
  - deadzones and scale factors
  - wheel geometry
  - DDSM serial port and motor IDs

## Hardware Notes
- Ubuntu 24.04 + ROS 2 Jazzy is the target environment.
- The drivetrain is assumed to be four independent DDSM motors.
- Hokuyo lidar model and connection type are still a hardware verification item. Prefer an existing ROS 2 driver if supported by the final model.

## Implementation Rules
- Keep the control pipeline modular. Lidar integration must publish or influence `cmd_vel`, not bypass wheel control.
- Prefer standard ROS messages before introducing custom messages.
- Avoid reviving deleted packages unless there is a clear reason. Add new packages only when they provide a cleaner public interface.
