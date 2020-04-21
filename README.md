# USB-9-DOF-IMU-Viewer
Desktop app to view &amp; log MPU9250 9DOF Inertial Measurement Unit (IMU) data to PC

**Project hardware:**

The hardware used is based on GY-9250 module (MPU9250 9DOF IMU sensor from InvenSense) and MCP2221A USB-to-I2C bridge from Microchip. The MCP2221A communicates with the MPU9250 via I2C interface only.

**Schematic:**

<img width="547" alt="mcp2221_mpu9250 schematic" src="https://user-images.githubusercontent.com/8460504/79907160-02b2df00-83ce-11ea-9564-a98e8b695fb7.png">

**Screenshot:**

<img width="651" alt="MPU9250 IMU viewer_4" src="https://user-images.githubusercontent.com/8460504/79906511-c0d56900-83cc-11ea-8e9a-d2f5c6e4e380.png">

**Source Code:**

The communication between the MCP2221A and MPU9250 sensor is based on the MPU9250 Arduino library project developed by Bolder Flight Systems, this is original repository: https://github.com/bolderflight/MPU9250



