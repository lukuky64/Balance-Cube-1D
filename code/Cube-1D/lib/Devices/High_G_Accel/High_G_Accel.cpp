// // This was just used to test that SPI comms worked with a seperate SD bus simultaneously.
// #include "HIGH_G_ACCEL.hpp"

// #if DUMMY_HIGH_G

// #else

// HIGH_G_ACCEL::HIGH_G_ACCEL() : m_initialised(false)
// {
// }

// /*****************************************************************************/
// /*!
//     @brief  Initialises the sensor with SPI and sets sensor data rate and range

//     @param  SPI_CS The device's chip select / enable pin

//     @param  pSPI_BUS The SPI object to be used for SPI connections.

//     @return True on successful initialisation
// */
// /*****************************************************************************/

// bool HIGH_G_ACCEL::begin(uint8_t SPI_CS, SPICOM &SPI_BUS)
// {
//     m_SPI_BUS = &SPI_BUS;

//     ESP_LOGI("HIGH_G_ACCEL", "Setting up high G");

//     SemaphoreGuard guard(m_SPI_BUS->mutex);
//     if (guard.acquired())
//     {
//         m_accel = new Adafruit_ADXL375(SPI_CS, m_SPI_BUS->BUS, 12345);

//         m_initialised = m_accel->begin();
//     }

//     ESP_LOGI("HIGH_G_ACCEL", "SET UP GOOD = %d", m_initialised);
//     return m_initialised;
// }

// bool HIGH_G_ACCEL::update()
// {
//     bool success = false;

//     SemaphoreGuard guard(m_SPI_BUS->mutex);
//     if (guard.acquired())
//     {
//         success = m_accel->getEvent(&event);
//     }

//     return success;
// }

// float HIGH_G_ACCEL::getAccelX()
// {
//     return event.acceleration.x;
// }

// float HIGH_G_ACCEL::getAccelY()
// {
//     return event.acceleration.y;
// }

// float HIGH_G_ACCEL::getAccelZ()
// {
//     return event.acceleration.z;
// }

// float HIGH_G_ACCEL::getGyroX()
// {
//     return event.acceleration.x;
// }

// float HIGH_G_ACCEL::getGyroY()
// {
//     return event.acceleration.y;
// }

// float HIGH_G_ACCEL::getGyroZ()
// {
//     return event.acceleration.z;
// }

// /**************************************************************************/
// /*!
//     @brief  Gets the timestamp of the most recent sensor event

//     @return timestamp in milliseconds
// */
// /**************************************************************************/
// uint32_t HIGH_G_ACCEL::getTimestampMS()
// {
//     return event.timestamp; // all sensor events have the same timestamp
// }

// bool HIGH_G_ACCEL::checkStatus()
// {
//     return true;
// }

// #endif
