/*
 * Name: lora.h
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

#include "lora_reg.h"
#include "xt_debug.h"
#include "stdbool.h"
#include "main.h"
#include "spi.h"
#include "gpio.h"

#define LORA_SPI &hspi2

typedef enum
{
    LORA_OK = 0,
    LORA_TIMEOUT,
    LORA_INVALID_SIZE,
    LORA_INVALID_CRC
} lora_err_t;

void lora_write_reg(lora_reg_t reg, uint8_t val);

uint8_t lora_read_reg(lora_reg_t reg);

lora_err_t lora_init(void);

void lora_sleep(void);

int16_t lora_packet_rssi(void);

float lora_packet_snr(void);

void lora_begin_tx(void);

lora_err_t lora_write_tx(uint8_t *buf, uint8_t size);

lora_err_t lora_end_tx(void);

lora_err_t lora_begin_rx(uint8_t *len);

void lora_begin_rx_continuous(void);

bool lora_check_rx(uint8_t *len);

lora_err_t lora_read_rx(uint8_t *buf, uint8_t size);

void lora_end_rx(void);

void lora_waiting_cad(void);

bool lora_is_cad_detected(void);

lora_err_t lora_waiting_cad_rx(uint8_t *len);