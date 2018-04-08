/*
 * displays.h
 *
 *  Created on: Feb 20, 2018
 *      Author: Sean Link
 *     Purpose: Store constants and function prototypes
 *              specific to the OLED
 *
 */

#ifndef DRIVERS_DISPLAYS_H_
#define DRIVERS_DISPLAYS_H_

#define MAX_SCREEN_WIDTH 96
#define MAX_SCREEN_HEIGHT 64

typedef enum {
    DISPLAY_OFF = 0, DISPLAY_NUMBER = 1, DISPLAY_BAR = 2, DISPLAY_COUNT = 3, DISPLAY_CIRCLE = 4
} DisplayMode;

void diplaySplashOnOLED(void);
void displayInfoOnBoard(uint8_t* formatString,uint32_t ADCValue,
                          uint32_t yLocationOnDisplay, DisplayMode displayMode);
void clearBlack(void);
void drawCircle(int32_t xValue, int32_t yValue);
void drawCrossCenterd(void);
char *floatToString(float x);

#endif /* DRIVERS_DISPLAYS_H_ */
