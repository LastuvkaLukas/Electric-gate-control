/*
 * Name: xt_servo_stm32.c
 * Description:
 *
 * Created Date: January 2024
 * Author:
 * 		Lukáš Laštůvka
 * 		https://github.com/LastuvkaLukas
 *
 * License: Apache License 2.0 (https://www.apache.org/licenses/LICENSE-2.0)
 * Copyright (c) 2024 Lukáš Laštůvka
 *
 */

#include "xt_servo_stm32.h"

/**
 * Maps a value from one range to another range.
 *
 * @param x The value to be mapped.
 * @param in_min The minimum value of the input range.
 * @param in_max The maximum value of the input range.
 * @param out_min The minimum value of the output range.
 * @param out_max The maximum value of the output range.
 *
 * @returns The mapped value.
 */
static uint32_t __map(uint32_t x, uint32_t in_min, uint32_t in_max, uint32_t out_min, uint32_t out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static uint8_t __rotated = 0;

void XT_Servo_Init(TIM_HandleTypeDef *timer, uint32_t channel)
{
    timer->Instance->CCR1 = SERVO_DIR_STOP;
    HAL_TIM_PWM_Start(timer, channel);
}

void XT_Servo_Deinit(TIM_HandleTypeDef *timer, uint32_t channel)
{
    XT_Servo_Stop(&htim2);
    HAL_TIM_PWM_Stop(timer, channel);
}

void XT_Servo_Set_Direction(TIM_HandleTypeDef *timer, XT_ServoDirection_t dir)
{
    XT_Servo_Stop(timer);
    int16_t i;
    if (dir == SERVO_DIR_RIGHT)
    {
        for (i = 90; i >= 0; i--)
        {
            XT_Servo_Set_Value(timer, i);
            HAL_Delay(SERVO_STOP_START_INTERVAL);
        }
        __rotated = 1;
        return;
    }
    for (i = 90; i <= 180; i++)
    {
        XT_Servo_Set_Value(timer, i);
        HAL_Delay(SERVO_STOP_START_INTERVAL);
    }
    __rotated = 1;
}

void XT_Servo_Stop(TIM_HandleTypeDef *timer)
{
    int16_t i;
    uint8_t state = __map(timer->Instance->CCR1, SERVO_DIR_RIGHT, SERVO_DIR_LEFT, 0, 180);
    if (state > 90)
    {
        for (i = state; i >= 90; i--)
        {
            XT_Servo_Set_Value(timer, i);
            HAL_Delay(SERVO_STOP_START_INTERVAL);
        }
        __rotated = 0;
        return;
    }
    for (i = state; i <= 90; i++)
    {
        XT_Servo_Set_Value(timer, i);
        HAL_Delay(SERVO_STOP_START_INTERVAL);
    }
    __rotated = 0;
}

void XT_Servo_Set_Value(TIM_HandleTypeDef *timer, uint32_t value)
{
    if (value < 0 || value > 180)
        Error_Handler();
    timer->Instance->CCR1 = __map(value, 0, 180, SERVO_DIR_RIGHT, SERVO_DIR_LEFT);
}

uint8_t XT_Servo_Rotated(TIM_HandleTypeDef *timer)
{
    return __rotated;
}

void XT_Servo_Wait_For_State(TIM_HandleTypeDef *timer, uint8_t state)
{
    while (__rotated != state)
    {
        HAL_Delay(1);
    }
}
