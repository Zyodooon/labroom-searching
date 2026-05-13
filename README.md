# Robot Workspace

Ubuntu 24.04 + ROS 2 Jazzy を前提にした移動ロボット用ワークスペース。  
当面の目標は、PS4 コントローラでメカナム足回りを動かし、その後に北陽 Lidar を追加して自律走行へ拡張することです。

## Current Control Pipeline

手動走行の基本経路は以下です。

```text
/joy -> /cmd_vel -> /wheel_rpm_targets -> DDSM motors
```

- `joy_driver`: PS4 入力を `geometry_msgs/msg/Twist` に変換
- `mecanum_drive`: `cmd_vel` を 4 輪 RPM に変換
- `ddsm_controller`: 4 輪分の RPM 指令を DDSM モータへ送信

## Build

```bash
cd ros_ws
source /opt/ros/jazzy/setup.bash
colcon build --symlink-install
```

## Manual Drive Bringup

```bash
cd ros_ws
source /opt/ros/jazzy/setup.bash
source install/setup.bash
ros2 launch mecanum_drive manual_drive.launch.py
```

必要に応じて YAML で以下を調整します。

- ジョイスティックの軸割り当て
- 速度スケール
- 車輪半径、ホイールベース、トレッド幅
- DDSM のシリアルポートと車輪位置ごとの motor ID

メカナムのモータ ID は `ros_ws/src/ddsm_controller/config/config.yaml` で
`front_left_motor_id`, `front_right_motor_id`, `rear_left_motor_id`,
`rear_right_motor_id` を変更します。

## CI

GitHub Actions は Ubuntu 24.04 上で ROS 2 Jazzy を直接セットアップし、`colcon build` と `colcon test` を実行します。  
以前の Docker Compose 前提の CI は、このリポジトリに定義ファイルが無いため廃止しました。

## Package List Sharing

APT の共有用リストは引き続き `packages.txt` と `Makefile` で管理します。

```bash
sudo apt install pre-commit
pre-commit install
make
```

強制的に記録したい場合:

```bash
make mark
```

他人の手動インストール済みパッケージを反映したい場合:

```bash
make sync
```
