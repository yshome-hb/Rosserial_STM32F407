/*
Copyright 2019 Ryan Caltabiano <https://github.com/XScorpion2>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "common/string.h"
#include "drivers.h"
#include "oled_driver.h"
#include OLED_FONT_H
#include "keyboard.h"

// Used commands from spec sheet: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
// for SH1106: https://www.velleman.eu/downloads/29/infosheets/sh1106_datasheet.pdf

// Fundamental Commands
#define CONTRAST 0x81
#define DISPLAY_ALL_ON 0xA5
#define DISPLAY_ALL_ON_RESUME 0xA4
#define NORMAL_DISPLAY 0xA6
#define INVERT_DISPLAY 0xA7
#define DISPLAY_ON 0xAF
#define DISPLAY_OFF 0xAE
#define NOP 0xE3

// Scrolling Commands
#define ACTIVATE_SCROLL 0x2F
#define DEACTIVATE_SCROLL 0x2E
#define SCROLL_RIGHT 0x26
#define SCROLL_LEFT 0x27
#define SCROLL_RIGHT_UP 0x29
#define SCROLL_LEFT_UP 0x2A

// Addressing Setting Commands
#define MEMORY_MODE 0x20
#define COLUMN_ADDR 0x21
#define PAGE_ADDR 0x22
#define PAM_SETCOLUMN_LSB 0x00
#define PAM_SETCOLUMN_MSB 0x10
#define PAM_PAGE_ADDR 0xB0  // 0xb0 -- 0xb7

// Hardware Configuration Commands
#define DISPLAY_START_LINE 0x40
#define SEGMENT_REMAP 0xA0
#define SEGMENT_REMAP_INV 0xA1
#define MULTIPLEX_RATIO 0xA8
#define COM_SCAN_INC 0xC0
#define COM_SCAN_DEC 0xC8
#define DISPLAY_OFFSET 0xD3
#define COM_PINS 0xDA
#define COM_PINS_SEQ 0x02
#define COM_PINS_ALT 0x12
#define COM_PINS_SEQ_LR 0x22
#define COM_PINS_ALT_LR 0x32

// Timing & Driving Commands
#define DISPLAY_CLOCK 0xD5
#define PRE_CHARGE_PERIOD 0xD9
#define VCOM_DETECT 0xDB

// Advance Graphic Commands
#define FADE_BLINK 0x23
#define ENABLE_FADE 0x20
#define ENABLE_BLINK 0x30

// Charge Pump Commands
#define CHARGE_PUMP 0x8D

// Misc defines
#ifndef OLED_BLOCK_COUNT
#    define OLED_BLOCK_COUNT (sizeof(OLED_BLOCK_TYPE) * 8)
#endif
#ifndef OLED_BLOCK_SIZE
#    define OLED_BLOCK_SIZE (OLED_MATRIX_SIZE / OLED_BLOCK_COUNT)
#endif

#define OLED_ALL_BLOCKS_MASK (((((OLED_BLOCK_TYPE)1 << (OLED_BLOCK_COUNT - 1)) - 1) << 1) | 1)

// i2c defines
#define I2C_CMD  0x00
#define I2C_DATA 0x40

#define I2C_WRITE_CMD(data, size) { \
    i2c_set_id(OLED_DISPLAY_ADDRESS << 1); \
    i2c_write_series(I2C_CMD, 1, (unsigned char *)data, size); \
}

#define I2C_WRITE_DATA(data, size) { \
    i2c_set_id(OLED_DISPLAY_ADDRESS << 1); \
    i2c_write_series(I2C_DATA, 1, (unsigned char *)data, size); \
}

#define HAS_FLAGS(bits, flags) ((bits & flags) == flags)

// Display buffer's is the same as the OLED memory layout
// this is so we don't end up with rounding errors with
// parts of the display unusable or don't get cleared correctly
// and also allows for drawing & inverting
uint8_t         oled_buffer[OLED_MATRIX_SIZE];
uint8_t *       oled_cursor;
OLED_BLOCK_TYPE oled_dirty          = 0;
bool            oled_initialized    = false;
bool            oled_active         = false;
bool            oled_scrolling      = false;
bool            oled_inverted       = false;
uint8_t         oled_brightness     = OLED_BRIGHTNESS;
oled_rotation_t oled_rotation       = 0;
uint8_t         oled_rotation_width = 0;
uint8_t         oled_scroll_speed   = 0;  // this holds the speed after being remapped to ssd1306 internal values
uint8_t         oled_scroll_start   = 0;
uint8_t         oled_scroll_end     = 7;
#if OLED_TIMEOUT > 0
uint32_t oled_timeout;
#endif
#if OLED_SCROLL_TIMEOUT > 0
uint32_t oled_scroll_timeout;
#endif

// Flips the rendering bits for a character at the current cursor position
static void InvertCharacter(uint8_t *cursor) {
    const uint8_t *end = cursor + OLED_FONT_WIDTH;
    while (cursor < end) {
        *cursor = ~(*cursor);
        cursor++;
    }
}

bool oled_init(oled_rotation_t rotation) {

    oled_rotation = oled_init_user(rotation);
    if (!HAS_FLAGS(oled_rotation, OLED_ROTATION_90)) {
        oled_rotation_width = OLED_DISPLAY_WIDTH;
    } else {
        oled_rotation_width = OLED_DISPLAY_HEIGHT;
    }

    uint8_t display_setup1[] = {
        DISPLAY_OFF,
        DISPLAY_CLOCK,
        0x80,
        MULTIPLEX_RATIO,
        OLED_DISPLAY_HEIGHT - 1,
        DISPLAY_OFFSET,
        OLED_DISPLAY_OFFSET,
        DISPLAY_START_LINE | 0x00,
        CHARGE_PUMP,
        0x14,
#if (OLED_IC != OLED_IC_SH1106)
        // MEMORY_MODE is unsupported on SH1106 (Page Addressing only)
        MEMORY_MODE,
        0x00,  // Horizontal addressing mode
#endif
    };
    
    I2C_WRITE_CMD(display_setup1, sizeof(display_setup1));

    if (!HAS_FLAGS(oled_rotation, OLED_ROTATION_180)) {
        uint8_t display_normal[] = {SEGMENT_REMAP_INV, COM_SCAN_DEC};
        I2C_WRITE_CMD(display_normal, sizeof(display_normal));
    } else {
        uint8_t display_flipped[] = {SEGMENT_REMAP, COM_SCAN_INC};
        I2C_WRITE_CMD(display_flipped, sizeof(display_flipped));
    }

    uint8_t display_setup2[] = {COM_PINS, OLED_COM_PINS, CONTRAST, OLED_BRIGHTNESS, PRE_CHARGE_PERIOD, 0xF1, VCOM_DETECT, 0x20, DISPLAY_ALL_ON_RESUME, NORMAL_DISPLAY, DEACTIVATE_SCROLL, DISPLAY_ON};
    I2C_WRITE_CMD(display_setup2, sizeof(display_setup2));

#if OLED_TIMEOUT > 0
    oled_timeout = clock_time();
#endif
#if OLED_SCROLL_TIMEOUT > 0
    oled_scroll_timeout = clock_time();
#endif

    oled_clear();
    oled_initialized = true;
    oled_active      = true;
    oled_scrolling   = false;
    return true;
}

__attribute__((weak)) oled_rotation_t oled_init_user(oled_rotation_t rotation) { return rotation; }

void oled_clear(void) {
    memset(oled_buffer, 0, sizeof(oled_buffer));
    oled_cursor = &oled_buffer[0];
    oled_dirty  = OLED_ALL_BLOCKS_MASK;
}

static void calc_bounds(uint8_t update_start, uint8_t *cmd_array) {
    // Calculate commands to set memory addressing bounds.
    uint8_t start_page   = OLED_BLOCK_SIZE * update_start / OLED_DISPLAY_WIDTH;
    uint8_t start_column = OLED_BLOCK_SIZE * update_start % OLED_DISPLAY_WIDTH;
#if (OLED_IC == OLED_IC_SH1106)
    // Commands for Page Addressing Mode. Sets starting page and column; has no end bound.
    // Column value must be split into high and low nybble and sent as two commands.
    cmd_array[0] = PAM_PAGE_ADDR | start_page;
    cmd_array[1] = PAM_SETCOLUMN_LSB | ((OLED_COLUMN_OFFSET + start_column) & 0x0f);
    cmd_array[2] = PAM_SETCOLUMN_MSB | ((OLED_COLUMN_OFFSET + start_column) >> 4 & 0x0f);
    cmd_array[3] = NOP;
    cmd_array[4] = NOP;
    cmd_array[5] = NOP;
#else
    // Commands for use in Horizontal Addressing mode.
    cmd_array[1] = start_column;
    cmd_array[4] = start_page;
    cmd_array[2] = (OLED_BLOCK_SIZE + OLED_DISPLAY_WIDTH - 1) % OLED_DISPLAY_WIDTH + cmd_array[1];
    cmd_array[5] = (OLED_BLOCK_SIZE + OLED_DISPLAY_WIDTH - 1) / OLED_DISPLAY_WIDTH - 1;
#endif
}

static void calc_bounds_90(uint8_t update_start, uint8_t *cmd_array) {
    cmd_array[1] = OLED_BLOCK_SIZE * update_start / OLED_DISPLAY_HEIGHT * 8;
    cmd_array[4] = OLED_BLOCK_SIZE * update_start % OLED_DISPLAY_HEIGHT;
    cmd_array[2] = (OLED_BLOCK_SIZE + OLED_DISPLAY_HEIGHT - 1) / OLED_DISPLAY_HEIGHT * 8 - 1 + cmd_array[1];
    ;
    cmd_array[5] = (OLED_BLOCK_SIZE + OLED_DISPLAY_HEIGHT - 1) % OLED_DISPLAY_HEIGHT / 8;
}

uint8_t crot(uint8_t a, int8_t n) {
    const uint8_t mask = 0x7;
    n &= mask;
    return a << n | a >> (-n & mask);
}

static void rotate_90(const uint8_t *src, uint8_t *dest) {
    for (uint8_t i = 0, shift = 7; i < 8; ++i, --shift) {
        uint8_t selector = (1 << i);
        for (uint8_t j = 0; j < 8; ++j) {
            dest[i] |= crot(src[j] & selector, shift - (int8_t)j);
        }
    }
}

void oled_render(void) {
    if (!oled_initialized) {
        return;
    }

    // Do we have work to do?
    oled_dirty &= OLED_ALL_BLOCKS_MASK;
    if (!oled_dirty || oled_scrolling) {
        return;
    }

    // Find first dirty block
    uint8_t update_start = 0;
    while (!(oled_dirty & ((OLED_BLOCK_TYPE)1 << update_start))) {
        ++update_start;
    }

    // Set column & page position
    uint8_t display_start[] = {COLUMN_ADDR, 0, OLED_DISPLAY_WIDTH - 1, PAGE_ADDR, 0, OLED_DISPLAY_HEIGHT / 8 - 1};
    if (!HAS_FLAGS(oled_rotation, OLED_ROTATION_90)) {
        calc_bounds(update_start, &display_start[0]);  // Offset from I2C_CMD byte at the start
    } else {
        calc_bounds_90(update_start, &display_start[0]);  // Offset from I2C_CMD byte at the start
    }

    // Send column & page position
    I2C_WRITE_CMD(display_start, sizeof(display_start));

    if (!HAS_FLAGS(oled_rotation, OLED_ROTATION_90)) {
        // Send render data chunk as is
        I2C_WRITE_DATA(&oled_buffer[OLED_BLOCK_SIZE * update_start], OLED_BLOCK_SIZE);
    } else {
        // Rotate the render chunks
        uint8_t source_map[] = OLED_SOURCE_MAP;
        uint8_t target_map[] = OLED_TARGET_MAP;

        uint8_t temp_buffer[OLED_BLOCK_SIZE];
        memset(temp_buffer, 0, sizeof(temp_buffer));
        for (uint8_t i = 0; i < sizeof(source_map); ++i) {
            rotate_90(&oled_buffer[OLED_BLOCK_SIZE * update_start + source_map[i]], &temp_buffer[target_map[i]]);
        }

        // Send render data chunk after rotating
        I2C_WRITE_DATA(&temp_buffer[0], OLED_BLOCK_SIZE);
    }

    // Turn on display if it is off
    oled_on();

    // Clear dirty flag
    oled_dirty &= ~((OLED_BLOCK_TYPE)1 << update_start);
}

void oled_set_cursor(uint8_t col, uint8_t line) {
    uint16_t index = line * oled_rotation_width + col * OLED_FONT_WIDTH;

    // Out of bounds?
    if (index >= OLED_MATRIX_SIZE) {
        index = 0;
    }

    oled_cursor = &oled_buffer[index];
}

void oled_advance_page(bool clearPageRemainder) {
    uint16_t index     = oled_cursor - &oled_buffer[0];
    uint8_t  remaining = oled_rotation_width - (index % oled_rotation_width);

    if (clearPageRemainder) {
        // Remaining Char count
        remaining = remaining / OLED_FONT_WIDTH;

        // Write empty character until next line
        while (remaining--) oled_write_char(' ', false);
    } else {
        // Next page index out of bounds?
        if (index + remaining >= OLED_MATRIX_SIZE) {
            index     = 0;
            remaining = 0;
        }

        oled_cursor = &oled_buffer[index + remaining];
    }
}

void oled_advance_char(void) {
    uint16_t nextIndex      = oled_cursor - &oled_buffer[0] + OLED_FONT_WIDTH;
    uint8_t  remainingSpace = oled_rotation_width - (nextIndex % oled_rotation_width);

    // Do we have enough space on the current line for the next character
    if (remainingSpace < OLED_FONT_WIDTH) {
        nextIndex += remainingSpace;
    }

    // Did we go out of bounds
    if (nextIndex >= OLED_MATRIX_SIZE) {
        nextIndex = 0;
    }

    // Update cursor position
    oled_cursor = &oled_buffer[nextIndex];
}

// Main handler that writes character data to the display buffer
void oled_write_char(const char data, bool invert) {
    // Advance to the next line if newline
    if (data == '\n') {
        // Old source wrote ' ' until end of line...
        oled_advance_page(true);
        return;
    }

    if (data == '\r') {
        oled_advance_page(false);
        return;
    }

    // copy the current render buffer to check for dirty after
    static uint8_t oled_temp_buffer[OLED_FONT_WIDTH];
    memcpy(&oled_temp_buffer, oled_cursor, OLED_FONT_WIDTH);

    // _Static_assert(sizeof(font) >= ((OLED_FONT_END + 1 - OLED_FONT_START) * OLED_FONT_WIDTH), "OLED_FONT_END references outside array");

    // set the reder buffer data
    uint8_t cast_data = (uint8_t)data;  // font based on unsigned type for index
    if (cast_data < OLED_FONT_START || cast_data > OLED_FONT_END) {
        memset(oled_cursor, 0x00, OLED_FONT_WIDTH);
    } else {
        const uint8_t *glyph = &font[(cast_data - OLED_FONT_START) * OLED_FONT_WIDTH];
        memcpy(oled_cursor, glyph, OLED_FONT_WIDTH);
    }

    // Invert if needed
    if (invert) {
        InvertCharacter(oled_cursor);
    }

    // Dirty check
    if (memcmp(&oled_temp_buffer, oled_cursor, OLED_FONT_WIDTH)) {
        uint16_t index = oled_cursor - &oled_buffer[0];
        oled_dirty |= ((OLED_BLOCK_TYPE)1 << (index / OLED_BLOCK_SIZE));
        // Edgecase check if the written data spans the 2 chunks
        oled_dirty |= ((OLED_BLOCK_TYPE)1 << ((index + OLED_FONT_WIDTH - 1) / OLED_BLOCK_SIZE));
    }

    // Finally move to the next char
    oled_advance_char();
}

void oled_write(const char *data, bool invert) {
    const char *end = data + strlen(data);
    while (data < end) {
        oled_write_char(*data, invert);
        data++;
    }
}

void oled_write_ln(const char *data, bool invert) {
    oled_write(data, invert);
    oled_advance_page(true);
}

void oled_pan(bool left) {
    uint16_t i = 0;
    for (uint16_t y = 0; y < OLED_DISPLAY_HEIGHT / 8; y++) {
        if (left) {
            for (uint16_t x = 0; x < OLED_DISPLAY_WIDTH - 1; x++) {
                i              = y * OLED_DISPLAY_WIDTH + x;
                oled_buffer[i] = oled_buffer[i + 1];
            }
        } else {
            for (uint16_t x = OLED_DISPLAY_WIDTH - 1; x > 0; x--) {
                i              = y * OLED_DISPLAY_WIDTH + x;
                oled_buffer[i] = oled_buffer[i - 1];
            }
        }
    }
    oled_dirty = OLED_ALL_BLOCKS_MASK;
}

oled_buffer_reader_t oled_read_raw(uint16_t start_index) {
    if (start_index > OLED_MATRIX_SIZE) start_index = OLED_MATRIX_SIZE;
    oled_buffer_reader_t ret_reader;
    ret_reader.current_element         = &oled_buffer[start_index];
    ret_reader.remaining_element_count = OLED_MATRIX_SIZE - start_index;
    return ret_reader;
}

void oled_write_raw_byte(const char data, uint16_t index) {
    if (index > OLED_MATRIX_SIZE) index = OLED_MATRIX_SIZE;
    if (oled_buffer[index] == data) return;
    oled_buffer[index] = data;
    oled_dirty |= ((OLED_BLOCK_TYPE)1 << (index / OLED_BLOCK_SIZE));
}

void oled_write_raw(const char *data, uint16_t size) {
    uint16_t cursor_start_index = oled_cursor - &oled_buffer[0];
    if ((size + cursor_start_index) > OLED_MATRIX_SIZE) size = OLED_MATRIX_SIZE - cursor_start_index;
    for (uint16_t i = cursor_start_index; i < cursor_start_index + size; i++) {
        uint8_t c = *data++;
        if (oled_buffer[i] == c) continue;
        oled_buffer[i] = c;
        oled_dirty |= ((OLED_BLOCK_TYPE)1 << (i / OLED_BLOCK_SIZE));
    }
}

void oled_write_pixel(uint8_t x, uint8_t y, bool on) {
    if (x >= oled_rotation_width) {
        return;
    }
    uint16_t index = x + (y / 8) * oled_rotation_width;
    if (index >= OLED_MATRIX_SIZE) {
        return;
    }
    uint8_t data = oled_buffer[index];
    if (on) {
        data |= (1 << (y % 8));
    } else {
        data &= ~(1 << (y % 8));
    }
    if (oled_buffer[index] != data) {
        oled_buffer[index] = data;
        oled_dirty |= ((OLED_BLOCK_TYPE)1 << (index / OLED_BLOCK_SIZE));
    }
}

bool oled_on(void) {
    if (!oled_initialized) {
        return oled_active;
    }

#if OLED_TIMEOUT > 0
    oled_timeout = clock_time();
#endif

    uint8_t display_on[] =
#ifdef OLED_FADE_OUT
        {FADE_BLINK, 0x00};
#else
        {DISPLAY_ON};
#endif

    if (!oled_active) {
        I2C_WRITE_CMD(display_on, sizeof(display_on));
        oled_active = true;
    }
    return oled_active;
}

bool oled_off(void) {
    if (!oled_initialized) {
        return !oled_active;
    }

    uint8_t display_off[] =
#ifdef OLED_FADE_OUT
        {FADE_BLINK, ENABLE_FADE | OLED_FADE_OUT_INTERVAL};
#else
        {DISPLAY_OFF};
#endif

    if (oled_active) {
        I2C_WRITE_CMD(display_off, sizeof(display_off));
        oled_active = false;
    }
    return !oled_active;
}

bool is_oled_on(void) { return oled_active; }

uint8_t oled_set_brightness(uint8_t level) {
    if (!oled_initialized) {
        return oled_brightness;
    }

    uint8_t set_contrast[] = {CONTRAST, level};
    if (oled_brightness != level) {
        I2C_WRITE_CMD(set_contrast, sizeof(set_contrast));
        oled_brightness = level;
    }
    return oled_brightness;
}

uint8_t oled_get_brightness(void) { return oled_brightness; }

// Set the specific 8 lines rows of the screen to scroll.
// 0 is the default for start, and 7 for end, which is the entire
// height of the screen.  For 128x32 screens, rows 4-7 are not used.
void oled_scroll_set_area(uint8_t start_line, uint8_t end_line) {
    oled_scroll_start = start_line;
    oled_scroll_end   = end_line;
}

void oled_scroll_set_speed(uint8_t speed) {
    // Sets the speed for scrolling... does not take effect
    // until scrolling is either started or restarted
    // the ssd1306 supports 8 speeds
    // FrameRate2   speed = 7
    // FrameRate3   speed = 4
    // FrameRate4   speed = 5
    // FrameRate5   speed = 0
    // FrameRate25  speed = 6
    // FrameRate64  speed = 1
    // FrameRate128 speed = 2
    // FrameRate256 speed = 3
    // for ease of use these are remaped here to be in order
    static const uint8_t scroll_remap[8] = {7, 4, 5, 0, 6, 1, 2, 3};
    oled_scroll_speed                    = scroll_remap[speed];
}

bool oled_scroll_right(void) {
    if (!oled_initialized) {
        return oled_scrolling;
    }

    // Dont enable scrolling if we need to update the display
    // This prevents scrolling of bad data from starting the scroll too early after init
    if (!oled_dirty && !oled_scrolling) {
        uint8_t display_scroll_right[] = {SCROLL_RIGHT, 0x00, oled_scroll_start, oled_scroll_speed, oled_scroll_end, 0x00, 0xFF, ACTIVATE_SCROLL};
        I2C_WRITE_CMD(display_scroll_right, sizeof(display_scroll_right));
        oled_scrolling = true;
    }
    return oled_scrolling;
}

bool oled_scroll_left(void) {
    if (!oled_initialized) {
        return oled_scrolling;
    }

    // Dont enable scrolling if we need to update the display
    // This prevents scrolling of bad data from starting the scroll too early after init
    if (!oled_dirty && !oled_scrolling) {
        uint8_t display_scroll_left[] = {SCROLL_LEFT, 0x00, oled_scroll_start, oled_scroll_speed, oled_scroll_end, 0x00, 0xFF, ACTIVATE_SCROLL};
        I2C_WRITE_CMD(display_scroll_left, sizeof(display_scroll_left));
        oled_scrolling = true;
    }
    return oled_scrolling;
}

bool oled_scroll_off(void) {
    if (!oled_initialized) {
        return !oled_scrolling;
    }

    if (oled_scrolling) {
        uint8_t display_scroll_off[] = {DEACTIVATE_SCROLL};
        I2C_WRITE_CMD(display_scroll_off, sizeof(display_scroll_off));
        oled_scrolling = false;
        oled_dirty     = OLED_ALL_BLOCKS_MASK;
    }
    return !oled_scrolling;
}

bool is_oled_scrolling(void) { return oled_scrolling; }

bool oled_invert(bool invert) {
    if (!oled_initialized) {
        return oled_inverted;
    }

    if (invert && !oled_inverted) {
        uint8_t display_inverted[] = {INVERT_DISPLAY};
        I2C_WRITE_CMD(display_inverted, sizeof(display_inverted));
        oled_inverted = true;
    } else if (!invert && oled_inverted) {
        uint8_t display_normal[] = {NORMAL_DISPLAY};
        I2C_WRITE_CMD(display_normal, sizeof(display_normal));
        oled_inverted = false;
    }

    return oled_inverted;
}

uint8_t oled_max_chars(void) {
    if (!HAS_FLAGS(oled_rotation, OLED_ROTATION_90)) {
        return OLED_DISPLAY_WIDTH / OLED_FONT_WIDTH;
    }
    return OLED_DISPLAY_HEIGHT / OLED_FONT_WIDTH;
}

uint8_t oled_max_lines(void) {
    if (!HAS_FLAGS(oled_rotation, OLED_ROTATION_90)) {
        return OLED_DISPLAY_HEIGHT / OLED_FONT_HEIGHT;
    }
    return OLED_DISPLAY_WIDTH / OLED_FONT_HEIGHT;
}

void oled_task(void) {
    if (!oled_initialized) {
        return;
    }

    oled_set_cursor(0, 0);
    oled_task_user();

#if OLED_SCROLL_TIMEOUT > 0
    if (oled_dirty && oled_scrolling) {
        oled_scroll_timeout = clock_time();
        oled_scroll_off();
    }
#endif

    // Smart render system, no need to check for dirty
    oled_render();

    // Display timeout check
#if OLED_TIMEOUT > 0
    if (oled_active && clock_time_exceed(oled_timeout, OLED_TIMEOUT)) {
        oled_off();
    }
#endif

#if OLED_SCROLL_TIMEOUT > 0
    if (!oled_scrolling && clock_time_exceed(oled_scroll_timeout, OLED_SCROLL_TIMEOUT)) {
#    ifdef OLED_SCROLL_TIMEOUT_RIGHT
        oled_scroll_right();
#    else
        oled_scroll_left();
#    endif
    }
#endif
}

__attribute__((weak)) void oled_task_user(void) {}
