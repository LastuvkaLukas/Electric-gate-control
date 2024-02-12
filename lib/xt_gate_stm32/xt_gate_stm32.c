/*
 * Name: xt_gate_stm32.c
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

#include "xt_gate_stm32.h"

#include "conf/strings_conf.h"
#include "conf/interruptions_conf.h"
#include "conf/states_conf.h"
#include "inc/ngp_types.h"

#include "xt_debug.h"
#define TAG "GATE"

#include "xt_servo_stm32.h"
#include "lora.h"

#include "ssd1306.h"
#include "ssd1306_fonts.h"

#include "msgpack.h"
#include "string.h"

#include "main.h"
#include "spi.h"

#define TASK_BTN_TICK 30
#define TASK_LORA_TICK 30
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)         \
    ((byte) & 0x80 ? '1' : '0'),     \
        ((byte) & 0x40 ? '1' : '0'), \
        ((byte) & 0x20 ? '1' : '0'), \
        ((byte) & 0x10 ? '1' : '0'), \
        ((byte) & 0x08 ? '1' : '0'), \
        ((byte) & 0x04 ? '1' : '0'), \
        ((byte) & 0x02 ? '1' : '0'), \
        ((byte) & 0x01 ? '1' : '0')

volatile uint8_t start_task = 0;
volatile deb_btn_interrupt_t btns[MAX_BTNS_INTERRUPT] = {0};
volatile state_table_t state_table = {0};
volatile btns_fld_t *state_table_btns;
volatile flags_fld_t *state_table_flags;

#if DEBUG_LEVEL == DEBUG_LEVEL_VERBOSE && DEBUG_LORA
static char *__NGP_Packet_Print_FNS(ngp_fns_t fns)
{
    switch (fns)
    {
    case NGP_FNS_NONE:
        return "NONE";

    case NGP_FNS_ECHO:
        return "ECHO";

    case NGP_FNS_MSG:
        return "MSG";

    case NGP_FNS_OK:
        return "OK";

    case NGP_FNS_ERR:
        return "ERR";

    case NGP_FNS_SNTP:
        return "SNTP";

    case NGP_FNS_NEW_TOKEN:
        return "NEW TOKEN";

    case NGP_FNS_ECHO_OK:
        return "ECHO OK";

    default:
        return "NONE";
    }
}

static char *__NGP_Packet_Print_ID(ngp_id_t id)
{
    switch (id)
    {
    case NGP_ID_EMPTY:
        return "EMPTY";

    case NGP_ID_BROADCAST:
        return "BROADCAST";

    case NGP_ID_GATE:
        return "GATE";

    case NGP_ID_NODE:
        return "NODE";

    case NGP_ID_NOT_SPECIFITED:
        return "NOT SPECIFIED";

    case NGP_ID_IOT_DEVICE:
        return "IOT DEVICE";

    case NGP_ID_IOT_SENSOR:
        return "IOT SENSOR";

    case NGP_ID_IOT_BUTTON:
        return "IOT BUTTON";

    case NGP_ID_IOT_LIGHT:
        return "IOT LIGHT";

    case NGP_ID_IOT_RELE:
        return "IOT RELE";

    case NGP_ID_IOT_SWITCH:
        return "IOT SWITCH";

    default:
        return "NONE";
    }
}
#endif

static void __NGP_Packet_Print(ngp_packet_t *in)
{
    if (in == NULL)
        return;

#if DEBUG_LEVEL == DEBUG_LEVEL_VERBOSE && DEBUG_LORA
    printf("\n\n********************************\n");
    printf("PACKET:\n\n");

    printf("TOKENS:\n");
    printf("*  rx token: %04x\n", in->header.fld.rx);
    printf("*  tx token: %04x\n", in->header.fld.tx);

    printf("\nCONFIG:\n");
    printf("*  QoS:  %u\n", in->header.fld.config.qos);
    printf("*  Part: %u\n", in->header.fld.config.part);
    printf("*  Fns:  %s (%u)\n", __NGP_Packet_Print_FNS(in->header.fld.config.fns), in->header.fld.config.fns);
    printf("*  ID:   %s (%c)\n", __NGP_Packet_Print_ID(in->header.fld.config.id), in->header.fld.config.id);
    printf("*  Prop: %02x\n", in->header.fld.config.property);
    printf("*  Len:  %u\n", in->header.fld.config.payload_len);

    printf("\nTOPIC: %.*s\n", NGP_TOPIC_SIZE, in->header.fld.topic);

    printf("\nPAYLOAD [HEX]:\n\t");
    for (size_t i = 0; i < in->header.fld.config.payload_len; i++)
    {
        if ((i % 15) == 0)
            printf("\n\t");
        printf("%02x ", in->payload[i]);
    }
    printf("\n");

    printf("\nPAYLOAD [STR]:\n\t");
    printf("%.*s", in->header.fld.config.payload_len, in->payload);
    printf("\n");

    printf("\nCRC: %i", in->footer.fld.crc);
    printf("\n********************************\n\n");
#endif
}

static void __XT_Gate_Task_Btn_Init(void)
{
    size_t i;
    for (i = 0; i < MAX_BTNS_INTERRUPT; i++)
    {
        btns[i].is_press = HAL_GPIO_ReadPin(btns[i].gpio_port, btns[i].gpio_pin);
        btns[i].last_press = btns[i].invert;
    }
}

static void __XT_Gate_Task_Lora_Init(void)
{
    lora_err_t err = lora_init();
    if (err != LORA_OK)
    {
        XT_Gate_Error(GATE_LORA_ERR);
    }
    lora_begin_rx_continuous();
}

static void __XT_Gate_Task_Lora(void)
{
}

static void __XT_Gate_Display(char *header, char *main, char *footer, uint8_t contrast)
{
    ssd1306_SetContrast(contrast);
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    char buf[10];
    sprintf(buf, "ID:%X", LORA_TOKEN_ADDRESS_TX);
    ssd1306_WriteString(buf, Font_7x10, White);
    ssd1306_WriteString(header, Font_7x10, White);
    ssd1306_CenterRowCol(main, Font_16x26, White);
    ssd1306_SetCursor(0, SSD1306_HEIGHT - 10);
    ssd1306_WriteString(footer, Font_7x10, White);
    ssd1306_UpdateScreen();
}

static void __XT_Gate_Table_Update(void)
{
    size_t i;
    state_table_data_t data_table = {0};
    for (i = 0; i < MAX_BTNS_INTERRUPT; i++)
    {
        data_table.btn_int.ptr |= btns[i].activated << i;
    }
    state_table.data.btn_int.ptr = data_table.btn_int.ptr;
    return;
}

static uint8_t __XT_Gate_Parse_Command(ngp_packet_t *packet)
{
    switch ((uint16_t)((*packet->payload << 1) | *(packet->payload + 1)))
    {
    case (uint16_t)(('A' << 1) | '1'):
        return 0;
    case (uint16_t)(('A' << 1) | '2'):
        return 1;
    case (uint16_t)(('B' << 1) | '1'):
        return 2;
    }
    return -1;
}

static void __XT_Gate_Lora_Send(lora_control_e control, gate_err_e err)
{
    ngp_packet_t packet = {
        .header.fld = {
            .rx = LORA_TOKEN_ADDRESS_RX,
            .tx = LORA_TOKEN_ADDRESS_TX,
            .topic = "ga00",
            .config = {
                .fns = NGP_FNS_MSG,
                .id = NGP_ID_IOT_DEVICE,
                .part = NGP_PART_MAIN,
                .property = 0x01,
                .qos = 2,
                .payload_len = 0,
            },
        },
        .footer.fld = {
            .crc = 0,
        },
        .payload = NULL,
    };

    uint8_t buf[255];

    switch ((uint8_t)control)
    {
    case SEND_INFO:
        memset(buf, 0x00, sizeof(buf));
        sprintf((char *)buf, "S:INFO,T:%#.2x,E:%#.2x,I:0b" BYTE_TO_BINARY_PATTERN ",F:0b" BYTE_TO_BINARY_PATTERN, LORA_TOKEN_ADDRESS_TX, err, BYTE_TO_BINARY(state_table.data.btn_int.ptr), BYTE_TO_BINARY(state_table.data.flag.ptr));
        packet.payload = buf;
        packet.header.fld.config.payload_len = strlen((char *)buf);
        break;

    case SEND_ERR:
        memset(buf, 0x00, sizeof(buf));
        sprintf((char *)buf, "S:ERR,T:%#.2x,E:%#.2x,I:0b" BYTE_TO_BINARY_PATTERN ",F:0b" BYTE_TO_BINARY_PATTERN, LORA_TOKEN_ADDRESS_TX, err, BYTE_TO_BINARY(state_table.data.btn_int.ptr), BYTE_TO_BINARY(state_table.data.flag.ptr));
        packet.payload = buf;
        packet.header.fld.config.payload_len = strlen((char *)buf);
        break;
    }

    lora_begin_tx();
    lora_write_tx(packet.header.ptr, sizeof(ngp_packet_header_t));
    lora_write_tx(packet.payload, packet.header.fld.config.payload_len);
    lora_write_tx(packet.footer.ptr, sizeof(ngp_packet_footer_t));
    lora_end_tx();
}

/* States BEGIN */

static void __S_Emergency(void)
{
    // Emergency STOP BTN
    if (state_table_btns->emergency_stop)
    {
        XT_Gate_Emergency_Stop();
    }
}

static void __S_Sensor(void)
{
    // Human sensors detected, set wait mode
    if ((state_table_btns->sensor_a ||
         state_table_btns->sensor_b) &&
        !state_table_flags->mode_wait)
    {
        state_table_flags->mode_wait = 1;

        __XT_Gate_Display(NULL, (char *)xt_strings.wait, NULL, 0xFF);
    }

    // Reset wait mode
    if (!(state_table_btns->sensor_a ||
          state_table_btns->sensor_b) &&
        state_table_flags->mode_wait)
    {
        state_table_flags->mode_wait = 0;
    }
}

static void __S_Mode(void)
{
    // Reset mode A1,A2 if all set
    if (state_table_flags->mode_a1 &&
        state_table_flags->mode_a2)
    {
        state_table_flags->mode_a1 = 0;
        state_table_flags->mode_a2 = 0;
    }

    // Reset mode press
    if (!state_table_btns->control_a &&
        !state_table_btns->control_b &&
        state_table_flags->mode_press)
    {
        state_table_flags->mode_press = 0;
    }

    // Set mode press for Control_A BTN and toggle mode A1
    if (state_table_btns->control_a &&
        !state_table_flags->mode_press)
    {
        state_table_flags->mode_press = 1;
        state_table_flags->mode_a1 = !state_table_flags->mode_a1;
    }

    // Set mode press for Control_B BTN and toggle mode A2
    if (state_table_btns->control_b &&
        !state_table_flags->mode_press)
    {
        state_table_flags->mode_press = 1;
        state_table_flags->mode_a2 = !state_table_flags->mode_a2;
    }
}

static void __S_Endstop(void)
{
    // Show EMPTY on display, no endstop detected and no servo rotated
    if (!state_table_btns->endstop_a &&
        !state_table_btns->endstop_b &&
        !state_table_flags->mode_wait &&
        !state_table_flags->servo_start)
    {
        __XT_Gate_Display(NULL, (char *)xt_strings.empty, NULL, 0x01);
    }

    // Show OPEN on display, endstop_A detected and no servo rotated
    if (state_table_btns->endstop_a &&
        !state_table_btns->endstop_b &&
        !state_table_flags->mode_wait &&
        !state_table_flags->servo_start &&
        !state_table_flags->servo_right)
    {
        __XT_Gate_Display(NULL, (char *)xt_strings.opened, NULL, 0x01);
    }

    // Show CLOSE on display, endstop_B detected and no servo rotated
    if (!state_table_btns->endstop_a &&
        state_table_btns->endstop_b &&
        !state_table_flags->mode_wait &&
        !state_table_flags->servo_start &&
        !state_table_flags->servo_left)
    {
        __XT_Gate_Display(NULL, (char *)xt_strings.closed, NULL, 0x01);
    }

    // Stop OPEN
    if (state_table_btns->endstop_a)
    {
        state_table_flags->mode_a1 = 0;
    }

    // Stop CLOSE
    if (state_table_btns->endstop_b)
    {
        state_table_flags->mode_a2 = 0;
    }
}

static void __S_Servo(void)
{
    // Start OPEN, servo start left rotate, show OPEN on display
    if (state_table_flags->mode_a1 &&
        !state_table_flags->mode_wait &&
        !state_table_flags->servo_start)
    {
        state_table_flags->servo_start = 1;
        state_table_flags->servo_left = 1;
        state_table_flags->servo_right = 0;

        XT_LOGD(TAG, "%s%s%s%s%s\n\r",
                xt_strings.CMD.state,
                xt_strings.start,
                xt_strings.CMD.separator,
                xt_strings.CMD.direction,
                state_table_flags->servo_left ? xt_strings.open : xt_strings.close);

        HAL_TIM_Base_Start_IT(&htim5);
        XT_Servo_Set_Direction(&htim2, SERVO_DIR_LEFT);

        __XT_Gate_Display(NULL, (char *)xt_strings.open, NULL, 0xFF);
    }

    // Start CLOSE, servo start right rotate, show CLOSE on display
    if (state_table_flags->mode_a2 &&
        !state_table_flags->mode_wait &&
        !state_table_flags->servo_start)
    {
        state_table_flags->servo_start = 1;
        state_table_flags->servo_left = 0;
        state_table_flags->servo_right = 1;

        XT_LOGD(TAG, "%s%s%s%s%s\n\r",
                xt_strings.CMD.state,
                xt_strings.start,
                xt_strings.CMD.separator,
                xt_strings.CMD.direction,
                state_table_flags->servo_left ? xt_strings.open : xt_strings.close);

        HAL_TIM_Base_Start_IT(&htim5);
        XT_Servo_Set_Direction(&htim2, SERVO_DIR_RIGHT);

        __XT_Gate_Display(NULL, (char *)xt_strings.close, NULL, 0xFF);
    }

    // Stop OPEN/CLOSE and WAIT, servo stop left/right rotate, show WAIT on display
    if ((state_table_flags->mode_a1 ||
         state_table_flags->mode_a2) &&
        state_table_flags->mode_wait &&
        state_table_flags->servo_start)
    {
        state_table_flags->servo_start = 0;

        XT_LOGD(TAG, "%s%s%s%s%s\n\r",
                xt_strings.CMD.state,
                xt_strings.wait,
                xt_strings.CMD.separator,
                xt_strings.CMD.direction,
                state_table_flags->servo_left ? xt_strings.open : xt_strings.close);

        XT_Servo_Stop(&htim2);
        HAL_TIM_Base_Stop_IT(&htim5);
        XT_GATE_Beacon_Blink(0);

        __XT_Gate_Display(NULL, (char *)xt_strings.wait, NULL, 0xFF);
    }

    // Stop OPEN/CLOSE, servo stop left/right rotate
    if (!state_table_flags->mode_a1 &&
        !state_table_flags->mode_a2 &&
        !state_table_flags->mode_wait &&
        state_table_flags->servo_start)
    {
        state_table_flags->servo_start = 0;
        state_table_flags->mode_a1 = 0;
        state_table_flags->mode_a2 = 0;

        XT_LOGD(TAG, "%s%s%s%s%s\n\r",
                xt_strings.CMD.state,
                xt_strings.stop,
                xt_strings.CMD.separator,
                xt_strings.CMD.direction,
                state_table_flags->servo_left ? xt_strings.open : xt_strings.close);

        state_table_flags->servo_left = 0;
        state_table_flags->servo_right = 0;

        XT_Servo_Stop(&htim2);
        HAL_TIM_Base_Stop_IT(&htim5);
        XT_GATE_Beacon_Blink(0);
    }

    // Stop OPEN/CLOSE/WAIT, servo stop left/right rotate
    if (!state_table_flags->mode_a1 &&
        !state_table_flags->mode_a2 &&
        state_table_flags->mode_wait &&
        (state_table_flags->servo_left ||
         state_table_flags->servo_right))
    {
        state_table_flags->mode_wait = 0;
        state_table_flags->servo_start = 0;
        state_table_flags->mode_a1 = 0;
        state_table_flags->mode_a2 = 0;

        XT_LOGD(TAG, "%s%s%s%s%s\n\r",
                xt_strings.CMD.state,
                xt_strings.stop,
                xt_strings.CMD.separator,
                xt_strings.CMD.direction,
                state_table_flags->servo_left ? xt_strings.open : xt_strings.close);

        state_table_flags->servo_left = 0;
        state_table_flags->servo_right = 0;

        XT_Servo_Stop(&htim2);
        HAL_TIM_Base_Stop_IT(&htim5);
        XT_GATE_Beacon_Blink(0);
    }
}

static void __S_Lora(void)
{
    // LoRa DIO0 check, recieve (A1, A2, B1) command
    if (state_table_btns->lora_dio)
    {
        uint8_t len;
        if (!lora_check_rx(&len))
            return;
        if (len == 0)
            return;

        ngp_packet_t *packet = malloc(sizeof(ngp_packet_t));
        packet->header.fld.config.payload_len = len - (sizeof(ngp_packet_header_t) + sizeof(ngp_packet_footer_t));

        packet->payload = malloc(packet->header.fld.config.payload_len);

        lora_read_rx(packet->header.ptr, sizeof(ngp_packet_header_t));
        lora_read_rx(packet->payload, packet->header.fld.config.payload_len);
        lora_read_rx(packet->footer.ptr, sizeof(ngp_packet_footer_t));
        lora_end_rx();
        if (packet->footer.fld.crc != 0 || packet->header.fld.rx != LORA_TOKEN_ADDRESS_TX)
        {
            goto END;
        }
        __NGP_Packet_Print(packet);

        uint8_t cmd = __XT_Gate_Parse_Command(packet);
        switch (cmd)
        {
        case 0:
            state_table_flags->mode_a1 = !state_table_flags->mode_a1;
            break;
        case 1:
            state_table_flags->mode_a2 = !state_table_flags->mode_a2;
            break;
        case 2:
            __XT_Gate_Lora_Send(SEND_INFO, -1);
            break;
        }

    END:
        free(packet->payload);
        packet->payload = NULL;
        free(packet);
        packet = NULL;
        lora_begin_rx_continuous();
    }
}

/* States END */

void XT_Gate_Init(void)
{
    ssd1306_Init();
    __XT_Gate_Display(NULL, "V1.0", (char *)xt_strings.init, 0xFF);
    HAL_Delay(1000);

    btns[SENSOR_A_INT] = (deb_btn_interrupt_t){
        .gpio_port = SENSOR_A_GPIO_Port,
        .gpio_pin = SENSOR_A_Pin,
        .invert = true,
    };

    btns[SENSOR_B_INT] = (deb_btn_interrupt_t){
        .gpio_port = SENSOR_B_GPIO_Port,
        .gpio_pin = SENSOR_B_Pin,
        .invert = true,
    };

    btns[ENDSTOP_A_INT] = (deb_btn_interrupt_t){
        .gpio_port = ENDSTOP_A_GPIO_Port,
        .gpio_pin = ENDSTOP_A_Pin,
        .invert = true,
    };

    btns[ENDSTOP_B_INT] = (deb_btn_interrupt_t){
        .gpio_port = ENDSTOP_B_GPIO_Port,
        .gpio_pin = ENDSTOP_B_Pin,
        .invert = true,
    };

    btns[EMERGENCY_INT] = (deb_btn_interrupt_t){
        .gpio_port = EMERGENCY_GPIO_Port,
        .gpio_pin = EMERGENCY_Pin,
        .invert = true,
    };

    btns[LORA_DIO_INT] = (deb_btn_interrupt_t){
        .gpio_port = LORA_DIO_GPIO_Port,
        .gpio_pin = LORA_DIO_Pin,
    };

    btns[CONTROL_A_INT] = (deb_btn_interrupt_t){
        .gpio_port = CONTROL_A_GPIO_Port,
        .gpio_pin = CONTROL_A_Pin,
        .invert = true,
    };

    btns[CONTROL_B_INT] = (deb_btn_interrupt_t){
        .gpio_port = CONTROL_B_GPIO_Port,
        .gpio_pin = CONTROL_B_Pin,
        .invert = true,
    };

    XT_Servo_Init(&htim2, TIM_CHANNEL_1);

    __XT_Gate_Task_Btn_Init();
    __XT_Gate_Task_Lora_Init();

    state_table_btns = &state_table.data.btn_int.fld;
    state_table_flags = &state_table.data.flag.fld;

    start_task = 1;
}

void XT_Gate_Loop(void)
{
    __S_Emergency();
    __S_Sensor();
    __S_Mode();
    __S_Endstop();
    __S_Servo();
    __S_Lora();
}

void XT_Gate_Error(gate_err_e err)
{
    XT_Servo_Deinit(&htim2, TIM_CHANNEL_1);
    HAL_TIM_Base_Stop_IT(&htim5);
    XT_GATE_Beacon_Blink(0);
    ssd1306_Fill(Black);
    ssd1306_SetCursor(0, 0);
    char buff[10] = {0};
    if (err == GATE_EMERGENCY_STOP)
    {
        sprintf(buff, "%s", xt_strings.emergency);
    }
    else
    {
        sprintf(buff, "%s %i", xt_strings.error, (uint8_t)err);
    }
    XT_LOGE(TAG, "%s%i\n\r", xt_strings.CMD.err, (uint8_t)err);
    ssd1306_CenterRowCol(buff, Font_16x26, White);
    ssd1306_UpdateScreen();

    if (err != GATE_LORA_ERR)
    {
        __XT_Gate_Lora_Send(SEND_ERR, err);
    }

    for (;;)
    {
        HAL_GPIO_TogglePin(EMERGENCY_LED_GPIO_Port, EMERGENCY_LED_Pin);
        ssd1306_SetContrast(255);
        ssd1306_UpdateScreen();
        HAL_Delay(100);
        ssd1306_SetContrast(100);
        ssd1306_UpdateScreen();
        HAL_Delay(100);
    }
}

void XT_Gate_Emergency_Stop()
{
    XT_Gate_Error(GATE_EMERGENCY_STOP);
}

void XT_GATE_Beacon_Blink(uint8_t state)
{
    if (state)
    {
        HAL_GPIO_TogglePin(BEACON_GPIO_Port, BEACON_Pin);
        return;
    }
    HAL_GPIO_WritePin(BEACON_GPIO_Port, BEACON_Pin, GPIO_PIN_RESET);
}

uint32_t task_button_last_tick = 0;
void XT_Gate_Task_Btn(uint32_t *tick)
{
    if (!start_task)
        return;
    if (*tick >= task_button_last_tick)
    {
        task_button_last_tick = *tick + TASK_BTN_TICK;

        size_t i;
        for (i = 0; i < MAX_BTNS_INTERRUPT; i++)
        {
            if (btns[i].last_press == btns[i].is_press)
            {
                continue;
            }

            btns[i].last_press = btns[i].is_press;

            if (btns[i].is_press ^ btns[i].invert)
            {
                btns[i].activated = true;
            }
            else
            {
                btns[i].activated = false;
            }
            XT_LOGV(TAG, "%s%i%s%s%3s\n\r",
                    xt_strings.CMD.state, btns[i].activated,
                    xt_strings.CMD.separator,
                    xt_strings.CMD.interrupt, btn_int_str[i]);

            __XT_Gate_Table_Update();
        }
    }
}

uint32_t task_lora_last_tick = 0;
void XT_Gate_Task_Lora(uint32_t *tick)
{
    if (!start_task)
        return;
    if (*tick >= task_lora_last_tick)
    {
        task_lora_last_tick = *tick + TASK_LORA_TICK;

        __XT_Gate_Task_Lora();
    }
}