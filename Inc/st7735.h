/*
 * lcdST7735.h
 *
 *  Created on: Aug 13, 2017
 *      Author: paramra
 */

#ifndef __LCDST7735_H__
#define __LCDST7735_H__

#define LCD_RED		31
#define LCD_GREEN	1024
#define LCD_BLUE	0x00F000
#define LCD_YELLOW	1055

typedef struct {
	uint8_t x;
	uint8_t y;
	uint16_t fcolor;
	uint16_t bcolor;
	uint8_t size;
	uint8_t digits;
	uint16_t data;
} Disp7Type;

int  disp7Init(Disp7Type *hdisp, uint8_t x, uint8_t y, uint16_t fcolor,
				uint16_t bcolor, uint8_t size, uint16_t data);
void disp7Update(Disp7Type *hdisp, uint16_t data);

void st7735Init(void);
void st7735Test(void);
void st7735DrawPixel(uint8_t x, uint8_t y, uint16_t color);
void st7735FillRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color);
//void st7735FillRect();
#endif /* LCDST7735_H_ */
