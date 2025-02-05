#pragma once

#define MCU_RX GPIO_NUM_44
#define MCU_TX GPIO_NUM_43
#define I2C_SDA GPIO_NUM_1
#define I2C_SCL GPIO_NUM_2
#define SPI_MOSI GPIO_NUM_11
#define SPI_CLK GPIO_NUM_12
#define SPI_MISO GPIO_NUM_13
#define SPI_MOSI_SD GPIO_NUM_36
#define SPI_CLK_SD GPIO_NUM_35
#define SPI_MISO_SD GPIO_NUM_37
#define SPI_CS_IMU GPIO_NUM_9
#define SPI_CS_MAG GPIO_NUM_10
#define SPI_CS_SD GPIO_NUM_38
#define BLDC_INA GPIO_NUM_15
#define BLDC_INB GPIO_NUM_7
#define BLDC_INC GPIO_NUM_6
#define BLDC_SENSE_A GPIO_NUM_4
#define BLDC_SENSE_B GPIO_NUM_5
#define BLDC_nSLP GPIO_NUM_18
#define BLDC_EN GPIO_NUM_16
#define BLDC_nFLT GPIO_NUM_17 // External pullup. LOW when fault
#define BLDC_nRST GPIO_NUM_8
#define BUZZER GPIO_NUM_42
#define ENC_3V3_A GPIO_NUM_47
#define IMU_INT1 GPIO_NUM_14
#define SERVO GPIO_NUM_21
#define USB_PD_FAULT GPIO_NUM_40

#if DEVBOARD
#define LED_NEO GPIO_NUM_48
#define ENC_3V3_B GPIO_NUM_39
#else
#define LED_NEO GPIO_NUM_39
#define ENC_3V3_B GPIO_NUM_48
#endif
