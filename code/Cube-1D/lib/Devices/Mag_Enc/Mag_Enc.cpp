#include "Mag_Enc.hpp"

/** Typical configuration for the 14bit AMS AS5147 magnetic sensor over SPI interface */
Mag_EncConfig_s AS5047_mag = {
    .spi_mode = SPI_MODE1,
    .clock_speed = 1000000,
    .bit_resolution = 14,
    .angle_register = 0x3FFF,
    .data_start_bit = 13,
    .command_rw_bit = 14,
    .command_parity_bit = 15};

Mag_Enc::Mag_Enc()
{
}

#if DUMMY_MAG

#else

/*****************************************************************************/
/*!
    @brief Initializes the magnetic sensor.

    @param cs - chip select pin

    @param SPI_BUS - SPICOM object containing the SPI bus and mutex

    @param config - configuration structure for specific sensor

    @return true if the magnetic sensor is successfully initialized, false otherwise
*/
/*****************************************************************************/
bool Mag_Enc::init(int cs, SPICOM &SPI_BUS, Mag_EncConfig_s config)
{
  chip_select_pin = cs;
  // angle read register of the magnetic sensor
  angle_register = config.angle_register ? config.angle_register : DEF_ANGLE_REGISTER;
  // register maximum value (counts per revolution)
  cpr = _powtwo(config.bit_resolution);
  spi_mode = config.spi_mode;
  clock_speed = config.clock_speed;
  bit_resolution = config.bit_resolution;

  command_parity_bit = config.command_parity_bit; // for backwards compatibilty
  command_rw_bit = config.command_rw_bit;         // for backwards compatibilty
  data_start_bit = config.data_start_bit;         // for backwards compatibilty

  m_SPI_BUS = &SPI_BUS;
  // 1MHz clock (AMS should be able to accept up to 10MHz)
  settings = SPISettings(clock_speed, MSBFIRST, spi_mode);
  // setup pins
  pinMode(chip_select_pin, OUTPUT);

  {
    SemaphoreGuard guard(m_SPI_BUS->mutex);
    if (guard.acquired())
    {
      m_SPI_BUS->BUS->begin();
      digitalWrite(chip_select_pin, HIGH);
      // SPI has an internal SPI-device counter, it is possible to call "begin()" from different devices

      // do any architectures need to set the clock divider for SPI? Why was this in the code?
      // m_SPI_BUS->BUS->setClfockDivider(SPI_CLOCK_DIV8);
    }
    else
    {
      return false;
    }
  }

  this->Sensor::init(); // call base class init
  return true;
}

/*****************************************************************************/
/*!
    @brief Gets the current angle of the motor shaft.

    @return float - current angle of the motor shaft in radians [rad].
*/
/*****************************************************************************/
float Mag_Enc::getSensorAngle()
{
  return (getRawCount() / (float)cpr) * _2PI;
}

/*****************************************************************************/
/*!
    @brief Gets the raw counter of the magnetic sensor.

    @return int - raw counter of the magnetic sensor.
*/
/*****************************************************************************/
int Mag_Enc::getRawCount()
{
  return (int)Mag_Enc::read(angle_register);
}

/*****************************************************************************/
/*!
    @brief Calculate the even parity of a 16 bit word.

    @param value - 16 bit word to calculate the parity of.

    @return byte - the parity of the 16 bit word.
*/
/*****************************************************************************/
byte Mag_Enc::spiCalcEvenParity(word value)
{
  byte cnt = 0;
  byte i;

  for (i = 0; i < 16; i++)
  {
    if (value & 0x1)
      cnt++;
    value >>= 1;
  }
  return cnt & 0x1;
}

/*****************************************************************************/
/*!
    @brief Read a register from the sensor.

    @param angle_register - address of the register to read (16 bit word).

    @return word - value of the register.
*/
/*****************************************************************************/
word Mag_Enc::read(word angle_register)
{

  // ESP_LOGI("MAG_ENC", "Reading register: %x", angle_register);

  word command = angle_register;

  if (command_rw_bit > 0)
  {
    command = angle_register | (1 << command_rw_bit);
  }
  if (command_parity_bit > 0)
  {
    // Add a parity bit on the the MSB
    command |= ((word)spiCalcEvenParity(command) << command_parity_bit);
  }

  {
    SemaphoreGuard guard(m_SPI_BUS->mutex);
    if (guard.acquired())
    {
      // SPI - begin transaction
      m_SPI_BUS->BUS->beginTransaction(settings);

      // Send the command
      digitalWrite(chip_select_pin, LOW);
      m_SPI_BUS->BUS->transfer16(command);
      digitalWrite(chip_select_pin, HIGH);
    }
  }

#if defined(ESP_H) && defined(ARDUINO_ARCH_ESP32) // if ESP32 board
  delayMicroseconds(50); // why do we need to delay 50us on ESP32? In my experience no extra delays are needed, on any of the architectures I've tested...
#else
  delayMicroseconds(1); // delay 1us, the minimum time possible in plain arduino. 350ns is the required time for AMS sensors, 80ns for MA730, MA702
#endif

  word register_value = 0;

  {
    SemaphoreGuard guard(m_SPI_BUS->mutex);
    if (guard.acquired())
    {
      // Now read the response
      digitalWrite(chip_select_pin, LOW);
      register_value = m_SPI_BUS->BUS->transfer16(0x00);
      digitalWrite(chip_select_pin, HIGH);

      // SPI - end transaction
      m_SPI_BUS->BUS->endTransaction();
    }
  }

  register_value = register_value >> (1 + data_start_bit - bit_resolution); // this should shift data to the rightmost bits of the word

  const static word data_mask = 0xFFFF >> (16 - bit_resolution);

  return register_value & data_mask; // Return the data, stripping the non data (e.g parity) bits
}

/*****************************************************************************/
/*!
    @brief Close the SPI connection. This function must be called exactly
    once for each init() call.
*/
/*****************************************************************************/
void Mag_Enc::close()
{
  SemaphoreGuard guard(m_SPI_BUS->mutex);
  if (guard.acquired())
  {
    m_SPI_BUS->BUS->end();
  }
}

/*****************************************************************************/
/*!
    @brief Check the status of the magnetic sensor.
    @return true if the magnetic sensor is working correctly, false otherwise.
*/
/*****************************************************************************/
bool Mag_Enc::checkStatus()
{
  return true;
}
#endif
