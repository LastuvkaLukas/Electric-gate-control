/*
 * Name: types.h
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

#include "gpio.h"
#include "stdint.h"
#include "stdbool.h"

typedef struct
{
    unsigned close : 1;
    unsigned open : 1;
    unsigned emergency_stop : 1;
} status_control_reg_t;

typedef struct
{
    unsigned enable : 1;
    unsigned standby : 1;
    unsigned blink : 1;
    unsigned active_emergency_txt : 1;
    unsigned active_big_txt : 1;
    unsigned active_header_txt : 1;
    unsigned active_footer_txt : 1;
} status_display_reg_t;

typedef struct
{
    unsigned enable : 1;
    unsigned stop : 1;
    unsigned direction : 1;
} status_servo_reg_t;

typedef struct
{
    unsigned activated : 1;
} status_sensor_reg_t;

typedef struct
{
    unsigned enable : 1;
    unsigned activate_tx : 1;
    unsigned activate_rx : 1;
    unsigned data_recieved : 1;
} status_lora_reg_t;

typedef struct
{
    unsigned active_emergency : 1;
    status_control_reg_t constrol;
    status_display_reg_t display;
    status_servo_reg_t servo;
    status_sensor_reg_t light_sensorA;
    status_sensor_reg_t light_sensorB;
    status_sensor_reg_t endstop_left;
    status_sensor_reg_t endstop_right;
    status_lora_reg_t lora;
} status_reg_t;

typedef struct
{
    uint16_t token_tx;
    uint16_t token_rx;
    uint8_t control_id;
    uint8_t *payload;
} lora_packet_t;

typedef struct
{
    GPIO_TypeDef *gpio_port;
    uint16_t gpio_pin;
    bool is_press;
    bool last_press;
    bool invert;
    bool activated;
} deb_btn_interrupt_t;

typedef enum
{
    GATE_LORA_ERR = 1,
    GATE_EMERGENCY_STOP
} gate_err_e;

typedef enum
{
    SEND_INFO = 0,
    SEND_ERR
} lora_control_e;