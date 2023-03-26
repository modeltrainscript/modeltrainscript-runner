![ alt text for screen readers](/img/logo.png "ModelTrainScript") 

# ModelTrainScript Runner

ModelTrainScript is an app for Apple devices (iPhone, iPad and Mac) that aims to make control and automation easy for your Z21 Controlled layout.
For more information please visit https://modeltrainscript.app

This repository contains the firmware for a ModelTrainScript runner. A runner is an **optional** extension for your ModelTrainScript app that allows you to run the scripts not only on your ESP32 module. The runner gives you the capability to configure the scripts on your phone but afterwards run them independent of your of your iPhone. This is especially useful for layout control scripts like "Set signal to red on turnout 3 set to thrown".

**Please be aware** that, although ModelTrainScript is aiming to make automation easy, the runner requires some technical background to get it working.

# What you need
To main requirement for using this runner is a working ModelTrainScript app configuration. For that you need an compatible Apple device and a layout controlled by any Z21 (or compatible) command station on the same network as the runner.

# How it works
The ESP32 module has a builtin WiFi module. Via WiFi your apple device is able to upload the configured scripts to the module. Also via WiFi the module communicates to the command station. Once configured you will only need to supply power to the module (please be aware most modules require 5V). No need to plug it into your computer anymore, all configuration can be done via the app. Scripts are stored on the runner, so even after a power reset the runner will continue runner your configured scripts.

# Installation Prerequisites
In order to be able you get the runner working you need to arrange a couple of things.

## Hardware
You need to own an ESP32 module, in theory all will do only tested the dev boards. You can buy them for a couple euro/dollar.

## Software
In order to upload the firmware to your module you will need platform.io. You can install the platform ide following the info found at https://platformio.org

# Installation
The installation consist out of a couple of steps
1. Clone this repository to your local machine
3. Open the PlatformIO IDE
2. Enter your WiFi credentials into the src/Config.h file. The hostname can be used to identify your device in case of multiple runners.
4. Connect your ESP32 module and configure the usb port
5. Upload the firmware to your module
6. In the serial monitor (or in your WiFi router) you can find the ip address of the module

After this you go to the ModelTrainScript app and create a new runner. Configure the ip address, validate the connection and you are ready to go. Now you can configure scripts that you can sync to your runner. 

# Questions
In case of any questions feel free to reach out to us via the website https://modeltrainscript.app

# Contribute
ModelTrainScript is free software without any profit in mind. Feel free to reach out in case you would like to contribute to the project.

# License
This software is available under the public domain license. 

![ alt text for screen readers](/img/license.png "license") 

This software contains a modified version of the Z21 library originally written by Philip Gatow https://sourceforge.net/u/gahtow/profile/

# Disclaimer
Please be aware the makers of ModelTrainScript is not responsible for any damage caused by using it.