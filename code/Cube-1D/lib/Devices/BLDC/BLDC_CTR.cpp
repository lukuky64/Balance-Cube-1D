#include "BLDC_CTR.hpp"

BLDC_CTR::BLDC_CTR() : m_torque_constant(0.5) // just to avoid division by zero even though this will be overwritten
{
}

BLDC_CTR::~BLDC_CTR()
{
}

#if DUMMY_BLDC

#else

/*****************************************************************************/
/*!
    @brief  Initialize the BLDC motor controller

    @param phA Phase A pin
    @param phB Phase B pin
    @param phC Phase C pin
    @param enable Enable pin
    @param senseA Current sense A pin
    @param senseB Current sense B pin
    @param MAG_CS Magnetic encoder chip select pin
    @param mag_enc Magnetic encoder object
    @param voltage Voltage of the motor
    @param Kv Motor Kv rating

    @return True on success
*/
/*****************************************************************************/
bool BLDC_CTR::begin(int phA, int phB, int phC, int enable, int senseA, int senseB, int MAG_CS, Mag_Enc *mag_enc, float voltage, float Kv)
{
    m_max_current = DEF_CURRENT_LIM; // 2A current limit by default
    m_Kv = Kv;
    setTorqueConstant(m_Kv);

    m_motor = new BLDCMotor(NUM_POLES, PHASE_RESISTANCE);
    m_driver = new BLDCDriver3PWM(phA, phB, phC, enable);
    m_sensor = mag_enc;
    m_current_sense = new InlineCurrentSense(SENSE_MVPA, senseA, senseB, NOT_SET);

    updateVoltageLimits(voltage);

    m_motor->linkSensor(m_sensor);

    bool driverSucc = ((m_driver->init()) != 0);
    m_motor->linkDriver(m_driver);
    m_current_sense->linkDriver(m_driver);

    bool currentSucc = ((m_current_sense->init()) != 0);
    m_motor->linkCurrentSense(m_current_sense);

    setMotorSettings();

    bool motorSucc = ((m_motor->init()) != 0);
    bool FOCSucc = ((m_motor->initFOC()) != 0);

    enableMotor(false); // Disable motor initially

    ESP_LOGI("BLDC_CTR", "Driver: %d, Motor: %d, FOC: %d, Current: %d", driverSucc, motorSucc, FOCSucc, currentSucc);

    return driverSucc && currentSucc && motorSucc && FOCSucc;
}

/*****************************************************************************/
/*!
    @brief Set the torque constant of the motor based on the Kv rating

    @param Kv Motor Kv rating (rpm/V)
*/
/*****************************************************************************/
void BLDC_CTR::setTorqueConstant(float Kv)
{
    m_torque_constant = 60 / (2 * PI * Kv);
}

bool BLDC_CTR::checkStatus()
{
    return true;
}

/*****************************************************************************/
/*!
    @brief  Enable or disable the motor

    @param bool True to enable, false to disable
*/
/*****************************************************************************/
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

/*****************************************************************************/
/*!
    @brief  The FOC algorithm loop, should be called as fast as possible
*/
/*****************************************************************************/
void BLDC_CTR::loopFOC()
{
    m_motor->loopFOC();
}

/*****************************************************************************/
/*!
    @brief  Create a new torque setpoint

    @param target The target torque
*/
/*****************************************************************************/
void BLDC_CTR::moveTarget(float target)
{
    target = torqueToCurrent(target);
    m_motor->move(target);
}

/***************************************************************************************/
/*!
    @brief Convert a torque setpoint to a current setpoint based on the torque constant

    @param tau The torque setpoint

    @return The current setpoint
*/
/***************************************************************************************/
float BLDC_CTR::torqueToCurrent(float tau)
{
    return tau / m_torque_constant;
}

/***************************************************************************************/
/*!
    @brief Update the voltage limits of the motor controller

    @param voltage The voltage can be supplied
*/
/***************************************************************************************/
void BLDC_CTR::updateVoltageLimits(float voltage)
{
    m_voltage = voltage;
    m_driver->voltage_power_supply = m_voltage;
    m_motor->voltage_limit = m_voltage;

    if (m_voltage >= 12)
    {
        m_motor->voltage_sensor_align = 12; // 12V for alignment. Maybe increase so we get good calibration readings
    }
    else if (m_voltage > 4)
    {
        m_motor->voltage_sensor_align = m_voltage;
    }
    else
    {
        ESP_LOGE("BLDC_CTR", "Voltage too low for sensor alignment.");
    }
}

/***************************************************************************************/
/*!
    @brief Sets up all motor settings for FOC Torque control
*/
/***************************************************************************************/
void BLDC_CTR::setMotorSettings()
{
    // set torque mode:
    m_motor->torque_controller = TorqueControlType::foc_current; // Default is voltage

    // set motion control loop to be used
    m_motor->controller = MotionControlType::torque;

    // Q axis
    m_motor->PID_current_q.P = 3;
    m_motor->PID_current_q.I = 300;
    m_motor->LPF_current_q.Tf = 0.01; // play around with these. Value will depend on the sampling time

    // D axiss
    m_motor->PID_current_d.P = 3;
    m_motor->PID_current_d.I = 300;
    m_motor->LPF_current_d.Tf = 0.01; // play around with these. Value will depend on the sampling time: alpha = Tf/(Tf + Ts)

    // // foc current control parameters
    // m_motor->PID_velocity.P = 0.05f;
    // m_motor->PID_velocity.I = 1;
    // m_motor->PID_velocity.D = 0;

    // velocity low pass filtering time constant
    // m_motor->LPF_velocity.Tf = 0.01f;

    // angle loop controller
    // m_motor->P_angle.P = 20;
    // angle loop velocity limit
    // m_motor->velocity_limit = 20;

    // set the inital target value
    m_motor->target = 0;
}

// !!! Need to implement
float BLDC_CTR::getMaxTau()
{
    return m_torque_constant * m_max_current; // current limit is 2A by default. this is not changed anywehre yet
    // return 0.3f;
}

float BLDC_CTR::getTarget()
{
    return m_motor->target; // does this need to be mutex locked?
}

float BLDC_CTR::getTheta()
{
    return m_motor->shaft_angle; // does this need to be mutex locked?
}

float BLDC_CTR::getOmega()
{
    return m_motor->shaft_velocity; // does this need to be mutex locked?
}

#endif