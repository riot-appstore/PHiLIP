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
 * @file			map_if.c
 * @author			Kevin Weiss
 * @date			05.03.2019
 * @brief			Handles memory map specific interface functions.
 * @details			Getting and setting of memory maps by name.
 * @}
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "PHiLIP_typedef.h"
#include "PHiLIP_map.h"
#include "app_errno.h"
#include "app_reg.h"
#include "map_if.h"

/* Private defines -----------------------------------------------------------*/
/** @brief Macro for getting the type size from the map */
#define T_SIZE(x) (PHILIP_TYPE_SIZE[MAP_T_TYPE[x]])

/* Functions -----------------------------------------------------------------*/
/**
 * @brief Gets parameters of a record in the memory map.
 *
 * @param[in]	index		index of the record to get
 * @param[out]	str			string to put the record information
 *
 * @return 		EOK on success
 * @return 		EOVERFLOW invalid record index
 */
error_t get_mm(uint16_t index, char* str) {
	if (index >= MAP_T_NUM_OF_RECORDS) {
		return EOVERFLOW;
	}
	sprintf(str,
			"0,{\"name\":\"%s\",\"offset\":%d,\"type\":\"%s\",\"\
array_size\":%d,bit_offset\":%d,\"bits\":%d}\n",
			MAP_T_NAME[index], MAP_T_OFFSET[index],
			PHILIP_TYPE_NAME[MAP_T_TYPE[index]], MAP_T_ARRAY_SIZE[index],
			MAP_T_BIT_OFFSET[index], MAP_T_BITS[index]);
	return EOK;
}

/**
 * @brief Gets the value of a record in the memory map
 *
 * @param[in]	mm_name		name of the record
 * @param[in]	array_index	index if the record is an array
 * @param[out]	data		data from the record
 *
 * @return 		EOK on success
 * @return 		EINVAL Invalid value
 * @return 		ERANGE invalid number range of array
 */
error_t get_mm_val(char* mm_name, uint32_t array_index, uint32_t *data) {
	int i;
	error_t err = EINVAL;
	for (i = 0; i < MAP_T_NUM_OF_RECORDS; i++) {
		if (strcmp(MAP_T_NAME[i], mm_name) == 0) {
			if (array_index) {
				if (array_index >= MAP_T_ARRAY_SIZE[i]) {
					return ERANGE;
				}
				return read_regs(MAP_T_OFFSET[i] + T_SIZE(i) * array_index,
						(uint8_t*) data, T_SIZE(i));
			} else if (MAP_T_BITS[i]) {
				err = read_regs(MAP_T_OFFSET[i], (uint8_t*) data, T_SIZE(i));
				if (err != EOK) {
					return err;
				}
				*data = (*data) >> MAP_T_BIT_OFFSET[i];
				*data = (*data) & ((1 << MAP_T_BITS[i]) - 1);
				return EOK;
			}
			return read_regs(MAP_T_OFFSET[i], (uint8_t*) data, T_SIZE(i));
		}
	}
	return err;
}

/**
 * @brief Sets the value of a record in the memory map
 *
 * @param[in]	mm_name		name of the record
 * @param[in]	array_index	index if the record is an array
 * @param[in]	data		data for the record
 * @param[in]	access		access level of the calling function
 *
 * @return 		EOK on success
 * @return 		EINVAL Invalid value
 * @return 		ERANGE invalid number range of array
 */
error_t set_mm_val(char* mm_name, uint32_t array_index, uint32_t data,
		uint8_t access) {
	int i;
	error_t err = EINVAL;
	for (i = 0; i < MAP_T_NUM_OF_RECORDS; i++) {
		if (strcmp(MAP_T_NAME[i], mm_name) == 0) {
			if (array_index) {
				if (array_index >= MAP_T_ARRAY_SIZE[i]) {
					return ERANGE;
				}
				return write_regs(MAP_T_OFFSET[i] + T_SIZE(i) * array_index,
						(uint8_t*) &data, T_SIZE(i), access);
			} else if (MAP_T_BITS[i]) {
				uint32_t tmp = 0;
				err = read_regs(MAP_T_OFFSET[i], (uint8_t*) &tmp, T_SIZE(i));
				if (err != EOK) {
					return err;
				}
				data &= ((1 << MAP_T_BITS[i]) - 1);
				data = data << MAP_T_BIT_OFFSET[i];
				tmp &= ~((((uint32_t) 1 << MAP_T_BITS[i]) - 1)
						<< MAP_T_BIT_OFFSET[i]);
				data |= tmp;

				return write_regs(MAP_T_OFFSET[i], (uint8_t*) &data, T_SIZE(i),
						access);
			}
			return write_regs(MAP_T_OFFSET[i], (uint8_t*) &data, T_SIZE(i),
					access);
		}
	}
	return err;
}
