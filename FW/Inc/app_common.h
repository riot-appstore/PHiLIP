/*
 * app_common.h
 *
 *  Created on: May 17, 2018
 *      Author: kevinweiss
 */

#ifndef APP_COMMON_H_
#define APP_COMMON_H_

void soft_reset();
void enable_interrupt();
void disable_interrupt();

/*
 * @brief	Enables interrupts.
 *
 */
#define EN_INT enable_interrupt()

/*
 * @brief	Disables interrupts.
 *
 */
#define DIS_INT disable_interrupt()

/*
 * @brief	Disables interrupts.
 *
 */
#define SOFT_RESET	soft_reset()



void delay_us(uint16_t micros);

#endif /* APP_COMMON_H_ */
