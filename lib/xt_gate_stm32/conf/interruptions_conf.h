/*
 * Name: interruptions_conf.h
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
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-variable"

#pragma region ed_0 //* EDIT HERE

#define XT_FOREACH_BTN_INT(XT_BTN_INT) \
    XT_BTN_INT(SENSOR_A)               \
    XT_BTN_INT(SENSOR_B)               \
    XT_BTN_INT(ENDSTOP_A)              \
    XT_BTN_INT(ENDSTOP_B)              \
    XT_BTN_INT(EMERGENCY)              \
    XT_BTN_INT(LORA_DIO)               \
    XT_BTN_INT(CONTROL_A)              \
    XT_BTN_INT(CONTROL_B)

#pragma endregion //* EDIT HERE

#pragma region ag_0 //! Automatically generated

#define XT_GENERATE_ENUM(ENUM) ENUM##_INT,
#define XT_GENERATE_STRING(STRING) #STRING,

typedef enum
{
    XT_FOREACH_BTN_INT(XT_GENERATE_ENUM)
} btn_int_e;

static const char *btn_int_str[] =
    {XT_FOREACH_BTN_INT(XT_GENERATE_STRING)};

#pragma endregion //! Automatically generated