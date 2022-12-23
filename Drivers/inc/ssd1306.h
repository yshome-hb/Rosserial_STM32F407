/**
 * This Library was originally written by Olivier Van den Eede (4ilo) in 2016.
 * Some refactoring was done and SPI support was added by Aleksander Alekseev (afiskon) in 2018.
 *
 * https://github.com/afiskon/stm32-ssd1306
 */

#ifndef __SSD1306_H__
#define __SSD1306_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "stm32f4xx.h"
#include "ssd1306_fonts.h"

#ifdef SSD1306_X_OFFSET
#define SSD1306_X_OFFSET_LOWER (SSD1306_X_OFFSET & 0x0F)
#define SSD1306_X_OFFSET_UPPER ((SSD1306_X_OFFSET >> 4) & 0x07)
#else
#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0
#endif

// SSD1306 OLED height in pixels
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT         64
#endif

// SSD1306 width in pixels
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH          128
#endif

// Procedure definitions
void ssd1306_init(void);
void ssd1306_displayon(void);
void ssd1306_displayoff(void);
void ssd1306_clear(uint8_t mask);
void ssd1306_updatescreen(void);
void ssd1306_drawpixel(uint8_t x, uint8_t y, uint8_t mask);
void ssd1306_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t mask);
void ssd1306_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t mask, uint8_t filled);
void ssd1306_circle(int32_t x, int32_t y, int32_t r, uint8_t mask, uint8_t filled);
char ssd1306_putc(int32_t x, int32_t y, char ch, FontDef font, uint8_t mask);
void ssd1306_print(int32_t x, int32_t y, char* str, FontDef font, uint8_t mask);
void ssd1306_bitmap(uint8_t x, uint8_t y, const uint8_t* bitmap, uint8_t w, uint8_t h, uint8_t mask);


#ifdef __cplusplus
}
#endif

#endif // __SSD1306_H__
