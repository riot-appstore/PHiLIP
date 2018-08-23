/*
 * app.h
 *
 *  Created on: May 16, 2018
 *      Author: kevinweiss
 */

#ifndef APP_H_
#define APP_H_

#include <stdint.h>
#include <errno.h>

uint32_t get_reg_size();

error_t increase_reg_uint8(uint32_t index, uint8_t access);
error_t increase_reg_int8(uint32_t index, uint8_t access);
error_t increase_reg_uint16(uint32_t index, uint8_t access);
error_t increase_reg_int16(uint32_t index, uint8_t access);
error_t increase_reg_uint32(uint32_t index, uint8_t access);
error_t increase_reg_int32(uint32_t index, uint8_t access);
error_t decrease_reg_uint8(uint32_t index, uint8_t access);
error_t decrease_reg_int8(uint32_t index, uint8_t access);
error_t decrease_reg_uint16(uint32_t index, uint8_t access);
error_t decrease_reg_int16(uint32_t index, uint8_t access);
error_t decrease_reg_uint32(uint32_t index, uint8_t access);
error_t decrease_reg_int32(uint32_t index, uint8_t access);
error_t read_reg(uint32_t index, uint8_t *data);
error_t read_regs(uint32_t index, uint8_t *data, uint16_t size);
error_t write_reg(uint32_t index, uint8_t data, uint8_t access);
error_t write_regs(uint32_t index, uint8_t *data, uint16_t size, uint8_t access);

error_t execute_reg_change();

#endif /* APP_H_ */
