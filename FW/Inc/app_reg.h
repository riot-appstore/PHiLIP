/*
 * MIT License
 *
 * Copyright 2018 Kevin Weiss for HAW Hamburg
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 ******************************************************************************
 * @addtogroup Application
 * @{
 * @file			app_reg.h
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			Accessing application registers.
 * @details			Deals with setting up registers, reading, writing and
 * 					access.
 ******************************************************************************
 */

#ifndef APP_REG_H_
#define APP_REG_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <errno.h>

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief		Initializes application registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @param[in]	saved_reg	Pointer to saved register memory map
 * @note		Populates system user registers and assigns register pointers.
 */
void init_app_reg(map_t *reg, map_t *saved_reg);

/**
 * @brief		Executes or commits all register changes on the periphs.
 *
 * @pre			All initializations must be called first.
 * @return      ERRNO error code
 */
error_t execute_reg_change();

/**
 * @brief		Gets the total size of the register map in bytes.
 *
 * @return      Number of bytes in register map
 */
uint32_t get_reg_size();

/**
 * @brief		An unprotected read from the register map.
 *
 * @param[in]	index 	The index of the register map to start to read
 * @param[out]	data	Pointer to data that was read
 *
 * @return      EOK on success
 * @return      EOVERFLOW if trying to read out of range
 *
 * @details		This is a direct read from the register without any protection,
 * 				the purpose is for speed.  This should also be used for
 * 				entering data directly to hardware registers.
 *
 * @warning		No interrupt protection or memory protection.
 */
void unprotected_read_uint8(uint32_t index, uint8_t *data);

/**
 * @brief		Reads a register from the register map.
 *
 * @param[in]	index 	The index of the register map to start to read
 * @param[out]	data	Pointer to data that was read
 *
 * @return      EOK on success
 * @return      EOVERFLOW if trying to read out of range
 *
 */
error_t read_reg(uint32_t index, uint8_t *data);

/**
 * @brief		Reads multiple registers from the register map.
 *
 * @param[in]	index 	The index of the register map to start to read
 * @param[out]	data	Pointer to data that was read
 * @param[in]	size 	The amount of bytes to read
 *
 * @return      EOK on success
 * @return      EOVERFLOW if trying to read out of range
 *
 * @warning		Multiple writes to data may occur so don't pass hardware
 * 				register addresses.
 */
error_t read_regs(uint32_t index, uint8_t *data, uint16_t size);

/**
 * @brief		Writes a register from the register map.
 *
 * @param[in]	index 	The index of the register map to start to write
 * @param[out]	data	Pointer to data that was write
 * @param[in]	access 	The access level of the caller
 *
 * @return      EOK on success
 * @return      EOVERFLOW if trying to read out of range
 * @return      EACCESS caller does not have write access
 *
 * @warning		Has interrupt protected to safely write, can cause jitter.
 */
error_t write_reg(uint32_t index, uint8_t data, uint8_t access);

/**
 * @brief		Writes multiple register from the register map.
 *
 * @param[in]	index 	The index of the register map to start to write
 * @param[out]	data	Pointer to data that was write
 * @param[in]	size 	The amount of bytes to write
 * @param[in]	access 	The access level of the caller
 *
 * @return      EOK on success
 * @return      EOVERFLOW if trying to read out of range
 * @return      EACCESS caller does not have write access
 *
 * @warning		Has interrupt protected to safely write, can cause jitter.
 */
error_t write_regs(uint32_t index, uint8_t *data, uint16_t size, uint8_t access);

/**
 * @brief			Adds a number and wraps around if register size exceeded
 *
 * @param[in,out]	i 	index to increment
 */
void add_index(uint16_t *i);

/**
 * @brief			Subtracts a number and wraps around if 0
 *
 * @param[in,out]	i 	index to decrement
 */
void sub_index(uint16_t *i);
#endif /* APP_REG_H_ */
/** @} */
