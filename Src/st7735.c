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
	HAL_SPI_Transmit(&hspi2, &t, 1, 5000);
}

static void sendData(uint8_t data)
{
	uint8_t t = data;
	setA0(1);
	HAL_SPI_Transmit(&hspi2, &t, 1, 5000);
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




