/*
 * Name: ngp_types.h
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
#ifndef __NGP_TYPES_H__
#define __NGP_TYPES_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "msgpack/zone.h"
#include "msgpack.h"

// GLOBAL MACROS //

#define NGP_NVS "ngp"
#define NGP_NVS_SETTINGS "key"
#define NGP_NVS_HASH "hash"
#define NGP_TOKEN_BROADCAST 0xFFFFU

// TOKEN KEY //

typedef uint16_t ngp_token_t;
typedef uint8_t ngp_id_t;
typedef uint8_t ngp_property_t;

typedef struct ngp_key_t
{
    ngp_token_t token;
    ngp_id_t id;
    ngp_property_t property;
} ngp_key_t;

typedef enum ngp_type_id_t
{
    NGP_ID_EMPTY = 0x00,
    NGP_ID_BROADCAST = 0xff,

    NGP_ID_GATE = '0',
    NGP_ID_NODE = '1',
    NGP_ID_NOT_SPECIFITED = '~',

    NGP_ID_IOT_DEVICE = 'D',
    NGP_ID_IOT_SENSOR = 'S',
    NGP_ID_IOT_BUTTON = 'B',
    NGP_ID_IOT_LIGHT = 'L',
    NGP_ID_IOT_RELE = 'R',
    NGP_ID_IOT_SWITCH = 'W',
} ngp_type_id_t;

// CONFIG //

typedef enum ngp_qos_t
{
    NGP_QOS_0 = 0,
    NGP_QOS_1 = 1,
    NGP_QOS_2 = 2,
} ngp_qos_t;

typedef enum ngp_part_t
{
    NGP_PART_MAIN = 0,
    NGP_PART_NUM_1 = 1,
    NGP_PART_NUM_2 = 2,
    NGP_PART_NUM_3 = 3,
} ngp_part_t;

typedef enum ngp_fns_t
{
    NGP_FNS_NONE = 0,
    NGP_FNS_ECHO = 1,
    NGP_FNS_MSG = 2,
    NGP_FNS_OK = 3,
    NGP_FNS_ERR = 4,
    NGP_FNS_SNTP = 5,
    NGP_FNS_NEW_TOKEN = 6,
    NGP_FNS_ECHO_OK = 7,
} ngp_fns_t;

typedef struct ngp_config_t
{
    ngp_qos_t qos : 2;
    ngp_part_t part : 2;
    ngp_fns_t fns : 4;
    unsigned id : 8 * sizeof(ngp_id_t);
    unsigned property : 8 * sizeof(ngp_property_t);
    unsigned payload_len : 8;
} ngp_config_t;

// TOPIC //

#define NGP_TOPIC_SIZE 4U
typedef uint8_t ngp_topic_t;

// PAYLOAD //

#define NGP_PAYLOAD_MIN_SIZE 1U
typedef uint8_t ngp_payload_t;

// CRC //

typedef uint16_t ngp_crc_t;

// PACKET //

#define NGP_BEGIN_PACKET_SIZE (2 * sizeof(ngp_token_t) + sizeof(ngp_config_t) + NGP_TOPIC_SIZE * sizeof(ngp_topic_t))
#define NGP_END_PACKET_SIZE (sizeof(ngp_crc_t))
#define NGP_PAYLOAD_MAX_SIZE (255U - (NGP_BEGIN_PACKET_SIZE + NGP_END_PACKET_SIZE))

#define NGP_PACKET_HEADER_SIZE (2 * sizeof(ngp_token_t) + sizeof(ngp_config_t) + NGP_TOPIC_SIZE * sizeof(ngp_topic_t))

typedef union ngp_packet_header_t
{
    struct
    {
        ngp_token_t rx;
        ngp_token_t tx;
        ngp_config_t config;
        ngp_topic_t topic[NGP_TOPIC_SIZE];
    } fld;
    uint8_t ptr[NGP_PACKET_HEADER_SIZE];
} ngp_packet_header_t;

#define NGP_PACKET_FOOTER_SIZE (sizeof(ngp_crc_t))

typedef union ngp_packet_footer_t
{
    struct
    {
        ngp_crc_t crc;
    } fld;
    uint8_t ptr[NGP_PACKET_FOOTER_SIZE];
} ngp_packet_footer_t;

typedef struct ngp_packet_t
{
    ngp_packet_header_t header;
    ngp_payload_t *payload;
    ngp_packet_footer_t footer;
} ngp_packet_t;

// MSGPACK //

typedef struct ngp_msgpack_buffer_t
{
    msgpack_sbuffer sbuf;
    msgpack_packer pk;
} ngp_msgpack_buffer_t;

typedef struct ngp_msgpack_object_t
{
    msgpack_zone mempool;
    msgpack_object obj;
} ngp_msgpack_object_t;

// GLOBAL //

typedef enum ngp_global_state_t
{
    NGP_STATE_NONE = -1,
    NGP_SENDED = 0,
    NGP_RECIEVED = 1,
    NGP_RECIEVED_OK = 2,
    NGP_RECIEVED_FAIL = 3,
} ngp_global_state_t;

// MESSAGE INBOX //

typedef struct ngp_msg_inbox_t
{
    ngp_token_t token;
    ngp_config_t config;
    uint64_t wait;
} ngp_msg_inbox_t;

#endif // __NGP_TYPES_H__