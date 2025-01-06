#include "BLDC_CTR.hpp"

BLDC_CTR::BLDC_CTR()
{
}

BLDC_CTR::~BLDC_CTR()
{
}

bool BLDC_CTR::begin(int phA, int phB, int phC, int enable, int senseA, int senseB, int MAG_CS, SPICOM &SPI_BUS, float voltage)
{
    m_motor = new BLDCMotor(num_poles, phase_res);
    m_driver = new BLDCDriver3PWM(phA, phB, phC, enable);
    m_sensor = new MagneticSensorSPI(AS5147_SPI, MAG_CS);
    m_current_sense = new InlineCurrentSense(sense_mVpA, senseA, senseB);

    m_SPI_BUS = &SPI_BUS;
    m_voltage = voltage;

    updateVoltageLimits(m_voltage);

    m_sensor->init(m_SPI_BUS->BUS);
    m_motor->linkSensor(m_sensor);

    bool success = ((m_driver->init()) != 0);
    m_motor->linkDriver(m_driver);
    m_current_sense->linkDriver(m_driver);

    success &= ((m_current_sense->init()) != 0);
    m_motor->linkCurrentSense(m_current_sense);

    setMotorSettings();
    success &= ((m_motor->init()) != 0);
    success &= ((m_motor->initFOC()) != 0);

    enableMotor(false); // Disable motor by default

    return success;
}

bool BLDC_CTR::checkStatus()
{
    return true;
}

void BLDC_CTR::enableMotor(bool enable)
{
    if (enable)
    {
        m_motor->enable();
    }
    else
    {
        m_motor->disable();
    }
}

void BLDC_CTR::loop(float target)
{
    m_motor->loopFOC();    // we want this to loop as fast as possible
    m_motor->move(target); // this can loop less frequently
}

void BLDC_CTR::updateVoltageLimits(float voltage)
{
    m_voltage = voltage;
    m_driver->voltage_power_supply = m_voltage;
    m_motor->voltage_limit = m_voltage;
}

void BLDC_CTR::setMotorSettings()
{
    m_motor->controller = MotionControlType::torque;
    // m_motor.torque_controller = TorqueControlType::foc_current; // added this in, might not be needed

    // controller configuration based on the control type
    m_motor->PID_velocity.P = 0.05f;
    m_motor->PID_velocity.I = 1;
    m_motor->PID_velocity.D = 0;

    // velocity low pass filtering time constant
    m_motor->LPF_velocity.Tf = 0.01f;

    // angle loop controller
    m_motor->P_angle.P = 20;
    // angle loop velocity limit
    m_motor->velocity_limit = 20;

    // set the inital target value
    m_motor->target = 0;
}