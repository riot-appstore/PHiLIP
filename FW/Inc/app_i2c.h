/*
 * app_i2c.h
 *
 *  Created on: May 16, 2018
 *      Author: kevinweiss
 */

#ifndef APP_I2C_H_
#define APP_I2C_H_

#include <errno.h>

error_t app_i2c_execute(i2c_t *i2c);

error_t app_i2c_init(I2C_HandleTypeDef *hi2c);
void i2c_err(I2C_HandleTypeDef* hi2c);
void i2c_it(I2C_HandleTypeDef* hi2c);

#endif /* APP_I2C_H_ */
