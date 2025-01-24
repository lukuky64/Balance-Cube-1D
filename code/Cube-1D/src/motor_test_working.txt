#include <Arduino.h>
#include <SimpleFOC.h>
#include "State_Machine.hpp"

// configure the ADC for the pin
bool adcInit(uint8_t pin)
{
    static bool initialized = false;

    int8_t channel = digitalPinToAnalogChannel(pin);
    if (channel < 0)
    {
        ESP_LOGE("ERROR", "Not ADC pin");
        return false; // not adc pin
    }

    analogSetAttenuation(ADC_11db); // this is what causes : Pin is not configured as analog channel. They are also all 11dB by default
    analogReadResolution(12);
    pinMode(pin, ANALOG);
    analogSetPinAttenuation(pin, ADC_11db); // this is what causes : Pin is not configured as analog channel. They are also all 11dB by default
    analogRead(pin);
    return true;
}

// magnetic sensor instance - SPI

// set up spi pins
// SPIClass SPI_1(HSPI);
// SPI_1.begin(14, 12, 13);

MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, 10);
// magnetic sensor instance - I2C
// MagneticSensorI2C sensor = MagneticSensorI2C(AS5600_I2C);
// magnetic sensor instance - analog output
// MagneticSensorAnalog sensor = MagneticSensorAnalog(A1, 14, 1020);

// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(11);
BLDCDriver3PWM driver = BLDCDriver3PWM(15, 7, 6, 16);
// Stepper motor & driver instance
// StepperMotor motor = StepperMotor(50);
// StepperDriver4PWM driver = StepperDriver4PWM(9, 5, 10, 6,  8);

// voltage set point variable
float target_voltage = 4;
// instantiate the commander
Commander command = Commander(Serial);
void doTarget(char *cmd) { command.scalar(&target_voltage, cmd); }

void setup()
{

    adcInit(4);
    adcInit(5);

    // use monitoring with serial
    Serial.begin(115200);
    // enable more verbose output for debugging
    // comment out if not needed
    SimpleFOCDebug::enable(&Serial);

    // initialise magnetic sensor hardware
    sensor.init();
    // link the motor to the sensor
    motor.linkSensor(&sensor);

    // power supply voltage
    driver.voltage_power_supply = 4.9;
    driver.init();
    motor.linkDriver(&driver);

    // aligning voltage
    motor.voltage_sensor_align = 4;
    // choose FOC modulation (optional)
    motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
    // set motion control loop to be used
    motor.controller = MotionControlType::torque;

    // comment out if not needed
    motor.useMonitoring(Serial);

    // initialize motor
    motor.init();
    // align sensor and start FOC
    motor.initFOC();

    // add target command T
    command.add('T', doTarget, "target voltage");

    Serial.println(F("Motor ready."));
    Serial.println(F("Set the target voltage using serial terminal:"));
    _delay(1000);
}

void loop()
{

    int value4 = analogRead(4); // GPIO4
    int value5 = analogRead(5); // GPIO5
    Serial.printf("GPIO4: %d, GPIO5: %d\n", value4, value5);

    // main FOC algorithm function
    // the faster you run this function the better
    // Arduino UNO loop  ~1kHz
    // Bluepill loop ~10kHz
    motor.loopFOC();

    // Motion control function
    // velocity, position or voltage (defined in motor.controller)
    // this function can be run at much lower frequency than loopFOC() function
    // You can also use motor.move() and set the motor.target in the code
    motor.move(target_voltage);

    // user communication
    command.run();
}