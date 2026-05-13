# robot_bringup

Manual drive stack の launch ファイルをまとめるパッケージ。

## Build

ワークスペースのルートでビルドする。

```bash
cd ~/rodep/fork_lab_serching/ros_ws
colcon build --packages-select robot_bringup joy_driver mecanum_drive ddsm_controller
source install/setup.bash
```

## Launch

ジョイスティック操作から DDSM モータ制御までまとめて起動する。

```bash
ros2 launch robot_bringup manual_drive.launch.py
```

起動されるノード:

- `joy_node`
- `joy_driver_node`
- `mecanum_drive_node`
- `ddsm_controller_node`

## Individual Launch Files

ジョイスティック入力と `/cmd_vel` 変換だけ起動する。

```bash
ros2 launch robot_bringup joy_driver.launch.py
```

DDSM controller だけ起動する。

```bash
ros2 launch robot_bringup ddsm.launch.py
```

## Config

各ノードの設定は、それぞれのパッケージ内の `config/config.yaml` を使う。

- `joy_driver/config/config.yaml`
- `mecanum_drive/config/config.yaml`
- `ddsm_controller/config/config.yaml`

## Topic Flow

```text
joy_node
  -> /joy
joy_driver_node
  -> /cmd_vel
mecanum_drive_node
  -> /wheel_rpm_targets
ddsm_controller_node
  -> DDSM motors
```
