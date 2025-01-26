/**
 * Torque control example using voltage control loop.
 *
 * Most of the low-end BLDC driver boards doesn't have current measurement therefore SimpleFOC offers
 * you a way to control motor torque by setting the voltage to the motor instead hte current.
 *
 * This makes the BLDC motor effectively a DC motor, and you can use it in a same way.
 */
#include <Arduino.h>
#include <SimpleFOC.h>
#include "State_Machine.hpp"

// BLDC motor & driver instance
BLDCMotor motor = BLDCMotor(11);
BLDCDriver3PWM driver = BLDCDriver3PWM(BLDC_INA, BLDC_INB, BLDC_INC, BLDC_EN);

MagneticSensorSPI sensor = MagneticSensorSPI(AS5147_SPI, SPI_CS_MAG);

// inline current sensor instance
// ACS712-05B has the resolution of 0.185mV per Amp
InlineCurrentSense current_sense = InlineCurrentSense(185.0f, BLDC_SENSE_A, BLDC_SENSE_B);

// commander communication instance
Commander command = Commander(Serial);
void doMotion(char *cmd) { command.motion(&motor, cmd); }

void setup()
{

    // use monitoring with serial
    Serial.begin(115200);
    // enable more verbose output for debugging
    // comment out if not needed
    SimpleFOCDebug::enable(&Serial);

    sensor.init();
    // link the motor to the sensor
    motor.linkSensor(&sensor);

    // driver config
    // power supply voltage [V]
    driver.voltage_power_supply = 12;
    driver.init();
    // link driver
    motor.linkDriver(&driver);
    // link current sense and the driver
    current_sense.linkDriver(&driver);

    // set control loop type to be used
    motor.controller = MotionControlType::torque;

    // controller configuration based on the control type
    motor.PID_velocity.P = 0.05f;
    motor.PID_velocity.I = 1;
    motor.PID_velocity.D = 0;
    // default voltage_power_supply
    motor.voltage_limit = 12;

    // velocity low pass filtering time constant
    motor.LPF_velocity.Tf = 0.01f;

    // angle loop controller
    motor.P_angle.P = 20;
    // angle loop velocity limit
    motor.velocity_limit = 20;

    // comment out if not needed
    motor.useMonitoring(Serial);
    motor.monitor_downsample = 0;                                  // disable intially
    motor.monitor_variables = _MON_TARGET | _MON_VEL | _MON_ANGLE; // monitor target velocity and angle

    // current sense init and linking
    current_sense.init();
    motor.linkCurrentSense(&current_sense);

    // initialise motor
    motor.init();

    ESP_LOGI("TEST", "Motor Initialised");
    // align encoder and start FOC
    int result = motor.initFOC();

    ESP_LOGI("TEST", "initFOC Result: %d", result);

    // set the inital target value
    motor.target = 12;

    // subscribe motor to the commander
    command.add('T', doMotion, "motion control");
    // command.add('M', doMotor, "motor");

    // Run user commands to configure and the motor (find the full command list in docs.simplefoc.com)
    Serial.println("Motor ready.");

    _delay(1000);
}

void loop()
{
    // iterative setting FOC phase voltage
    motor.loopFOC();

    // iterative function setting the outter loop target
    motor.move();

    // motor monitoring
    motor.monitor();

    // user communication
    command.run();
}