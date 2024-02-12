/*
 * Name: strings_conf.h
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

#pragma region ed_0 //* EDIT HERE

#define XT_FOREACH_STRINGS_CMD(XT_STRINGS)    \
    XT_STRINGS(const char *, separator, ",")  \
    XT_STRINGS(const char *, state, "S:")     \
    XT_STRINGS(const char *, direction, "D:") \
    XT_STRINGS(const char *, interrupt, "I:") \
    XT_STRINGS(const char *, ok, "OK")        \
    XT_STRINGS(const char *, err, "ERR")

#define XT_FOREACH_STRINGS_VALUE(XT_STRINGS)       \
    XT_STRINGS(const char *, init, "Starting ...") \
    XT_STRINGS(const char *, empty, "-----")       \
    XT_STRINGS(const char *, open, "open")         \
    XT_STRINGS(const char *, opened, "opened")     \
    XT_STRINGS(const char *, close, "close")       \
    XT_STRINGS(const char *, closed, "closed")     \
    XT_STRINGS(const char *, wait, "wait")         \
    XT_STRINGS(const char *, lora, "LoRa")         \
    XT_STRINGS(const char *, error, "ERR")         \
    XT_STRINGS(const char *, stop, "stop")         \
    XT_STRINGS(const char *, start, "start")         \
    XT_STRINGS(const char *, emergency, "STOP")

#pragma endregion //* EDIT HERE

#pragma region ag_0 //! Automatically generated

#define XT_STRING_D(_, NAME, DATA) .NAME = DATA,
#define XT_STRING_S(TYPE, NAME, DATA) TYPE NAME;

typedef struct
{
    struct cmd_t
    {
        XT_FOREACH_STRINGS_CMD(XT_STRING_S)
    } CMD;
    XT_FOREACH_STRINGS_VALUE(XT_STRING_S)
} xt_strings_t;

static const xt_strings_t xt_strings = {
    .CMD = {XT_FOREACH_STRINGS_CMD(XT_STRING_D)},
    XT_FOREACH_STRINGS_VALUE(XT_STRING_D)};

#pragma endregion //! Automatically generated