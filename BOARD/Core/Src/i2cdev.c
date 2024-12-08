/*
 * i2cdev.c
 *
 *  Created on: Mar 21, 2020
 *      Author: i8-tech
 */


/* Includes ------------------------------------------------------------------*/
#include "i2cdev.h"
#include "gpio.h"
#include "i2c.h"

extern uint8_t i2cIN[4], i2cOUT[11];

extern uint8_t user_huge_values, huge_updated, huge_colors[8], huge_stat;
extern uint8_t device_stat, new_data_flag_usb;
extern uint8_t testshow;

void check_i2c_slaves_IT(void)
{	
	if (new_data_flag_usb == 0 && device_stat == 0)
	{
		if (huge_updated == 0)
		{	
			//HAL_I2C_Master_Abort_IT(&hi2c3, 0x10);
			__NOP();	
			huge_updated = 1;
			HAL_I2C_Master_Abort_IT(&hi2c3, 0x10);
		}
		else
		{
		//	HAL_I2C_DisableListen_IT(&hi2c3);
		  huge_updated = 0;
			i2cOUT[0] = 's';
			i2cOUT[1] = huge_colors[0];
			i2cOUT[2] = huge_colors[1];
			i2cOUT[3] = huge_colors[2];
//			HAL_I2C_Master_Transmit_IT(&hi2c3, 0x10, i2cOUT, 4);
			if (HAL_I2C_Master_Transmit_IT(&hi2c3, 0x10, i2cOUT, 4) != HAL_OK)
			{		
				HAL_I2C_Master_Abort_IT(&hi2c3, 0x10);
				HAL_I2C_Master_Transmit_IT(&hi2c3, 0x10, i2cOUT, 4);
			}
		}
	}
}



uint8_t check_i2c_slaves(void)
{	
	if (new_data_flag_usb == 0 && device_stat == 0)
	{
//		if (huge_updated == 0)
//		{
			 	
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_AF);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_ADD10);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_ADDR);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_ARLO);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_BERR);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_BTF);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_BUSY);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_DUALF);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_TXE);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_GENCALL);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_MASK);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_MSL);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_OVR);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_RXNE);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_SB);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_STOPF);
//__HAL_I2C_CLEAR_FLAG(&hi2c3, I2C_FLAG_TRA);

//			MX_I2C3_Init();
			
 // HAL_NVIC_DisableIRQ(I2C3_EV_IRQn);
//	HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
//  HAL_I2C_DeInit(&(&hi2c3->handle));


//   MX_I2C3_Init();
//	 HAL_I2C_MspInit(&hi2c3);
//			
//			huge_updated = 1;
//		}
//		else
		{
		  huge_updated = 0;
			i2cOUT[0] = 's';
			i2cOUT[1] = huge_colors[0];
			i2cOUT[2] = huge_colors[1];
			i2cOUT[3] = huge_colors[2];
	//		HAL_I2C_Master_Transmit_IT(&hi2c3, 0x10, i2cOUT, 4);
			HAL_I2C_Master_Transmit(&hi2c3, 0x10, i2cOUT,4,1);
			HAL_I2C_Master_Receive(&hi2c3, 0x10, i2cIN,4,10);
		}
	}
		
//	if (HAL_I2C_Master_Transmit(&hi2c3, 0x10, i2cOUT, 4, 1) == HAL_OK)
//	 {
//		// HAL_I2C_Master_Receive(&hi2c3, 0x10, i2cIN, 4, 100);
//		 HAL_I2C_Master_Receive_IT(&hi2c3, 0x10, i2cIN, 4);
//		 return i2cIN[3];	
//	 }
//	 else
//	 {
//		return 0;
//	 }
// }
		return 255;
}

void start_i2c_data(void)
{
	while (huge_updated == 0)
	{
		__NOP();
		SetLED(1,1,1,1);
	}
	
	i2cOUT[0] = 'd';
	i2cOUT[1] = 0;
	i2cOUT[2] = 1;
	i2cOUT[3] = 1;
	
	HAL_I2C_Master_Transmit_IT(&hi2c3, 0x10, i2cOUT, 4);
}

uint8_t get_i2c_data(void)
{
		if (huge_updated == 1)
		{
			huge_updated = 0;
			HAL_I2C_Master_Transmit_IT(&hi2c3, 0x10, i2cOUT, 4);		
		}
}
/*----------------------------------------------------------------------------*/
/* check for all devicsec connected to Fascin8                                                              */
/*----------------------------------------------------------------------------*/
/** Initial ADS1299 as
        * Internal clock
        * START Pin hold low
        * Reset ADS once
        * PWDN Pin high
        * SPI turn on
*/
