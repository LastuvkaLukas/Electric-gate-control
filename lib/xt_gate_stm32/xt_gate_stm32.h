/*
 * Name: xt_gate_stm32.h
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

#include "inc/types.h"
#include "inc/ssd1306_center.h"

#define LORA_TOKEN_ADDRESS_TX 0xAD11
#define LORA_TOKEN_ADDRESS_RX 0x2117
#define MAX_BTNS_INTERRUPT 8

extern volatile deb_btn_interrupt_t btns[MAX_BTNS_INTERRUPT];

void XT_Gate_Init(void);
void XT_Gate_Loop(void);
void XT_Gate_Error(gate_err_e err);
void XT_Gate_Emergency_Stop(void);
void XT_GATE_Beacon_Blink(uint8_t state);

void XT_Gate_Task_Btn(uint32_t *tick);
void XT_Gate_Task_Lora(uint32_t *tick);