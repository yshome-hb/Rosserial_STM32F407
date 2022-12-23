#include <stdlib.h>
#include <string.h>  // For memcpy
#include <math.h>
#include "io.h"
#include "delay.h"
#include "ssd1306.h"

/**************** SSD1306 defines ****************/
#define SSD1306_GPIO_CLK_ENABLE() {	\
	do{								\
		__GPIOD_CLK_ENABLE(); 		\
	}while(0);						\
}

#define SSD1306_RST_GPIO			GPIOD
#define SSD1306_RST_PIN				GPIO_PIN_12
#define SSD1306_RST                 PDout(12)

#define SSD1306_DC_GPIO			    GPIOD
#define SSD1306_DC_PIN				GPIO_PIN_11
#define SSD1306_DC                  PDout(11)

#define SSD1306_SCL_GPIO			GPIOD
#define SSD1306_SCL_PIN				GPIO_PIN_14
#define SSD1306_SCL                 PDout(14)

#define SSD1306_SDA_GPIO			GPIOD
#define SSD1306_SDA_PIN				GPIO_PIN_13
#define SSD1306_SDA                 PDout(13)

// Screenbuffer
static uint8_t frame_buffer[SSD1306_HEIGHT / 8][SSD1306_WIDTH];

static void ssd1306_reset(void) {
    SSD1306_RST = 0;
    delay_ms(100);
    SSD1306_RST = 1;
}

// Send a byte to the command register
static void ssd1306_writeCommand(uint8_t byte) {
    SSD1306_DC = 0;
    for(int i = 0; i < 8; i++) {
        SSD1306_SCL = 0;
        if(byte & 0x80) SSD1306_SDA = 1;
        else SSD1306_SDA = 0;
        SSD1306_SCL = 1;
        byte <<= 1;
    }
}

// Send data
static void ssd1306_writeData(uint8_t* buffer, int size) {
    SSD1306_DC = 1;
    while(size--) {
        uint8_t b = *buffer;
        buffer++;
        for(int i = 0; i < 8; i++) {
            SSD1306_SCL = 0;
            if(b & 0x80) SSD1306_SDA = 1;
            else SSD1306_SDA = 0;
            SSD1306_SCL = 1;
            b <<= 1;
        }
    }
}

// Initialize the oled screen
void ssd1306_init(void) {

    SSD1306_GPIO_CLK_ENABLE();
    
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

	GPIO_InitStruct.Pin = SSD1306_RST_PIN;
    HAL_GPIO_Init(SSD1306_RST_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SSD1306_DC_PIN;
    HAL_GPIO_Init(SSD1306_DC_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SSD1306_SCL_PIN;
    HAL_GPIO_Init(SSD1306_SCL_GPIO, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SSD1306_SDA_PIN;
    HAL_GPIO_Init(SSD1306_SDA_GPIO, &GPIO_InitStruct);

    // Reset OLED
    ssd1306_reset();

    // Init OLED
    ssd1306_writeCommand(0xAE); //display off

    ssd1306_writeCommand(0x20); //Set Memory Addressing Mode
    ssd1306_writeCommand(0x00); // 00b,Horizontal Addressing Mode; 01b,Vertical Addressing Mode;
                                // 10b,Page Addressing Mode (RESET); 11b,Invalid

    ssd1306_writeCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7

#ifdef SSD1306_MIRROR_VERT
    ssd1306_writeCommand(0xC0); // Mirror vertically
#else
    ssd1306_writeCommand(0xC8); //Set COM Output Scan Direction
#endif

    ssd1306_writeCommand(0x00); //---set low column address
    ssd1306_writeCommand(0x10); //---set high column address

    ssd1306_writeCommand(0x40); //--set start line address - CHECK

    ssd1306_writeCommand(0x81); //--set contrast control register - CHECK
    ssd1306_writeCommand(0x8F);

#ifdef SSD1306_MIRROR_HORIZ
    ssd1306_writeCommand(0xA0); // Mirror horizontally
#else
    ssd1306_writeCommand(0xA1); //--set segment re-map 0 to 127 - CHECK
#endif

#ifdef SSD1306_INVERSE_COLOR
    ssd1306_writeCommand(0xA7); //--set inverse color
#else
    ssd1306_writeCommand(0xA6); //--set normal color
#endif

// Set multiplex ratio.
#if (SSD1306_HEIGHT == 128)
    // Found in the Luma Python lib for SH1106.
    ssd1306_writeCommand(0xFF);
#else
    ssd1306_writeCommand(0xA8); //--set multiplex ratio(1 to 64) - CHECK
#endif

#if (SSD1306_HEIGHT == 32)
    ssd1306_writeCommand(0x1F); //
#elif (SSD1306_HEIGHT == 64)
    ssd1306_writeCommand(0x3F); //
#elif (SSD1306_HEIGHT == 128)
    ssd1306_writeCommand(0x3F); // Seems to work for 128px high displays too.
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    ssd1306_writeCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content

    ssd1306_writeCommand(0xD3); //-set display offset - CHECK
    ssd1306_writeCommand(0x00); //-not offset

    ssd1306_writeCommand(0xD5); //--set display clock divide ratio/oscillator frequency
    ssd1306_writeCommand(0xF0); //--set divide ratio

    ssd1306_writeCommand(0xD9); //--set pre-charge period
    ssd1306_writeCommand(0x22); //

    ssd1306_writeCommand(0xDA); //--set com pins hardware configuration - CHECK
#if (SSD1306_HEIGHT == 32)
    ssd1306_writeCommand(0x02);
#elif (SSD1306_HEIGHT == 64)
    ssd1306_writeCommand(0x12);
#elif (SSD1306_HEIGHT == 128)
    ssd1306_writeCommand(0x12);
#else
#error "Only 32, 64, or 128 lines of height are supported!"
#endif

    ssd1306_writeCommand(0xDB); //--set vcomh
    ssd1306_writeCommand(0x20); //0x20,0.77xVcc

    ssd1306_displayon(); //--turn on SSD1306 panel

    // Clear screen
    ssd1306_clear(0);
    
    // Flush buffer to screen
    ssd1306_updatescreen();
}

void ssd1306_displayon(void) {
    ssd1306_writeCommand(0x8D); //--set DC-DC enable
    ssd1306_writeCommand(0x14); //
    ssd1306_writeCommand(0xAF); //--turn on SSD1306 panel
}

void ssd1306_displayoff(void) {
    ssd1306_writeCommand(0xAE); //display off
    ssd1306_writeCommand(0x8D); //--set DC-DC disable
    ssd1306_writeCommand(0x10); //
}

// Fill the whole screen with the given color
void ssd1306_clear(uint8_t mask) {
    if(mask == 0)
        memset(frame_buffer, 0x00, sizeof(frame_buffer));
    else
        memset(frame_buffer, 0xFF, sizeof(frame_buffer));
}

// Write the screenbuffer with changed to the screen
void ssd1306_updatescreen(void) {
    // Write data to each page of RAM. Number of pages
    // depends on the screen height:
    //
    //  * 32px   ==  4 pages
    //  * 64px   ==  8 pages
    //  * 128px  ==  16 pages
    for(uint8_t i = 0; i < SSD1306_HEIGHT/8; i++) {
        ssd1306_writeCommand(0xB0 + i); // Set the current RAM page address.
        ssd1306_writeCommand(0x00 + SSD1306_X_OFFSET_LOWER);
        ssd1306_writeCommand(0x10 + SSD1306_X_OFFSET_UPPER);
        ssd1306_writeData(frame_buffer[i], SSD1306_WIDTH);
    }
}

// Draw one pixel in the screenbuffer
// X => X Coordinate
// Y => Y Coordinate
// color => Pixel color
void ssd1306_drawpixel(uint8_t x, uint8_t y, uint8_t mask) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        // Don't write outside the buffer
        return;
    }
   
    // Draw in the right color
    if(mask) {
        frame_buffer[y >> 3][x] |= 1 << (y & 0x07);
    } else { 
        frame_buffer[y >> 3][x] &= ~(1 << (y & 0x07));
    }
}

// Draw line by Bresenhem's algorithm
void ssd1306_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint8_t mask) {
    int32_t dx = abs(x1 - x0);
    int32_t dy = abs(y1 - y0);
    int32_t sx = ((x0 < x1) ? 1 : -1);
    int32_t sy = ((y0 < y1) ? 1 : -1);
    int32_t error = dx - dy;
    int32_t error2;
    
    ssd1306_drawpixel(x1, y1, mask);

    while((x0 != x1) || (y0 != y1)) {
        ssd1306_drawpixel(x0, y0, mask);
        error2 = error * 2;
        if(error2 > -dy) {
            error -= dy;
            x0 += sx;
        }
        
        if(error2 < dx) {
            error += dx;
            y0 += sy;
        }
    }
}

//Draw rectangle
void ssd1306_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t mask, uint8_t filled) {
    if (filled) {
        if (x0 == x1) {
            ssd1306_line(x0, y0, x1, y1, mask);
        }
        else if (x0 < x1) {
            for (int32_t x = x0; x <= x1; ++x) {
                ssd1306_line(x, y0, x, y1, mask);
            }
        }
        else {
            for (int32_t x = x1; x <= x0; ++x) {
                ssd1306_line(x, y0, x, y1, mask);
            }
        }
    }
    else {
        ssd1306_line(x0, y0, x1, y0, mask);
        ssd1306_line(x1, y0, x1, y1, mask);
        ssd1306_line(x1, y1, x0, y1, mask);
        ssd1306_line(x0, y1, x0, y0, mask);
    }
}

//Draw circle by Bresenhem's algorithm
void ssd1306_circle(int32_t x, int32_t y, int32_t r, uint8_t mask, uint8_t filled) {
    int32_t ddF_x = 1;
    int32_t ddF_y = -2 * r;

    int32_t f = 1 - r;
    int32_t x1 = 0;
    int32_t y1 = r;

    if (filled) {
        ssd1306_line(x, y + r, x, y - r, mask);
        ssd1306_line(x + r, y, x - r, y, mask);
    }
    else {
        ssd1306_drawpixel(x, y + r, mask);
        ssd1306_drawpixel(x, y - r, mask);
        ssd1306_drawpixel(x + r, y, mask);
        ssd1306_drawpixel(x - r, y, mask);
    }

    while (x1 < y1) {
        if (f >= 0) {
            y1--;
            ddF_y += 2;
            f += ddF_y;
        }
        x1++;
        ddF_x += 2;
        f += ddF_x;
        if (filled) {
            ssd1306_line(x + x1, y + y1, x - x1, y + y1, mask);
            ssd1306_line(x + x1, y - y1, x - x1, y - y1, mask);
            ssd1306_line(x + y1, y + x1, x - y1, y + x1, mask);
            ssd1306_line(x + y1, y - x1, x - y1, y - x1, mask);
        }
        else {
            ssd1306_drawpixel(x + x1, y + y1, mask);
            ssd1306_drawpixel(x - x1, y + y1, mask);
            ssd1306_drawpixel(x + x1, y - y1, mask);
            ssd1306_drawpixel(x - x1, y - y1, mask);
            ssd1306_drawpixel(x + y1, y + x1, mask);
            ssd1306_drawpixel(x - y1, y + x1, mask);
            ssd1306_drawpixel(x + y1, y - x1, mask);
            ssd1306_drawpixel(x - y1, y - x1, mask);
        }
    }
}

// Draw 1 char to the screen buffer
// ch       => char om weg te schrijven
// Font     => Font waarmee we gaan schrijven
// color    => Black or White
char ssd1306_putc(int32_t x, int32_t y, char ch, FontDef font, uint8_t mask) {
    // Check if character is valid
    if (ch < FONT_ASCII_MIN || ch > FONT_ASCII_MAX)
        return 0;
    
    // Check remaining space on current line
    if (SSD1306_WIDTH < (x + font.width) ||
        SSD1306_HEIGHT < (y + font.height)) {
        // Not enough space on current line
        return 0;
    }
    
    // Use the font to write
    for(int32_t i = 0; i < font.height; i++) {
        uint16_t b = font.data[(ch - 32) * font.height + i];
        for(int32_t j = 0; j < font.width; j++) {
            if((b << j) & 0x8000)  {
                ssd1306_drawpixel(x + j, y + i, mask);
            } else {
                ssd1306_drawpixel(x + j, y + i, !mask);
            }
        }
    }

    // Return written char for validation
    return ch;
}

// Write full string to screenbuffer
void ssd1306_print(int32_t x, int32_t y, char* str, FontDef font, uint8_t mask) {
    // Write until null-byte
    while (*str) {
        if (ssd1306_putc(x, y, *str, font, mask) != *str) {
            // Char could not be written
            return;
        }
        
        // Next char
        str++;
        x += font.width;
    }
}

//Draw bitmap - ported from the ADAFruit GFX library
void ssd1306_bitmap(uint8_t x, uint8_t y, const uint8_t* bitmap, uint8_t w, uint8_t h, uint8_t mask) {
    int16_t bytew = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        return;
    }

    for (uint8_t j = 0; j < h; j++, y++) {
        for (uint8_t i = 0; i < w; i++) {
            if (i & 7)
                byte <<= 1;
            else
                byte = (*(const uint8_t *)(&bitmap[j * bytew + i / 8]));

            if (byte & 0x80)
                ssd1306_drawpixel(x + i, y, mask);
        }
    }
}

