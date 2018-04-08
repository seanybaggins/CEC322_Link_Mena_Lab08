/*
 * displays.c
 *
 *  Created on: Feb 20, 2018
 *      Author: Sean Link and Andrew Hostetter
 *     Purpose: Store function definitions specific to the OLED
 */
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "displays.h"
#include "grlib/grlib.h"
#include "../MPU/personalMpu.h"

tContext sContext;

// Purpose: Prints a black box over the entire OLED display
void clearBlack(void){

    tRectangle sRect;
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.i16YMax = 69;
    GrContextForegroundSet(&sContext, ClrBlack);
    GrRectFill(&sContext, &sRect);
}

// Purpose: Display data to the OLED display
void displayInfoOnBoard(uint8_t* formatString,uint32_t ADCValue,
                          uint32_t yLocationOnDisplay, DisplayMode displayMode) {
    if(displayMode == DISPLAY_NUMBER) {
        uint8_t displayDataBuffer[16];

        // Setting the text to be white
        GrContextForegroundSet(&sContext, ClrWhite);

        sprintf((char*)displayDataBuffer, (char*)formatString, ADCValue);

        // Prints number to the OLED display
        GrStringDrawCentered(&sContext,(char*)displayDataBuffer, -1,
                             GrContextDpyWidthGet(&sContext) / 2, yLocationOnDisplay, true);
    }
    else if(displayMode == DISPLAY_BAR) {
        tRectangle sRect;

        sRect.i16XMin = 0;
        sRect.i16YMin = yLocationOnDisplay - 4;
        sRect.i16YMax = yLocationOnDisplay + 4;

        // Scaling data down to fit on screen
        sRect.i16XMax = ADCValue / 6783;

        // Setting color of display
        GrContextForegroundSet(&sContext, ClrWhite);

        // Writing bar to screen
        GrRectFill(&sContext, &sRect);
    }
}

// Purpose: Display the splash screen on the OLED display
void diplaySplashOnOLED(void) {

    tRectangle sRect;

    clearBlack();

    // Fill the top part of the screen with blue to create the banner.
    sRect.i16XMin = 0;
    sRect.i16YMin = 0;
    sRect.i16XMax = GrContextDpyWidthGet(&sContext) - 1;
    sRect.i16YMax = 9;
    GrContextForegroundSet(&sContext, ClrDarkBlue);
    GrRectFill(&sContext, &sRect);

    // Change foreground for white text.
    GrContextForegroundSet(&sContext, ClrWhite);

    // Put the lab name in the middle of the banner.
    GrContextFontSet(&sContext, g_psFontFixed6x8);
    GrStringDrawCentered(&sContext, "Sean Link & Chloeleen Mena", -1,
                         GrContextDpyWidthGet(&sContext) / 2, 4, 0);

}

// Purpose: To draw a cross on the center of the board
void drawCrossCenterd(void) {
    // Setting the text to be white
    GrContextForegroundSet(&sContext, ClrWhite);

    uint32_t xCentered = GrContextDpyWidthGet(&sContext)/2;
    uint32_t x1 = xCentered + 3;
    uint32_t x2 = xCentered -3;

    uint32_t yCentered = GrContextDpyHeightGet(&sContext)/2;
    uint32_t y1 = yCentered + 3;
    uint32_t y2 = yCentered -3;

    GrLineDrawH(&sContext, x1, x2, yCentered);
    GrLineDrawV(&sContext, xCentered, y1, y2);
}

// Purpose: To scale data from MPU and draw circle on OLED
void drawCircle(int32_t xValue, int32_t yValue) {
    //scaling values
    xValue = (xValue * MAX_SCREEN_WIDTH / MAX_MPU_RANGE_OFFSET_AT_0)
            + MAX_SCREEN_WIDTH / 2;
    yValue = (yValue * MAX_SCREEN_HEIGHT / MAX_MPU_RANGE_OFFSET_AT_0)
            + MAX_SCREEN_HEIGHT / 2;
    uint32_t radious = 3;
    GrCircleDraw(&sContext, xValue, yValue, radious);

}

// Code provided by Dr. Davis
char *floatToString(float x)
{
  // Use of static allocation here BAD code style
  // Functional but quick & dirty use only
  static char buf[32];
  int mantissa = (int)(x);
  int fraction = abs((int)((x - mantissa)*1000));
  if (fraction > 99)
  {
    sprintf(buf, "%d.%d    ", mantissa, fraction);
  }
  else if (fraction > 9)
  {
    sprintf(buf, "%d.0%d    ", mantissa, fraction);
  }
  else
  {
    sprintf(buf, "%d.00%d    ", mantissa, fraction);
  }

  return buf;
}
