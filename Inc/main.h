/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
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
#define BEACON_Pin GPIO_PIN_0
#define BEACON_GPIO_Port GPIOA
#define SERVO_CH1_Pin GPIO_PIN_5
#define SERVO_CH1_GPIO_Port GPIOA
#define LORA_RST_Pin GPIO_PIN_4
#define LORA_RST_GPIO_Port GPIOC
#define LORA_DIO_Pin GPIO_PIN_10
#define LORA_DIO_GPIO_Port GPIOE
#define LORA_DIO_EXTI_IRQn EXTI15_10_IRQn
#define EMERGENCY_Pin GPIO_PIN_11
#define EMERGENCY_GPIO_Port GPIOE
#define EMERGENCY_EXTI_IRQn EXTI15_10_IRQn
#define LORA_NSS_Pin GPIO_PIN_12
#define LORA_NSS_GPIO_Port GPIOE
#define TEST_LED1_Pin GPIO_PIN_12
#define TEST_LED1_GPIO_Port GPIOD
#define EMERGENCY_LED_Pin GPIO_PIN_14
#define EMERGENCY_LED_GPIO_Port GPIOD
#define TEST_LED3_Pin GPIO_PIN_15
#define TEST_LED3_GPIO_Port GPIOD
#define SENSOR_A_Pin GPIO_PIN_0
#define SENSOR_A_GPIO_Port GPIOD
#define SENSOR_A_EXTI_IRQn EXTI0_IRQn
#define SENSOR_B_Pin GPIO_PIN_1
#define SENSOR_B_GPIO_Port GPIOD
#define SENSOR_B_EXTI_IRQn EXTI1_IRQn
#define ENDSTOP_A_Pin GPIO_PIN_2
#define ENDSTOP_A_GPIO_Port GPIOD
#define ENDSTOP_A_EXTI_IRQn EXTI2_IRQn
#define ENDSTOP_B_Pin GPIO_PIN_3
#define ENDSTOP_B_GPIO_Port GPIOD
#define ENDSTOP_B_EXTI_IRQn EXTI3_IRQn
#define CONTROL_A_Pin GPIO_PIN_6
#define CONTROL_A_GPIO_Port GPIOD
#define CONTROL_A_EXTI_IRQn EXTI9_5_IRQn
#define CONTROL_B_Pin GPIO_PIN_7
#define CONTROL_B_GPIO_Port GPIOD
#define CONTROL_B_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
