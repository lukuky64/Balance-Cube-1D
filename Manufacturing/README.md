# Balance-Cube-1D
This project implements a 6-DOF IMU (with the possible addition of an optical rotary encoder) and active current sensing and magnetic encoding of a BLDC motor to perform closed-loop feedback control to solve an inverted pendulum problem. This device uses a custom PCB, implementing a dual-core 240MHz ESP32-S3 which is taken advantage of through a FreeRTOS implementation for multithreaded operations. This is all powered via a USB-C port with onboard power delivery control for seamless use.

## Stats

![Alt](https://repobeats.axiom.co/api/embed/fa978a6de85922ec26276b8099c5ba40b2c26986.svg "Repobeats analytics image")
