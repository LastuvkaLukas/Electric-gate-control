/*
 * Name: ssd1306_center.c
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

#include "ssd1306_center.h"

void ssd1306_CenterRow(char *str, FontDef font, SSD1306_COLOR color)
{
    uint8_t size = strlen(str) * font.FontWidth;
    if (size < SSD1306_WIDTH - 1)
        ssd1306_SetCursor((SSD1306_WIDTH - size) / 2, 0);
    ssd1306_WriteString(str, font, color);
}

void ssd1306_CenterCol(char *str, FontDef font, SSD1306_COLOR color)
{
    uint8_t size = (SSD1306_HEIGHT / 2) - (font.FontHeight / 2);
    ssd1306_SetCursor(0, size);
    ssd1306_WriteString(str, font, color);
}

void ssd1306_CenterRowCol(char *str, FontDef font, SSD1306_COLOR color)
{
    uint8_t row = strlen(str) * font.FontWidth;
    if (row < SSD1306_WIDTH - 1)
        row = (SSD1306_WIDTH - row) / 2;
    else
        row = 0;

    ssd1306_SetCursor(row, (SSD1306_HEIGHT / 2) - (font.FontHeight / 2));
    ssd1306_WriteString(str, font, color);
}