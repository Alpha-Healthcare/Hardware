/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "ads.h"
#include "i2cdev.h"

#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define W1 !HAL_GPIO_ReadPin(W1_GPIO_Port,W1_Pin)
#define W2 !HAL_GPIO_ReadPin(W2_GPIO_Port,W2_Pin)
#define W3 !HAL_GPIO_ReadPin(W3_GPIO_Port,W3_Pin)
#define W4 !HAL_GPIO_ReadPin(W4_GPIO_Port,W4_Pin)
#define W5 !HAL_GPIO_ReadPin(W5_GPIO_Port,W5_Pin)
#define W6 !HAL_GPIO_ReadPin(W6_GPIO_Port,W6_Pin)



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t recivBuff[32], transBuff[128]; //USB CDC transmit and recieve buffers
int new_data_flag_usb = 0; // new data USB arrival
int app_version = 1;
int device_stat = 0;
// SPI & data gathering
ADS_SettingTypeDef device_setting;

uint8_t dummyclk[27];  // zero data for spi recieve data
uint8_t RDATACflag;  // continus read from ADS flag (1: data gathering going on   0:no data gathering)
uint8_t sample_number; // the total number of samples recieved in a gathering session

uint8_t num_of_huges, user_huge_values = 0, huge_updated = 1, huge_colors[8]= {0x33,0x33,0x33,3,3,3,3,3}, huge_stat = 0;
uint8_t i2cIN[4] = {12,12,12,12}, i2cOUT[11];

uint32_t ADCvals[4];
float TemperatureC;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Variable_Defults(void);
void SystemResponseUSB(void);
void Write_Flash(uint32_t address, uint32_t data);
uint32_t Read_Flash(uint32_t address);
void light_pattern(int8_t patt);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_I2C3_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_USB_DEVICE_Init();


  /* USER CODE BEGIN 2 */
  light_pattern(2);
  HAL_GPIO_WritePin(O1_GPIO_Port, O1_Pin, 1);
  HAL_GPIO_WritePin(O2_GPIO_Port, O2_Pin, 1);


  HAL_NVIC_SetPriority(TIM3_IRQn, 2, 2);
  HAL_NVIC_SetPriority(I2C3_EV_IRQn, 1, 1);
  HAL_NVIC_SetPriority(I2C3_ER_IRQn, 1, 1);

  HAL_TIM_Base_Stop_IT(&htim3);
  Variable_Defults();
  HAL_Delay(500);
  ADS_Init(1);

  HAL_GPIO_WritePin(CS0,GPIO_PIN_SET);  //ADS SPI turn off
  HAL_GPIO_WritePin(CS1,GPIO_PIN_SET);  //ADS SPI turn off
  HAL_GPIO_WritePin(CS2,GPIO_PIN_SET);  //ADS SPI turn off


 if ((ADS_Read_Reg(0x00, 0) & 0x0F) != 0x0E) Error_Handler();
  if ((ADS_Read_Reg(0x00, 1) & 0x0F) != 0x0E) Error_Handler();
  if ((ADS_Read_Reg(0x00, 2) & 0x0F) != 0x0E) Error_Handler();


  device_setting.linked_ear = 'n';
  device_setting.TestSignal = 'T'; // test signal
  device_setting.SamplingRate = '8'; //2000 sps
  device_setting.SRBConnection = 'C';  //
  device_setting.Gain = 24;
  device_setting.ExtraGain = 1;
  device_setting.Channels = 0xFFFF;  //
  device_setting.ExtraChannels = 0x00;  //extra channels off
  device_setting.derivation = 0xFF; //all derivation connected

  analog_setting(device_setting);  // write setting

  if ((ADS_Read_Reg(0x00, 0) & 0x0F) != 0x0E) Error_Handler();
  if ((ADS_Read_Reg(0x00, 1) & 0x0F) != 0x0E) Error_Handler();
  if ((ADS_Read_Reg(0x00, 2) & 0x0F) != 0x0E) Error_Handler();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  HAL_Delay(CSdelay);
  HAL_TIM_Base_Start_IT(&htim3);
  light_pattern(1);

  //HAL_ADC_Start_DMA(&hadc1, ADCvals, 4);
  HAL_GPIO_WritePin(O1_GPIO_Port, O1_Pin, 1);
  HAL_GPIO_WritePin(O2_GPIO_Port, O2_Pin, 1);

  //float V25 = 0.76;
  //float Avg_Slope = 2.5;
  //float Temp_ADVal = 0;
  HAL_Delay(200);
//  ADS_Read_Reg(0x00, 1);



  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  SystemResponseUSB();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void CDC_ReceiveCallBack(uint8_t *buf, uint32_t len)
{
	int i;
	for (i=0; i<32; i++)
			recivBuff[i] = buf[i];
	SetLED(3,3,1,3);
	new_data_flag_usb = 1;
}


void SystemResponseUSB(void)
{

	if (new_data_flag_usb == 1 )
		{

			if (recivBuff[0]=='U' && recivBuff[1]=='8' && recivBuff[2]=='_')
				{
					if ( recivBuff[3]=='R') HAL_NVIC_SystemReset();
					else if ( recivBuff[3]=='B' && RDATACflag==0)   //System Begin
									{
										device_stat = 1;
										HAL_TIM_Base_Stop_IT(&htim3);
										HAL_GPIO_WritePin(O1_GPIO_Port, O1_Pin, 0);
										HAL_GPIO_WritePin(O2_GPIO_Port, O2_Pin, 0);
										//if (HAL_I2C_IsDeviceReady(&hi2c3, 0x10, 1, 100) == HAL_OK) start_i2c_data();
										if (huge_stat) start_i2c_data();
										sample_number = 0;
										light_pattern(2);
										ADS_Start_data_full();
									}

					else if ( recivBuff[3]=='E' && RDATACflag==1)
									{
						  HAL_GPIO_WritePin(O1_GPIO_Port, O1_Pin, 1);
						  HAL_GPIO_WritePin(O2_GPIO_Port, O2_Pin, 1);
										 device_stat = 0;
										 light_pattern(1);
										 SetLED(3,0,3,3);
										 ADS_Stop_data_Full();
										 Write_Flash(flash_address, Read_Flash(flash_address)+1);
										 HAL_TIM_Base_Start_IT(&htim3);
									}
//					else if ( recivBuff[3]=='S' && RDATACflag==0  )
//						{
//							light_pattern(3);
//							transBuff[23] = num_of_huges;
//							transBuff[24] = firmware_version;     //program version
//							transBuff[25] = device_uniqeID >> 8;
//							transBuff[26] = (uint8_t)device_uniqeID + W1 + 2*W2 + 4*W3 + 8*W4 + 16*W5 + 32*W6;
//							transBuff[27] = device_series_ID;
//							uint32_t flashdata = Read_Flash(flash_address);  //flash data (number of 'stop' used)
//							transBuff[28] = flashdata >> 16 ;
//							transBuff[29] = flashdata >> 8 ;
//							transBuff[30] = flashdata;
//							transBuff[31] = company_ID;
//
//							CDC_Transmit_FS(transBuff, 32);
//							HAL_Delay(50);
//							light_pattern(1);
//						}
						else if ( recivBuff[3]=='s' && RDATACflag==0  ) // 's' for new setting format
						{
							app_version = 1;
							light_pattern(3);

							device_setting.TestSignal = recivBuff[4];
							device_setting.SamplingRate = recivBuff[5];
							device_setting.LeadoffMode = recivBuff[6];
							device_setting.SRBConnection = recivBuff[7];
							device_setting.Gain = recivBuff[8];
							device_setting.ExtraGain = recivBuff[9];

							device_setting.linked_ear = recivBuff[10];

							device_setting.Channels = 65536 * recivBuff[17] + 256 * recivBuff[16] + recivBuff[15];
							device_setting.ExtraChannels = recivBuff[18];
							//device_setting.derivation = recivBuff[19];
							device_setting.derivation = 0x00;

							analog_setting(device_setting);  // write setting

							huge_colors[0]= recivBuff[20];
							huge_colors[1]= recivBuff[21];
							huge_colors[2]= recivBuff[22];
							huge_colors[3]= recivBuff[23];

							for (unsigned char i = 0; i < 24; i++)   // read back written setting
									transBuff[i] = ADS_Read_Reg(i, 2);


							transBuff[22] = device_setting.derivation;
							transBuff[23] = num_of_huges;

							transBuff[24] = firmware_version;     //program version
							transBuff[25] = device_uniqeID >> 8;
							transBuff[26] = (uint8_t)device_uniqeID + W1 + 2*W2 + 4*W3 + 8*W4 + 16*W5 + 32*W6;
							transBuff[27] = device_series_ID;
							uint32_t flashdata = Read_Flash(flash_address);  //flash data (number of 'stop' used)
							transBuff[28] = flashdata >> 16 ;
							transBuff[29] = flashdata >> 8 ;
							transBuff[30] = flashdata;
							transBuff[31] = company_ID;
							HAL_Delay(50);
							CDC_Transmit_FS(transBuff, 32);
							HAL_Delay(50);
							light_pattern(1);

						   if ((ADS_Read_Reg(0x00, 0) & 0x0F) != 0x0E) Error_Handler();
						  if ((ADS_Read_Reg(0x00, 1) & 0x0F) != 0x0E) Error_Handler();
						   if ((ADS_Read_Reg(0x00, 2) & 0x0F) != 0x0E) Error_Handler();

						}
				}
	     new_data_flag_usb = 0;
			 SetLED(3,3,0,3);
    }
}

void Write_Flash(uint32_t address, uint32_t data)
{
     HAL_FLASH_Unlock();
     __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR );
     FLASH_Erase_Sector(FLASH_SECTOR_11, VOLTAGE_RANGE_3);
     HAL_FLASH_Program(TYPEPROGRAM_WORD, address, data);
     HAL_FLASH_Lock();
}

uint32_t Read_Flash(uint32_t address)
{
		uint32_t Flash_data;
		Flash_data = *(uint32_t*) address;
		return Flash_data;
}

void Variable_Defults(void)
{
	 /* Variables Initialize Values */
		for (unsigned char i = 0; i < 28; i++)	transBuff[i] = i + 20;
		transBuff[24] = 11;
		transBuff[25] = 0;

		for (unsigned char i = 0; i < 27; i++)	dummyclk[i] = 0;
		RDATACflag = 0;

		// Complite SetUp Notification
		SetLED(1,1,1,1); HAL_Delay(500);
		SetLED(1,0,0,0); HAL_Delay(100);
		SetLED(0,1,0,0); HAL_Delay(100);
		SetLED(0,0,1,0); HAL_Delay(100);
		SetLED(0,0,0,1); HAL_Delay(100);
		SetLED(0,0,0,0);
}



void light_pattern(int8_t patt)
{
	if (patt == 0)  // off
		{
			HAL_GPIO_WritePin(STAT1_GPIO_Port, STAT1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(STAT2_GPIO_Port, STAT2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(STAT3_GPIO_Port, STAT3_Pin, GPIO_PIN_RESET);
		}
	else if (patt == 1)  // white simple (idle)
		{
//			HAL_GPIO_WritePin(STAT1_GPIO_Port, STAT1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(STAT1_GPIO_Port, STAT1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(STAT2_GPIO_Port, STAT2_Pin, GPIO_PIN_RESET);
			//HAL_GPIO_WritePin(STAT3_GPIO_Port, STAT3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(STAT3_GPIO_Port, STAT3_Pin, GPIO_PIN_RESET);
		}
	else if (patt == 2)  // White pattern (data gathering)
		{
			HAL_GPIO_WritePin(STAT1_GPIO_Port, STAT1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(STAT2_GPIO_Port, STAT2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(STAT3_GPIO_Port, STAT3_Pin, GPIO_PIN_RESET);
		}
	else if (patt == 3)  // blink blue (setting)
		{
			//HAL_GPIO_WritePin(STAT1_GPIO_Port, STAT1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(STAT1_GPIO_Port, STAT1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(STAT2_GPIO_Port, STAT2_Pin, GPIO_PIN_SET);
//			HAL_GPIO_WritePin(STAT3_GPIO_Port, STAT3_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(STAT3_GPIO_Port, STAT3_Pin, GPIO_PIN_RESET);
		}
	else if (patt == 4)  // blink blue (setting)
		{
			HAL_GPIO_WritePin(STAT1_GPIO_Port, STAT1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(STAT2_GPIO_Port, STAT2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(STAT3_GPIO_Port, STAT3_Pin, GPIO_PIN_SET);
		}
	else
		{
			HAL_GPIO_WritePin(STAT1_GPIO_Port, STAT1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(STAT2_GPIO_Port, STAT2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(STAT3_GPIO_Port, STAT3_Pin, GPIO_PIN_SET);
		}
}

/* ************************************** */
/* ************************************** */
/* ************************************** */

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
			HAL_I2C_Master_Receive_IT(&hi2c3, 0x10, i2cIN, 4);
}


void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{

		if (i2cIN[0] == 'S')
		{
			__NOP();
			huge_stat = 1;
			num_of_huges = i2cIN[2];
		}
		else if (i2cIN[0] == 'D')
		{
			user_huge_values = i2cIN[3];
		}

		huge_updated = 1;

}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
//	if (i2cIN[3] != 0xfa)
//		SetLED (2,2,2,2);
	__NOP();
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	light_pattern(4);
	SetLED(0,0,0,0);
	HAL_TIM_Base_Stop_IT(&htim3);
	while(1)
	{
		SetLED(1,1,1,1);
		HAL_Delay(150);
		SetLED(0,0,0,0);
		HAL_Delay(150);
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
