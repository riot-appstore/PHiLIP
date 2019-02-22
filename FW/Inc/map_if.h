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
