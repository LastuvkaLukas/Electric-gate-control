/*
 * Name: lora.c
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

#include "lora.h"
#include "inttypes.h"

#define TAG "LoRa"

static void lora_clear_irq(void)
{
    lora_write_reg(LORA_REG_IRQ_FLAGS, 0xff);
}

static void lora_set_mode(lora_reg_op_mode_t mode)
{
    // XT_LOGV(TAG, "Set mode num: %i\n\r", mode);
    lora_clear_irq();
    lora_write_reg(LORA_REG_OP_MODE, LORA_OP_MODE_LONG_RANGE_MODE | mode);
}

static void lora_reset(void)
{
    XT_LOGD(TAG, "Reset module\n\r");
    HAL_GPIO_WritePin(LORA_RST_GPIO_Port, LORA_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LORA_RST_GPIO_Port, LORA_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(10);
}

static void lora_set_tx_power(uint8_t level)
{
    XT_LOGD(TAG, "Set TX power level: %i\n\r", level);
    if (level < 2)
        level = 2;
    else if (level > 17)
        level = 17;
    lora_write_reg(LORA_REG_PA_CONFIG, LORA_PA_CONFIG_PA_SELECT_PA_BOOST | (level - 2));
}

static void lora_set_frequency(uint64_t frequency)
{
    XT_LOGD(TAG, "Set frequency to: %li Mhz\n\r", (uint32_t)(frequency / 1000000));
    uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

    lora_write_reg(LORA_REG_FRF_MSB, (uint8_t)(frf >> 16));
    lora_write_reg(LORA_REG_FRF_MID, (uint8_t)(frf >> 8));
    lora_write_reg(LORA_REG_FRF_LSB, (uint8_t)(frf >> 0));
}

static void lora_set_preamble_length(uint16_t len)
{
    XT_LOGD(TAG, "Set preamble length: %i\n\r", len);
    lora_write_reg(LORA_REG_PREAMBLE_MSB, (uint8_t)(len >> 8));
    lora_write_reg(LORA_REG_PREAMBLE_LSB, (uint8_t)(len >> 0));
}

static void lora_set_sync_word(uint8_t word)
{
    XT_LOGI(TAG, "Set Network ID: %#02x\n\r", word);
    lora_write_reg(LORA_REG_SYNC_WORD, word);
}

static void lora_enable_crc(void)
{
    XT_LOGD(TAG, "Enable CRC in payload\n\r");
    lora_write_reg(LORA_REG_MODEM_CONFIG_2, lora_read_reg(LORA_REG_MODEM_CONFIG_2) | LORA_MODEM_CONFIG2_RX_PAYLOAD_CRC_ON);
}

void lora_write_reg(lora_reg_t reg, uint8_t val)
{
    // XT_LOGV(TAG, "Write %#02x to %#02x register\n\r", val, reg);
    uint8_t tx[2] = {0x80 | reg, val};

    HAL_GPIO_WritePin(LORA_NSS_GPIO_Port, LORA_NSS_Pin, GPIO_PIN_RESET);
    uint32_t res1 = HAL_SPI_Transmit(LORA_SPI, tx, 2, 500);
    HAL_GPIO_WritePin(LORA_NSS_GPIO_Port, LORA_NSS_Pin, GPIO_PIN_SET);

    if (res1 != HAL_OK)
    {
        XT_LOGE(TAG, "SPI transmit failed (%ld)\n\r", res1);
    }
}

uint8_t lora_read_reg(lora_reg_t reg)
{
    // XT_LOGV(TAG, "Read from %#02x register\n\r", reg);
    uint8_t tx[1] = {reg};
    uint8_t rx[1];

    HAL_GPIO_WritePin(LORA_NSS_GPIO_Port, LORA_NSS_Pin, GPIO_PIN_RESET);
    uint32_t res1 = HAL_SPI_Transmit(LORA_SPI, tx, 1, 500);
    uint32_t res2 = HAL_SPI_Receive(LORA_SPI, rx, 1, 500);
    HAL_GPIO_WritePin(LORA_NSS_GPIO_Port, LORA_NSS_Pin, GPIO_PIN_SET);

    if (res1 != HAL_OK || res2 != HAL_OK)
    {
        XT_LOGE(TAG, "SPI transmit/receive failed (%ld %ld)\n\r", res1, res2);
    }

    return rx[0];
}

lora_err_t lora_init(void)
{
    XT_LOGI(TAG, "Initialize LoRa module\n\r");
    lora_err_t err = LORA_OK;

    lora_reset();
    lora_set_mode(LORA_OP_MODE_SLEEP);

    uint8_t i = 0;
    uint8_t version = 0;
    while (i++ < 100)
    {
        version = lora_read_reg(LORA_REG_VERSION);
        if (version == 0x12)
            break;
        HAL_Delay(2);
    }
    if (i >= 100 + 1)
    {
        XT_LOGE(TAG, "LoRa module not valid version 0x12 (%#.2x)\n\r", version);
        return LORA_TIMEOUT;
    }

    lora_write_reg(LORA_REG_FIFO_RX_BASE_ADDR, 0);
    lora_write_reg(LORA_REG_FIFO_TX_BASE_ADDR, 0);

    lora_write_reg(LORA_REG_LNA, lora_read_reg(LORA_REG_LNA) | LORA_LNA_BOOST_HF);
    lora_write_reg(LORA_REG_MODEM_CONFIG_3, LORA_MODEM_CONFIG3_AGC_AUTO_ON);

    lora_set_tx_power(17);
    lora_set_frequency(433000000);
    lora_set_preamble_length(255);
    lora_set_sync_word(0xFD);
    lora_enable_crc();

    lora_set_mode(LORA_OP_MODE_STDBY);
    return err;
}

void lora_sleep(void)
{
    XT_LOGI(TAG, "Go sleep LoRa module\n\r");
    lora_set_mode(LORA_OP_MODE_SLEEP);
}

int16_t lora_packet_rssi(void)
{
    return (lora_read_reg(LORA_REG_PKT_RSSI_VALUE) - (433000000 < 868E6 ? 164 : 157));
}

float lora_packet_snr(void)
{
    return ((int8_t)lora_read_reg(LORA_REG_PKT_SNR_VALUE)) * 0.25;
}

void lora_begin_tx(void)
{
    lora_set_mode(LORA_OP_MODE_STDBY);
    lora_write_reg(LORA_REG_FIFO_ADDR_PTR, 0);
}

lora_err_t lora_write_tx(uint8_t *buf, uint8_t size)
{
    if (size == 0 || (uint16_t)((lora_read_reg(LORA_REG_FIFO_ADDR_PTR) + size)) > 0xff)
        return LORA_INVALID_SIZE;

    volatile size_t i;
    for (i = 0; i < size; i++)
        lora_write_reg(LORA_REG_FIFO, *buf++);

    return LORA_OK;
}

lora_err_t lora_end_tx(void)
{
    uint8_t len = lora_read_reg(LORA_REG_FIFO_ADDR_PTR);
    if (len == 0)
        return LORA_INVALID_SIZE;

    lora_write_reg(LORA_REG_PAYLOAD_LENGTH, len);

    lora_set_mode(LORA_OP_MODE_TX);
    uint8_t read;
    do
    {
        read = lora_read_reg(LORA_REG_IRQ_FLAGS);
        HAL_Delay(2);
    } while ((read & LORA_IRQ_FLAGS_TX_DONE) == 0);

    XT_LOGI(TAG, "Transmit payload with length: %i\n\r", len);
    lora_write_reg(LORA_REG_IRQ_FLAGS, LORA_IRQ_FLAGS_TX_DONE);

    return LORA_OK;
}

void lora_begin_rx_continuous()
{
    lora_set_mode(LORA_OP_MODE_RX_CONTINUOUS);
}

bool lora_check_rx(uint8_t *len)
{
    *len = 0;
    uint8_t read;

    read = lora_read_reg(LORA_REG_IRQ_FLAGS);

    if (read & LORA_IRQ_FLAGS_RX_TIMEOUT)
    {
        lora_begin_rx_continuous();
        return 0;
    }

    if ((read & LORA_IRQ_FLAGS_RX_DONE))
    {
        if (lora_read_reg(LORA_REG_IRQ_FLAGS) & LORA_IRQ_FLAGS_PAYLOAD_CRC_ERROR)
        {
            lora_begin_rx_continuous();
            return 0;
        }

        *len = lora_read_reg(LORA_REG_RX_NB_BYTES);

        XT_LOGI(TAG, "Recieved payload with length: %i\n\r", *len);
        lora_write_reg(LORA_REG_FIFO_ADDR_PTR, lora_read_reg(LORA_REG_FIFO_RX_CURRENT_ADDR));

        return 1;
    }

    return 0;
}

lora_err_t lora_begin_rx(uint8_t *len)
{
    *len = 0;
    uint8_t read;

    lora_set_mode(LORA_OP_MODE_RX_SINGLE);

    for (;;)
    {
        read = lora_read_reg(LORA_REG_IRQ_FLAGS);

        if ((read & LORA_IRQ_FLAGS_RX_DONE))
            break;

        if (read & LORA_IRQ_FLAGS_RX_TIMEOUT)
            return LORA_TIMEOUT;

        HAL_Delay(2);
    }

    if (lora_read_reg(LORA_REG_IRQ_FLAGS) & LORA_IRQ_FLAGS_PAYLOAD_CRC_ERROR)
        return LORA_INVALID_CRC;

    *len = lora_read_reg(LORA_REG_RX_NB_BYTES);

    XT_LOGI(TAG, "Recieved payload with length: %i\n\r", *len);
    lora_write_reg(LORA_REG_FIFO_ADDR_PTR, lora_read_reg(LORA_REG_FIFO_RX_CURRENT_ADDR));

    return LORA_OK;
}

lora_err_t lora_read_rx(uint8_t *buf, uint8_t size)
{

    if (size == 0 || (uint16_t)((lora_read_reg(LORA_REG_FIFO_ADDR_PTR) + size)) > 0xff)
        return LORA_INVALID_SIZE;

    for (int i = 0; i < size; i++)
        *buf++ = lora_read_reg(LORA_REG_FIFO);

    return LORA_OK;
}

void lora_end_rx(void)
{
    lora_set_mode(LORA_OP_MODE_STDBY);
}

void lora_waiting_cad(void)
{
    uint8_t read;

    lora_set_mode(LORA_OP_MODE_CAD);
    for (;;)
    {
        read = lora_read_reg(LORA_REG_IRQ_FLAGS);

        if (read & LORA_IRQ_FLAGS_CAD_DONE)
        {
            if (read & LORA_IRQ_FLAGS_CAD_DETECTED)
                break;
            lora_set_mode(LORA_OP_MODE_CAD);
        }

        HAL_Delay(2);
    }
    XT_LOGD(TAG, "CAD detected\n\r");
}

bool lora_is_cad_detected(void)
{
    lora_set_mode(LORA_OP_MODE_CAD);

    uint8_t read;

    for (;;)
    {
        read = lora_read_reg(LORA_REG_IRQ_FLAGS);
        if (read & LORA_IRQ_FLAGS_CAD_DONE)
        {
            if (read & LORA_IRQ_FLAGS_CAD_DETECTED)
            {
                XT_LOGD(TAG, "CAD detected\n\r");
                return true;
            }
            return false;
        }
    }
}

lora_err_t lora_waiting_cad_rx(uint8_t *len)
{
    lora_waiting_cad();
    return lora_begin_rx(len);
}