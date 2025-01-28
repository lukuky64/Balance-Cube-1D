## Ideas:
- [x] Logging can cause delays. Use a circular buffer for data collection and perform batch writes to the SD card to minimise blocking times.
- [ ] Consider fusing IMU and optical/magnetic encoder data using a complementary or Kalman filter to improve angle estimation accuracy.
- [x] Integrate low-power modes for non-critical tasks to maximise energy efficiency.



## Issues:
- [x] When disconnecting from serial, the ESP goes into a non code state (this is normal) but the Red and Green LEDs get left HIGH
- [x] the inturrpt has been setup in IMU.cpp. It should be active HIGH but the library described it strangely



## For future:
- [ ] refreshStatusAll, we need to implement all the function used in this
- [x] Once inturrpt is HIGH, do we need to manually set it back to LOW?
    - No, when it is read, it is reset
- [ ] removed USBPD check from requirements for now
- [ ] need to implement timestamps into kalman fiter, the imu data has this associated with it
- [ ] fusing the encoder with IMU in estimator estimate function
- [x] need to consider the orientation of the imu for estimator correct function
- [x] logging to SD - would be good to define all logging stuff in one file (like header, names, etc) but this isn't as memory efficient
- [ ] need to create a target angle based on centre of mass (manual calibration)
- [x] need to define torque limit in BLDC - getMaxTau()
- [ ] SPI and I2C inherently have their own mutex locks, so i don't have to define them in my code for those buses







# To upload the firmware:

1. Install `esptool.py`:
   > pip install esptool

2. Connect your ESP32 to the computer.

3. Run the following command (replace /dev/ttyUSB0 with your device's port):
   > esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 write_flash 0x1000 firmware.bin
