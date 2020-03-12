/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
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

/* Includes *******************************************************************/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "PHiLIP_typedef.h"
#include "PHiLIP_map.h"
#include "app_reg.h"
#include "map_if.h"

/* Private macros *************************************************************/
/** @brief Macro for getting the type size from the map */
#define T_SIZE(x) (PHILIP_TYPE_SIZE[MAP_T_TYPE[x]])

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
error_t get_mm(uint16_t index, char* str) {
	if (index >= MAP_T_NUM_OF_RECORDS) {
		return EOVERFLOW;
	}
	sprintf(str,
			"{\"name\":\"%s\",\"offset\":%d,\"type\":\"%s\",\"\
array_size\":%d,\"bit_offset\":%d,\"bits\":%d,\"result\":0}\n",
			MAP_T_NAME[index], MAP_T_OFFSET[index],
			PHILIP_TYPE_NAME[MAP_T_TYPE[index]], MAP_T_ARRAY_SIZE[index],
			MAP_T_BIT_OFFSET[index], MAP_T_BITS[index]);
	return 0;
}

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
				if (err != 0) {
					return err;
				}
				*data = (*data) >> MAP_T_BIT_OFFSET[i];
				*data = (*data) & ((1 << MAP_T_BITS[i]) - 1);
				return 0;
			}
			return read_regs(MAP_T_OFFSET[i], (uint8_t*) data, T_SIZE(i));
		}
	}
	return err;
}

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
				if (err != 0) {
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
