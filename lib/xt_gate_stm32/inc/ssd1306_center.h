/*
 * Name: ssd1306_center.h
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

#include "ssd1306.h"
#include "ssd1306_conf.h"
#include "string.h"

void ssd1306_CenterRow(char *str, FontDef font, SSD1306_COLOR color);

void ssd1306_CenterCol(char *str, FontDef font, SSD1306_COLOR color);

void ssd1306_CenterRowCol(char *str, FontDef font, SSD1306_COLOR color);