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
- `front_left_motor_id`
- `front_right_motor_id`
- `rear_left_motor_id`
- `rear_right_motor_id`
- `motor_ids` (旧形式: `[front_left, front_right, rear_left, rear_right]`)
- `max_rpm`
- `enable_feedback`

メカナムの取り付けや配線の都合でモータ ID と車輪位置が一致しない場合は、
`config/config.yaml` の位置名付き ID を変更する。

```yaml
front_left_motor_id: 1
front_right_motor_id: 2
rear_left_motor_id: 3
rear_right_motor_id: 4
```

## Launch

```bash
ros2 launch ddsm_controller launch.py
```
