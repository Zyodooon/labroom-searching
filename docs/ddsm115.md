# DDSM115 Notes

Source: https://www.waveshare.com/wiki/DDSM115

## Serial

- Interface: RS485
- Baud rate: 115200
- Data bits: 8
- Parity: none
- Stop bits: 1
- Flow control: none
- Format: 115200 8N1

## Protocol

- Frame length: 10 bytes
- CRC: CRC-8/MAXIM over `DATA[0]` through `DATA[8]`
- Reply style: request and response
- Maximum communication rate: 500 Hz

## Velocity Mode

- Command ID: `0x64`
- Velocity range: `-330` to `330` rpm
- Velocity data type: signed 16-bit integer
- Byte order in the command frame: high byte first, then low byte

Velocity command frame:

```text
DATA[0] ID
DATA[1] 0x64
DATA[2] velocity high byte
DATA[3] velocity low byte
DATA[4] 0
DATA[5] 0
DATA[6] acceleration time
DATA[7] brake
DATA[8] 0
DATA[9] CRC8
```

Example frames from the vendor documentation:

```text
01 64 FF CE 00 00 00 00 00 DA  # -50 rpm
01 64 FF 9C 00 00 00 00 00 9A  # -100 rpm
01 64 00 00 00 00 00 00 00 50  # 0 rpm
01 64 00 32 00 00 00 00 00 D3  # 50 rpm
01 64 00 64 00 00 00 00 00 4F  # 100 rpm
```

## Mode Switch

Switch to velocity loop:

```text
01 A0 00 00 00 00 00 00 00 02
```

Mode values:

- `0x01`: current loop
- `0x02`: velocity loop
- `0x03`: position loop

## Feedback

Feedback frame:

```text
DATA[0] ID
DATA[1] mode value
DATA[2] torque current high byte
DATA[3] torque current low byte
DATA[4] velocity high byte
DATA[5] velocity low byte
DATA[6] position high byte
DATA[7] position low byte
DATA[8] error code
DATA[9] CRC8
```
