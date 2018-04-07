/*
 * personalMpu.h
 *
 *  Created on: Mar 27, 2018
 *      Author: Sean Link and Chloe Mena
 *     Purpose: Store the function prototypes and constants specific to the
 *              MPU sensor
 */

#ifndef DRIVERS_MPU_PERSONALMPU_H_
#define DRIVERS_MPU_PERSONALMPU_H_

#define MPU9150_I2C_ADDRESS 0x69

void setupMPU(void);
void MPUDone(void);
void MPUCallback(void *pvCallbackData, uint_fast8_t ui8Status);

#endif /* DRIVERS_MPU_PERSONALMPU_H_ */
