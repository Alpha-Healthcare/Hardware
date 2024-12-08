/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define O1_Pin GPIO_PIN_2
#define O1_GPIO_Port GPIOE
#define O2_Pin GPIO_PIN_3
#define O2_GPIO_Port GPIOE
#define W6_Pin GPIO_PIN_4
#define W6_GPIO_Port GPIOE
#define W5_Pin GPIO_PIN_5
#define W5_GPIO_Port GPIOE
#define W4_Pin GPIO_PIN_6
#define W4_GPIO_Port GPIOE
#define W3_Pin GPIO_PIN_13
#define W3_GPIO_Port GPIOC
#define W2_Pin GPIO_PIN_14
#define W2_GPIO_Port GPIOC
#define W1_Pin GPIO_PIN_15
#define W1_GPIO_Port GPIOC
#define ADC_33digital_Pin GPIO_PIN_1
#define ADC_33digital_GPIO_Port GPIOC
#define CLKSEL0_Pin GPIO_PIN_2
#define CLKSEL0_GPIO_Port GPIOC
#define CS0_Pin GPIO_PIN_3
#define CS0_GPIO_Port GPIOC
#define CLKSEL1_Pin GPIO_PIN_0
#define CLKSEL1_GPIO_Port GPIOA
#define DRDY_Pin GPIO_PIN_1
#define DRDY_GPIO_Port GPIOA
#define DRDY_EXTI_IRQn EXTI1_IRQn
#define ADC_25analog_Pin GPIO_PIN_2
#define ADC_25analog_GPIO_Port GPIOA
#define ADC_50power_Pin GPIO_PIN_3
#define ADC_50power_GPIO_Port GPIOA
#define CLKSEL2_Pin GPIO_PIN_10
#define CLKSEL2_GPIO_Port GPIOE
#define CS1_Pin GPIO_PIN_11
#define CS1_GPIO_Port GPIOE
#define CS2_Pin GPIO_PIN_12
#define CS2_GPIO_Port GPIOE
#define START_Pin GPIO_PIN_13
#define START_GPIO_Port GPIOE
#define RSTN_Pin GPIO_PIN_14
#define RSTN_GPIO_Port GPIOE
#define PWDN_Pin GPIO_PIN_15
#define PWDN_GPIO_Port GPIOE
#define STAT1_Pin GPIO_PIN_12
#define STAT1_GPIO_Port GPIOB
#define STAT2_Pin GPIO_PIN_13
#define STAT2_GPIO_Port GPIOB
#define STAT3_Pin GPIO_PIN_14
#define STAT3_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_12
#define LED1_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_13
#define LED2_GPIO_Port GPIOD
#define LED3_Pin GPIO_PIN_14
#define LED3_GPIO_Port GPIOD
#define LED4_Pin GPIO_PIN_15
#define LED4_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */


#define flash_address 0x080E0000
#define firmware_version 26
#define device_uniqeID 0xA400
#define device_series_ID 48 // for Fasin8
#define company_ID 144 // for Infinite8 R Limited Company
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
