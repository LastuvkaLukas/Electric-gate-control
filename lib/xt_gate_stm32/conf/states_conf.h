/*
 * Name: states_conf.h
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
#include "stdint.h"

#pragma GCC diagnostic ignored "-Wunknown-pragmas"

#define CONST_STATE(NAME) const state_table_t STATE_##NAME

typedef struct
{
    unsigned sensor_a : 1;
    unsigned sensor_b : 1;
    unsigned endstop_a : 1;
    unsigned endstop_b : 1;
    unsigned emergency_stop : 1;
    unsigned lora_dio : 1;
    unsigned control_a : 1;
    unsigned control_b : 1;
} btns_fld_t;

typedef struct
{
    unsigned servo_start : 1;
    unsigned servo_left : 1;
    unsigned servo_right : 1;
    unsigned lora_rx : 1;
    unsigned mode_a1 : 1;
    unsigned mode_a2 : 1;
    unsigned mode_wait : 1;
    unsigned mode_press : 1;
} flags_fld_t;

typedef struct
{
    union btns_t
    {
        btns_fld_t fld;
        uint8_t ptr;
    } btn_int;

    union flags_t
    {
        flags_fld_t fld;
        uint8_t ptr;
    } flag;
} state_table_data_t;

typedef union
{
    state_table_data_t data;
    uint16_t ptr;
} state_table_t;

#pragma region ed_0 //* EDIT HERE

// TODO: Remove CONST_STATE()
// CONST_STATE(OPEN) = (state_table_t){
//     .data = {
//         .btn_int.fld = {
//             .sensor_a = 0,
//             .sensor_b = 0,
//             .endstop_a = 0,
//             .endstop_b = 1,
//             .emergency_stop = 0,
//             .lora_dio = 0,
//             .control_a = 0,
//             .control_b = 0,
//         },
//         .flag.fld = {
//             .servo_start = 1,
//             .servo_left = 1,
//             .servo_right = 0,
//             .lora_rx = 0,
//             .mode_a1 = 1,
//             .mode_a2 = 0,
//             .mode_wait = 0,
//             .mode_press = 0,
//         },
//     },
// };

#pragma endregion //* EDIT HERE