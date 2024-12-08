#ifndef __ADS_H
#define __ADS_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "main.h"

#define CSdelay   1

#define CLKSEL0 CLKSEL0_GPIO_Port,CLKSEL0_Pin
#define CLKSEL1 CLKSEL1_GPIO_Port,CLKSEL1_Pin
#define CLKSEL2 CLKSEL2_GPIO_Port,CLKSEL2_Pin

#define CS0    CS0_GPIO_Port,CS0_Pin
#define CS1    CS1_GPIO_Port,CS1_Pin
#define CS2    CS1_GPIO_Port,CS2_Pin
#define CSN CS0

#define START  START_GPIO_Port,START_Pin
#define RSTN   RSTN_GPIO_Port,RSTN_Pin
#define PWDN   PWDN_GPIO_Port,PWDN_Pin
#define DRDY   HAL_GPIO_ReadPin(DRDY_GPIO_Port,DRDY_Pin)
	 
	 

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
	
	uint8_t linked_ear;

	uint8_t derivation;

} ADS_SettingTypeDef;

///////////////////////////////////////////////////////////////init
void ADS_Init(int main_ic);
///////////////////////////////////////////////////////////////register
void ADS_Write_Reg(uint8_t Reg, uint8_t val, int i);
uint8_t ADS_Read_Reg(uint8_t Reg, int i);
///////////////////////////////////////////////////////////////command
void ADS_Wakeup(void);
void ADS_Standby(void);
void ADS_Reset(int i);
void ADS_Start(void);
void ADS_Stop(void);
//////////////////////////////////////////////////////////////setting
void ADS_Internal_Ref_Active(int i, uint8_t dd);
void ADS_BIAS_SIGNAL_DERIVATION(uint8_t P, uint8_t N, int i);
void ADS_SRB1_Conection(uint8_t con, int i);
void ADS_LEOF(void);
//void ADS_Setting(int i,char testState, char TxRate, char LFstate, char SRB, char N, char P, uint8_t ch1, uint8_t ch2, uint8_t ch3, uint8_t ch4, uint8_t ch5, uint8_t ch6, uint8_t ch7, uint8_t ch8);

void  analog_setting(ADS_SettingTypeDef sett);
//////////////////////////////////////////////////////////////data gathering
void ADS_Read_data_Single(int i);
void ADS_Read_data_cont(int i);
void ADS_Stop_data_cont(int i);

void ADS_Start_data_one(void);
void ADS_Start_data_full(void);
void ADS_Stop_data_Full(void);



#ifdef __cplusplus
}
#endif

#endif /* __STM32F4xx_HAL_SPI_H */
