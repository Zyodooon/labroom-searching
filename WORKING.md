# WORKING.md

## Resume Rule
- Start each session by reading `CODEX.md` and this `WORKING.md`.
- Treat this file as the persistent handoff note for terminal-based Codex work.

## Done
- Refactored `joy_driver` to publish `/cmd_vel` from PS4 joystick input.
- Added `mecanum_drive` package to convert `/cmd_vel` into four wheel RPM targets.
- Extended `ddsm_controller` to consume four wheel RPM targets and map them to four motor IDs.
- Added a manual bringup launch for `joy -> cmd_vel -> mecanum -> ddsm`.
- Replaced the broken Docker-based GitHub Actions workflow with native ROS 2 Jazzy CI.
- Added project guidance in `CODEX.md` and updated the root `README.md`.
- Verified the active workspace packages with `colcon build --symlink-install` and `colcon test`.

## Current Control Flow
- `/joy`
- `/cmd_vel`
- `/wheel_rpm_targets`
- `ddsm_controller`

## Next
- Set the real `port_name`, `motor_ids`, wheel radius, wheel base, and tread values from hardware.
- Confirm wheel direction signs on the real mecanum platform.
- Confirm the Hokuyo lidar model number and connection type.
- Integrate the lidar with an existing ROS 2 driver after hardware details are fixed.

## Notes
- Existing deleted files under `ros_ws/src/bringup` were left untouched.
- `__pycache__` directories may exist locally from validation commands and can be ignored or cleaned later.
