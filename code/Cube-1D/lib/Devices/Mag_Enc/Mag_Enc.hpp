#pragma once

#include "Arduino.h"
#include <SPI.h>
#include "common/base_classes/Sensor.h"
#include "common/foc_utils.h"
#include "common/time_utils.h"
#include "Comms/Comms.hpp"
#include "SemaphoreGuard.hpp"
#include "Params.hpp"

#define DEF_ANGLE_REGISTER 0x3FFF

struct Mag_EncConfig_s
{
  int spi_mode;
  long clock_speed;
  int bit_resolution;
  int angle_register;
  int data_start_bit;
  int command_rw_bit;
  int command_parity_bit;
};
// typical configuration structures
extern Mag_EncConfig_s AS5047_mag;

class Mag_Enc : public Sensor
{
public:
  Mag_Enc();

#if DUMMY_MAG

  bool init(int cs, SPICOM &SPI_BUS, Mag_EncConfig_s config) { return true; }
  float getSensorAngle() { return 0; }
  bool checkStatus() { return true; }

#else

  /** sensor initialise pins */
  bool init(int cs, SPICOM &SPI_BUS, Mag_EncConfig_s config);

  // implementation of abstract functions of the Sensor class
  /** get current angle (rad) */
  float getSensorAngle() override;

  // returns the spi mode (phase/polarity of read/writes) i.e one of SPI_MODE0 | SPI_MODE1 | SPI_MODE2 | SPI_MODE3
  int spi_mode;

  /* returns the speed of the SPI clock signal */
  long clock_speed;

  bool checkStatus();

#endif

private:
  float cpr; //!< Maximum range of the magnetic sensor
  // spi variables
  int angle_register;   //!< SPI angle register to read
  int chip_select_pin;  //!< SPI chip select pin
  SPISettings settings; //!< SPI settings variable
  // spi functions
  /** Stop SPI communication */
  void close();
  /** Read one SPI register value */
  word read(word angle_register);
  /** Calculate parity value  */
  byte spiCalcEvenParity(word value);

  /**
   * Function getting current angle register value
   * it uses angle_register variable
   */
  int getRawCount();

  int bit_resolution;     //!< the number of bites of angle data
  int command_parity_bit; //!< the bit where parity flag is stored in command
  int command_rw_bit;     //!< the bit where read/write flag is stored in command
  int data_start_bit;     //!< the the position of first bit

  SPICOM *m_SPI_BUS = nullptr;
};
