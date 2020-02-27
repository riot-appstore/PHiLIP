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
 * @file			map_if.h
 * @author			Kevin Weiss
 * @date			05.03.2019
 * @brief			Handles memory map specific interface functions.
 * @details			Getting and setting of memory maps by name.
 ******************************************************************************
 */

#ifndef MAP_IF_H_
#define MAP_IF_H_

/* Function prototypes -------------------------------------------------------*/
/**
 * @brief Gets parameters of a record in the memory map.
 *
 * @param[in]	index		index of the record to get
 * @param[out]	str			string to put the record information
 *
 * @return 		EOK on success
 * @return 		EOVERFLOW invalid record index
 */
error_t get_mm(uint16_t index, char* str);

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
error_t get_mm_val(char* mm_name, uint32_t array_index, uint32_t *data);

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
		uint8_t access);

#endif /* MAP_IF_H_ */
/** @} */
