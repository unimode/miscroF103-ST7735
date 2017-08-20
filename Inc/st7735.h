/*
 * lcdST7735.h
 *
 *  Created on: Aug 13, 2017
 *      Author: paramra
 */

#ifndef __LCDST7735_H__
#define __LCDST7735_H__

void st7735Init(void);

void st7735Test(void);
void st7735DrawPixel(uint8_t x, uint8_t y, uint16_t color);
//void st7735FillRect();
#endif /* LCDST7735_H_ */
