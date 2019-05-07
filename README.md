# maze-car
This repository contains the example code for the Mazeblazer game. 

The full game instructions and a more detailed getting started guide are available at:
https://docs.google.com/document/d/1SaZNl0VVnFEm_aR2c8GMJGQdpziAJxaRw1_wqC6xl4o/edit?usp=sharing

## Getting Started
1. Download the code from this repository to your computer
- Download git for windows/linux/macOS from https://git-scm.com/downloads
- Use command `git clone https://github.com/VerkkuUkkeli/maze-car` to download a copy of the repository to your machine
2. Download and install the Arduino IDE from https://www.arduino.cc/en/Main/Software
3. Install the following Arduino libraries from Sketch -> Include Library -> Manage Libraries menu in the arduino IDE
- Adafruit Unified Sensor
- Adafruit LSM303DLHC

## Software
- `main.ino` contains the arduino setup and loop code. Write your maze solving application into this file.
- `helpers.ino` contains helper functions for interfacing with the car hardware.

## Hardware
The provided code already takes care of interfacing with the hardware. If you for whatever reason want to write code for interfacing with the hardware directly you are welcome to do so. The components used in the car are listed below.

- Arduino Nano
- Motor driver - [TB6612FNG](https://learn.sparkfun.com/tutorials/tb6612fng-hookup-guide)
- 2 motors
- 5 custom IR distance sensors
- Magnetometer - LSM303DLHC
