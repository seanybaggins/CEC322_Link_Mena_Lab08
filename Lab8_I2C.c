/*
 * Lab8_I2C.c
 *
 * Class: CEC322
 * University: ERAU - Prescott
 * Authors: Sean Link
 * Date: 3/20/2018
 *
 * This lab demonstrates the use of I2C communication where the TM4C
 * micro controller is the master and the MPU is the slave. The MPU
 * gets accelerometer data which is then sent to the main processor.
 * This data is used to create the "bubble level" and to display the
 * information on the board as a floating point number.
 *
 */

// Base includes with the timers Examples
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "grlib/grlib.h"
#include "drivers/cfal96x64x16.h"

// Necessary for blinking LED
#include "drivers/LED/LED.h"
#include "driverlib/gpio.h"

// Necessary for lower case string conversion
#include <ctype.h>

// Necessary for printing to the OLED
#include "drivers/OLED/displays.h"

// Necessary for the functionality of the UART
#include "drivers/UART/personalUART.h"
#include "driverlib/uart.h"

// Necessary for timers
#include "drivers/Timers/personalTimers.h"
#include "driverlib/timer.h"

// Necessary for the I2C
#include "drivers/I2C/personalI2C.h"

// Necessary for Mpu
#include "drivers/MPU/personalMpu.h"

// Necessary for the sensor lib
#include "sensorlib/hw_mpu9150.h"
#include "sensorlib/hw_ak8975.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/ak8975.h"
#include "sensorlib/mpu9150.h"


//****************************************************************************
// Globals
//****************************************************************************
extern tContext sContext;
extern tI2CMInstance g_sI2CInst;
extern tMPU9150 g_sMPU9150Inst;
extern bool globalMpuDone;
uint8_t menuSelection = '\0';

//****************************************************************************
// Main function
//****************************************************************************
int
main(void)
{

    // Enable lazy stacking for interrupt handlers.  This allows floating-point
    // instructions to be used within interrupt handlers, but at the expense of
    // extra stack usage.
    FPULazyStackingEnable();

    // Set the clocking to run directly from the crystal.
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                       SYSCTL_XTAL_16MHZ);

    // Initialize the display driver.
    CFAL96x64x16Init();

    // Disabling all Interrupts
    IntMasterDisable();

    // Initialize the graphics context and find the middle X coordinate.
    GrContextInit(&sContext, &g_sCFAL96x64x16);
    GrContextFontSet(&sContext, g_psFontFixed6x8);

    //*************************************************************************
    // Configuration
    //*************************************************************************
    setupUART();

    setupLED();

    setupTimers();

    setupI2C();

    IntMasterEnable();

    // I2C must be set up before the MPU and Interrupts must be enabled
    setupMPU();

    //************************************************************************
    // Initializing Variables
    //************************************************************************
    uint32_t blinkingLightCounter = 0;
    bool exitProgram = false;
    DisplayMode displayMode = DISPLAY_CIRCLE;

    //************************************************************************
    // starting functional calls and main while loop
    //************************************************************************
    // Displaying Splash Screen
    diplaySplashOnOLED();

    // Displaying UART Menu
    printMainMenu();

    //************************************************************************
    // Main while loop
    //************************************************************************
    while(exitProgram == false) {
        // Blinking the LED
        if (blinkingLightCounter %
                (FIVE_PERCENT_CYCLE_ON + NIENTYFIVE_PERCENT_CYCLE_OFF) <=
                FIVE_PERCENT_CYCLE_ON) {
            GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, GPIO_PIN_2);
        }
        else {
            GPIOPinWrite(GPIO_PORTG_BASE, GPIO_PIN_2, 0);
        }

        blinkingLightCounter++;
        //********************************************************************
        // Functional calls dependent on menu selection
        //********************************************************************

        switch(menuSelection) {
            case 'm' :
                printMainMenu();
                break;
            case 'n' :
                displayMode = DISPLAY_NUMBER;
                break;
            case 'b' :
                displayMode = DISPLAY_CIRCLE;
                break;
            case 'q' :
                exitProgram = true;
                break;
        }
        menuSelection = '\0';

        // Should the MPU be done collecting data, the data will be displayed
        // to the OLED. Whether the information is displayed as the bubble
        // or a floating point number is dependent on the menu selection
        switch(globalMpuDone) {
            case true:{
                static float accelerometerData[3];

                // Reading data from the accelerometer
                MPU9150DataAccelGetFloat(&g_sMPU9150Inst,
                                         &accelerometerData[0],
                                         &accelerometerData[1],
                                         &accelerometerData[2]);

                // Displaying data
                IntMasterDisable();
                clearBlack();
                if (displayMode == DISPLAY_CIRCLE) {
                    drawCrossCenterd();
                    // Note: multiplying by -1 and switching the x and y values
                    // Converts the coordinate system defined on the gyroscope
                    // to the coordinate system defined on the OLED
                    drawCircle((-1)*accelerometerData[1], (-1)*accelerometerData[0]);
                }
                else if (displayMode == DISPLAY_NUMBER) {
                    // Note: multiplying by -1 and switching the x and y values
                    // Converts the coordinate system defined on the gyroscope
                    // to the coordinate system defined on the OLED
                    displayInfoOnBoard((uint8_t *)floatToString(accelerometerData[1]), -1, 25, DISPLAY_NUMBER);
                    displayInfoOnBoard((uint8_t *)floatToString(accelerometerData[0]), -1, 50, DISPLAY_NUMBER);
                }

                IntMasterEnable();
                globalMpuDone = false;

                //Request another reading from the MPU9150
                MPU9150DataRead(&g_sMPU9150Inst, MPUCallback, 0);
            }
        }
    }
    IntMasterDisable();
    clearBlack();
}


//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif
