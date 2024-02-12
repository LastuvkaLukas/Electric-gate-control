/*
 * Name: xt_servo_stm32.h
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

#pragma once

#include "tim.h"

/**
 * The interval in seconds for stopping and starting a servo motor.
 */
#define SERVO_STOP_START_INTERVAL 0

/**
 * Enumeration of servo directions.
 * Only for:
 *      Fclk    => 42MHz
 *      ARR     => 1000
 *      PSC     => 840
 */
typedef enum
{
    SERVO_DIR_STOP = 150,
    SERVO_DIR_RIGHT = 100,
    SERVO_DIR_LEFT = 195
} XT_ServoDirection_t;

/**
 * Initializes a servo motor connected to a specific timer and channel.
 *
 * @param timer Pointer to the timer used for controlling the servo motor.
 * @param channel The channel of the timer used for controlling the servo motor.
 *
 * @returns None
 */
void XT_Servo_Init(TIM_HandleTypeDef *timer, uint32_t channel);
/**
 * Deinitializes a servo motor connected to a specific timer and channel.
 *
 * @param timer Pointer to the timer used for controlling the servo motor.
 * @param channel The channel used by the servo motor.
 *
 * @returns None
 */
void XT_Servo_Deinit(TIM_HandleTypeDef *timer, uint32_t channel);
/**
 * Sets the direction of a servo motor.
 *
 * @param timer Pointer to the timer used for controlling the servo motor.
 * @param dir The direction to set for the servo motor.
 *
 * @returns None
 */
void XT_Servo_Set_Direction(TIM_HandleTypeDef *timer, XT_ServoDirection_t dir);
/**
 * Sets the value of a servo connected to a specific timer.
 *
 * @param timer Pointer to the timer used for controlling the servo motor.
 * @param value The value to set for the servo.
 *
 * @returns None
 */
void XT_Servo_Set_Value(TIM_HandleTypeDef *timer, uint32_t value);

uint8_t XT_Servo_Rotated(TIM_HandleTypeDef *timer);

void XT_Servo_Wait_For_State(TIM_HandleTypeDef *timer, uint8_t state);
/**
 * Stops the servo motor controlled by the specified timer.
 *
 * @param timer Pointer to the timer used for controlling the servo motor.
 *
 * @returns None
 */
void XT_Servo_Stop(TIM_HandleTypeDef *timer);