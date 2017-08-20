/*
 * ST7735.c
 *
 *  Created on: Aug 13, 2017
 *      Author: paramra
 */
#include "main.h"
#include "stm32f1xx_hal.h"
#include "spi.h"
#include "gpio.h"
#include "font7x15.h"

extern UART_HandleTypeDef huart2;

static void setCS(uint8_t value)
{
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, value);
}

static void setA0(uint8_t value)
{
	HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, value);
}

static void setRESET(uint8_t value)
{
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, value);
}

static void sendCmd(uint8_t data)
{
	uint8_t t = data;
	setA0(0);
	//HAL_SPI_Transmit_DMA(&hspi2, &t, 1);
	HAL_SPI_Transmit(&hspi2, &t, 1, 5000);
	while(hspi2.Instance->SR  & SPI_SR_BSY);
}

static void sendData(uint8_t data)
{
	uint8_t t = data;
	setA0(1);
	//HAL_SPI_Transmit_DMA(&hspi2, &t, 1);
	HAL_SPI_Transmit(&hspi2, &t, 1, 5000);
	while(hspi2.Instance->SR  & SPI_SR_BSY);
}

void st7735Init(void)
{
	setCS(0);
	HAL_Delay(100);

	// software reset
	sendCmd(0x01);
	HAL_Delay(100);

	// hardware reset
	setRESET(0);
    HAL_Delay(100);
    setRESET(1);
    HAL_Delay(100);

    // wake up
    sendCmd(0x11);
    HAL_Delay(100);

    // color mode 16bit
    sendCmd(0x3A);
    sendData(0x05);

    // direction and color
    sendCmd(0x36);
    sendData(0x1C); // RGB
    //sendData(0x1C); // BGR

    sendCmd(0x29); // turn on display

    // setCS(1); ???

}

void st7735SetRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	sendCmd(0x2A);
	sendData(0x00);
	sendData(x1);
	sendData(0x00);
	sendData(x2);

	sendCmd(0x2B);
	sendData(0x00);
	sendData(y1);
	sendData(0x00);
	sendData(y2);
}

void st7735DrawPixel(uint8_t x, uint8_t y, uint16_t color)
{
	uint8_t tmp;

	st7735SetRect(x, y, x, y);
	sendCmd(0x2C);
	tmp = (uint8_t)((color & 0xFF00)>>8);
	sendData(tmp);
	tmp = (uint8_t)(color & 0x00FF);
	sendData(tmp);
}

void st7735FillRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color)
{
	if(width == 0)
		width = 1;
	if(height == 0)
		height = 1;

	st7735SetRect(x, y, x+width-1, y+height-1);
	sendCmd(0x2C);
	setA0(1);

	HAL_SPI_DeInit(&hspi2);
	hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
	HAL_SPI_Init(&hspi2);

	while(HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_RESET);
	HAL_StatusTypeDef result = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*)(&color), 2*width*height);
	while(hspi2.Instance->SR  & SPI_SR_BSY);
	//HAL_Delay(100);

	HAL_SPI_DeInit(&hspi2);
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	HAL_SPI_Init(&hspi2);
}

void st7735FillRect2(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint16_t color)
{
	uint8_t tmp;
	uint8_t x,y;

	st7735SetRect(x1, y1, x2, y2);
	sendCmd(0x2C);
	setA0(1);

	HAL_SPI_DeInit(&hspi2);
	hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
	HAL_SPI_Init(&hspi2);

	while(HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_RESET);
	HAL_StatusTypeDef result = HAL_SPI_Transmit_DMA(&hspi2, (uint8_t*)(&color), 2*(x2-x1+1)*(y2-y1+1));
	while(hspi2.Instance->SR  & SPI_SR_BSY);


	HAL_SPI_DeInit(&hspi2);
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	HAL_SPI_Init(&hspi2);
}

void st7735DrawRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t color, uint8_t thick)
{
	st7735FillRect(x, y, width, thick, color);
	st7735FillRect(x, y+height-thick, width, thick, color);
	st7735FillRect(x, y, thick, height, color);
	st7735FillRect(x+width-thick, y, thick, height, color);
}

void st7735DrawSymbol(uint8_t x, uint8_t y, uint8_t chr, uint16_t charColor, uint16_t bkgColor)
{
	uint8_t i;
	uint8_t j;

	st7735SetRect(x, y, x+12, y+8);
	sendCmd(0x2C);
	setA0(1);

	HAL_SPI_DeInit(&hspi2);
	hspi2.Init.DataSize = SPI_DATASIZE_16BIT;
	HAL_SPI_Init(&hspi2);

	uint8_t k;
	for (i=0;i<7;i++){
		for (k=2;k>0;k--){
			uint8_t chl=NewBFontLAT[ ( (chr-0x20)*14 + i+ 7*(k-1)) ];
			chl=chl<<2*(k-1);
			uint8_t h;
			if (k==2) h=6; else h=7;
			for (j=0;j<h;j++){
				unsigned int color;
				if(chl & 0x80) color=charColor; else color=bkgColor;
				chl = chl<<1;
				while(HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_RESET);
				HAL_SPI_Transmit(&hspi2, &color, 1, 5000);
				while(hspi2.Instance->SR  & SPI_SR_BSY);
			}
		}
	}

	for (j=0;j<13;j++) {
		while(HAL_SPI_GetState(&hspi2) == HAL_SPI_STATE_RESET);
		HAL_SPI_Transmit(&hspi2, &bkgColor, 1, 5000);
		while(hspi2.Instance->SR  & SPI_SR_BSY);
	}

	HAL_SPI_DeInit(&hspi2);
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	HAL_SPI_Init(&hspi2);

}

void st7735DrawText(uint8_t x, uint8_t y, const uint8_t str[], uint16_t charColor, uint16_t bkgColor) {

	while (*str!=0) {
		st7735DrawSymbol(x, y, *str, charColor, bkgColor);
		y=y+8;
		str++;
	}
}

