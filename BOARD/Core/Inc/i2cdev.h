#ifndef __I2CDEV_H
#define __I2CDEV_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"

#define CSdelay22   5

	 


typedef struct
{
  uint8_t TestSignal;      /*!< Specifies the test signal on/off.
                                  This parameter must be set to a value 1 or 0 */

  uint8_t SamplingRate;    /*!< Specifies the sampling rate of data ghathering.
                                  This parameter can be a value of 
																		1 = 250sps
																		2 = 500sps
																		3 = 1000sps
																		4 = 2000sps*/
	
  uint32_t Channels;       /*!< Each bit specifies the channels state.
                                   0 = off, 1 = on. */

  uint8_t Gain;            /*!< Specifies the gain(amplitude) of all brain channels.
                                  This parameter can be a value of 2**21 to zero */

  uint8_t ExtraChannels;  /*!< Specifies the gain(amplitude) of all brain channels.
                                  This parameter can be a value of @ref I2C_dual_addressing_mode */

  uint32_t ExtraGain;      /*!< Specifies the second device own address if dual addressing mode is selected
                                  This parameter can be a 7-bit address. */
																	
	uint8_t LeadoffMode;
	
	uint8_t SRBConnection; 
	
} i2cDevicesTypeDef;




///////////////////////////////////////////////////////////////init

uint8_t check_i2c_slaves(void);
void check_i2c_slaves_IT(void);
uint8_t get_i2c_data(void);
void start_i2c_data(void);


#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_SPI_H */
