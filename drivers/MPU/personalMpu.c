/*
 * personalMpu.c
 *
 *  Created on: Mar 27, 2018
 *      Author: Sean Link and Chloe Mena
 *     Purpose: Store the function definitions specific to the MPU
 */

// Includes must be revisited a;sldkfja;slkdjf;aslkdjf;alskjd;lfajdskljf;lasjdf;
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "personalMpu.h"

#include "driverlib/interrupt.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"

#include "sensorlib/hw_mpu9150.h"
#include "sensorlib/hw_ak8975.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/ak8975.h"
#include "sensorlib/mpu9150.h"

bool globalMpuDone = false;
tI2CMInstance g_sI2CInst;
tMPU9150 g_sMPU9150Inst;

// GPIO Port B Interrupt Service Routine
void MPUDone(void) {
    uint32_t ui32Status;

    // Get the interrupt status.
    ui32Status = GPIOIntStatus(GPIO_PORTB_BASE, true);

    // Clearing interrupt flag
    GPIOIntClear(GPIO_PORTB_BASE, ui32Status);

}

// MPU Callback
void MPUCallback(void *pvCallbackData, uint_fast8_t ui8Status) {
    if (ui8Status == I2CM_STATUS_SUCCESS) {
        globalMpuDone = true;
    }
}

// Called by the NVIC as a result of I2C3 Interrupt. I2C3 is the I2C connection
// to the MPU9150.
void MPU9150I2CIntHandler(void) {
    // Pass through to the I2CM interrupt handler provided by sensor library.
    // This is required to be at application level so that I2CMIntHandler can
    // receive the instance structure pointer as an argument.
    I2CMIntHandler(&g_sI2CInst);
}

// Dependency: I2C Peripheral must be configured first
void setupMPU(void) {

    // Initialize I2C3 instance associated with I2C3
    I2CMInit(&g_sI2CInst, I2C3_BASE, INT_I2C3, 0xff, 0xff,
            SysCtlClockGet());

    // Initialize the MPU.
    globalMpuDone = false;
    MPU9150Init(&g_sMPU9150Inst, &g_sI2CInst, MPU9150_I2C_ADDRESS,
               MPUCallback, 0);
    while(!globalMpuDone);

    // Configuring the MPU for +/- 4 g accelerometer range.
    globalMpuDone = false;
    MPU9150ReadModifyWrite(&g_sMPU9150Inst, MPU9150_O_ACCEL_CONFIG,
                           ~MPU9150_ACCEL_CONFIG_AFS_SEL_M,
                           MPU9150_ACCEL_CONFIG_AFS_SEL_4G,
                           MPUCallback, 0);
    while(!globalMpuDone);
}
