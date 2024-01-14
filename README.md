# Clothesline System with Automated Rain Detection - Arduino Project

## Introduction

The Clothesline System with Automated Rain Detection is an Arduino-based project designed to simplify the process of drying clothes outdoors. By incorporating various sensors and a stepper motor, the system can automatically retract the clothesline in response to rain or adverse weather conditions.

## Features

- **Automated Rain Detection:** Equipped with a rain sensor, the system retracts the clothesline when rain or moisture is detected, protecting the clothes from getting wet.
- **Humidity and Temperature Monitoring:** Utilizes a DHT (Digital Humidity and Temperature) sensor to monitor ambient humidity and temperature, helping users determine the optimal time for clothes drying.
- **Light Intensity Sensing:** An LDR (Light Dependent Resistor) measures ambient light intensity. The system considers this data when deciding whether to retract the clothesline, accounting for low light conditions even if rain is detected.
- **Stepper Motor Control:** Employs a stepper motor to control the movement of the clothesline. It rotates to retract the line during adverse conditions and reverses the rotation to extend it when conditions improve.
- **Remote Monitoring with Thingspeak Integration:** Integrated with the Thingspeak platform, allowing users to remotely monitor the system's status. Real-time data, including temperature, humidity, and rain status, can be viewed through the Thingspeak web interface or mobile app.

## Installation

1. **Mount the sensors:** Position the rain sensor, DHT sensor, and LDR at suitable locations for accurate readings.
2. **Connect the Arduino board:** Follow the provided pin definitions in the code to connect the Arduino board to the sensors and the stepper motor.
3. **Upload Arduino code:** Upload the provided Arduino code to enable automated functionality.
4. **Power on the system:** Ensure the system is connected to a Wi-Fi network for Thingspeak integration.

## Usage

1. Hang clothes on the clothesline as usual.
2. The system will autonomously retract the line during unfavorable weather conditions.

## Remote Monitoring

- Access real-time data, including temperature, humidity, and rain status, through the [Thingspeak web interface](https://thingspeak.com/).
- Monitor the system remotely using the Thingspeak mobile app.

## Team Members - ElectroMavericks Team

This project is proudly developed by the ElectroMavericks Team:

- **Prabodha K.P.K.A**
- **Vidmal H.V.P**
- **Nayanthara J.N.P**
- **Surendra S.A.J.E**

### PCB Design

#### Schematic Diagram

See [Appendix 1 – PCB Schematic](path/to/your/appendices/appendix1.png).

#### PCB Layout

See [Appendix 2 – PCB Layout](path/to/your/appendices/appendix2.png) and [Appendix 3 – PCB 3D View](path/to/your/appendices/appendix3.png).

### Enclosure Design

See [Appendix 4 – Enclosure 1](path/to/your/appendices/appendix4.png) and [Appendix 5 – Enclosure 2](path/to/your/appendices/appendix5.png).
