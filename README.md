# MPU9250 9DOF IMU USB Viewer
This is a desktop app to view motion data from MPU9250 9-DOF Inertial Measurement Unit (IMU) sensor. The project is meant to be used by educators and instructors to demostrate motion sensing from acceleration, gyration and earth magnetic field captured by the IMU.

A GPS can be integrated into the system with minimum wiring for navigation and geomatics applications, please visit the "Qt Geolocator"  repository for more information: https://github.com/kelray/Qt-Geolocator

**Worth noting** that the app works with MPU6050 6DOF IMU since it shares with the MPU9250 the same registers set for the accelerometer and gyroscope, and of course the magnetometer data is gibberish since it doesn't have one. The full-scale ranges are different as well, yet the app is capable of collecting acceleration and gyration data. 

**Hardware:**

A GY-9250 IMU module (based on MPU9250 9DOF IMU sensor from InvenSense with other peripherals such as voltage regulator and pull-up resistors) is interfaced to an MCP2221A, a USB-to-UART/I2C bridge from Microchip, via I2C bus.

Both the MCP2221A and the MPU9250 module are powered directly from the USB 5v power. The GY-9250 module has onboard 3.3v voltage regulator.

**Schematic:**

<p align="center"> <img width="547" alt="mcp2221_mpu9250 schematic" src="https://user-images.githubusercontent.com/8460504/79907160-02b2df00-83ce-11ea-9564-a98e8b695fb7.png">

<p align="center"> <img width="450" alt="GY9250" src= "https://hackster.imgix.net/uploads/attachments/221729/9250-1.jpg?auto=compress%2Cformat&w=1280&h=960&fit=max">
 
 Picture from: https://www.hackster.io/30503/using-the-mpu9250-to-get-real-time-motion-data-08f011
 
**Screenshot:**

<p align="center"> <img width="651" alt="MPU9250 IMU viewer_4" src="https://user-images.githubusercontent.com/8460504/79906511-c0d56900-83cc-11ea-8e9a-d2f5c6e4e380.png">

**Libraries & Dependencies:**

The GUI is developed using Qt framework version 4.11.0, source code might require changes if you are using an older version. 

Interfacing the MCP2221A requires library and DLL from Microchip Technologies, the one used here is version 2.1.1 (uploaded among the rest of source code files).

The communication between the MCP2221A and MPU9250 sensor is based on the MPU9250 Arduino library project developed by Bolder Flight Systems, this is original repository: https://github.com/bolderflight/MPU9250

**Source Code:**

The code plots real-time data streaming from MPU9250 sensor to three different charts for acceleration, gyration and magnetic field in the X, Y and Z axes, data is saved to a CSV file when "Save Data" checkbox is checked.

The MPU9250 accelerometer and gyroscope full-scale ranges can be changed on-the-fly from the GUI. This feature doesn't apply to the magnetometer since it has a fixed full-scale range of ±4800 μT.

The MCP2221A has 4 GPIO pins that can be used for digital input, digital output, analog input and/or analog output. In this project I'm using the as digital output only. Each digital output can be set or reset by checking the corresponding checkbox in the GUI. The low- and high-voltage levels are 0v and 5v, respectively.

**For further learning** on how to use the MCP2221 USB-to-UART/I2C bridge please check this tutorial: http://elrayescampaign.blogspot.com/2016/06/mcp2221-tutorial-easy-usb-interfacing.html

**License:**

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The views and conclusions contained in the software and documentation are
 those of the author and should not be interpreted as representing official
 policies, either expressed or implied, by the author.
 
Please let me know if you notice any errors or problems with the source code.

**Disclaimer: Cannot be used for any life-critical application, use it at your own risk.**



