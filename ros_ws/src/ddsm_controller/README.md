# ddsm_controller

4 輪分の RPM 指令を DDSM モータへ送る ROS 2 ノード。

## Topic

- subscribe: `/wheel_rpm_targets`
- publish: `/motor_vel_feedback`
- type: `std_msgs/msg/Float64MultiArray`

配列の順序は以下で固定:

1. front_left
2. front_right
3. rear_left
4. rear_right

## Parameters

- `port_name`
- `baud_rate`
- `motor_ids`
- `max_rpm`
- `enable_feedback`

## Launch

```bash
ros2 launch ddsm_controller launch.py
```
