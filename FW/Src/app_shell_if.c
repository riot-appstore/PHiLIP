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
 * @file			app_shell_if.c
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			Protocol for application communication.
 * @details			This initializes and runs the serial communication
 * 					protocol for interfacing to registers.  It used the STM HAL
 * 					and a UART for the IO.  It also uses DMA.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "PHiLIP_typedef.h"
#include "app_access.h"
#include "app_errno.h"
#include "app_common.h"
#include "app_reg.h"

#include "app_shell_if.h"

/* Private defines -----------------------------------------------------------*/
/** @brief   The command to read application registers */
#define READ_REG_CMD	"rr "

/** @brief   The command to write application registers */
#define WRITE_REG_CMD	"wr "

/** @brief   The command to execute and commit changes in the registers */
#define EXECUTE_CMD		"ex\n"

/** @brief   The command to provide a software reset to the applications */
#define RESET_CMD		"mcu_rst\n"
/** @} */

/** @brief   Maximum characters for parsing strings to numbers */
#define ATOU_MAX_CHAR	5

/** @brief   Error code for parsing strings to numbers */
#define ATOU_ERROR		0xFFFFFFFF

/** @brief   Macro for max values of a byte */
#define BYTE_MAX		((uint8_t)0xFF)

/** @brief   Is a ascii number */
#define IS_NUM(x)			(x >= '0' && x <= '9')

/* Private function prototypes -----------------------------------------------*/
static error_t _cmd_read_reg(char *str, uint16_t buf_size);
static error_t _cmd_write_reg(char *str, uint16_t buf_size, uint8_t access);
static error_t _cmd_execute(char *str);
static error_t _cmd_reset();

static error_t _valid_args(char *str, uint32_t *arg_count, uint16_t buf_size);
static uint32_t _fast_atou(char **str, char terminator);

/* Functions -----------------------------------------------------------------*/
/**
 * @brief Parses a string and executes commands.
 *
 * @param[in]	str			String with the command
 * @param[in]	buf_size	The max size of the string buffer
 * @param[in]	access		The callers access level
 *
 * @return 		EOK on success
 * @return 		EPROTONOSUPPORT command not supported
 * @return 		EACCES caller doesn't have access
 * @return 		EMSGSIZE message size too big
 * @return 		EINVAL Invalid value
 * @return 		EOVERFLOW invalid address
 * @return 		ERANGE invalid number range
 * @return 		ENODATA not enough data
 * @return 		EUNKNOWN
 *
 * @warning		May protect interrupts and cause jitter.
 */
error_t parse_command(char *str, uint16_t buf_size, uint8_t access) {
	error_t err = EPROTONOSUPPORT;

	if (memcmp(str, READ_REG_CMD, strlen(READ_REG_CMD)) == 0) {
		err = _cmd_read_reg(str, buf_size);
	} else if (memcmp(str, WRITE_REG_CMD, strlen(WRITE_REG_CMD)) == 0) {
		err = _cmd_write_reg(str, buf_size, access);
	} else if (memcmp(str, EXECUTE_CMD, strlen(EXECUTE_CMD)) == 0) {
		if (!(access & IF_ACCESS)) {
			err = EACCES;
		} else {
			err = _cmd_execute(str);
		}
	} else if (memcmp(str, RESET_CMD, strlen(RESET_CMD)) == 0) {
		if (!(access & IF_ACCESS)) {
			err = EACCES;
		} else {
			err = _cmd_reset();
		}
	}
	if (str[buf_size - 1] != 0) {
		err = EMSGSIZE;
	}

	if (err != EOK) {
		sprintf(str, "%d%s", err, TX_END_STR);
	}

	return err;
}

static error_t _cmd_read_reg(char *str, uint16_t buf_size) {
	char *first_str = str;
	char *arg_str = str + strlen(READ_REG_CMD);
	uint32_t index = _fast_atou(&arg_str, ' ');

	if (index == ATOU_ERROR) {
		return EINVAL;
	} else if (index >= get_reg_size()) {
		return EOVERFLOW;
	} else {
		uint32_t size = _fast_atou(&arg_str, RX_END_CHAR);
		if (size == ATOU_ERROR) {
			return EINVAL;
		} else if ((size * 2) + strlen(TX_END_STR) + strlen("0,0x")
				>= buf_size) {
			return ERANGE;
		} else {
			uint8_t data;
			str += sprintf(str, "%d,0x", EOK);
			index += size;
			if (index > get_reg_size()) {
				index -= get_reg_size();
			}
			while (size > 0) {
				index--;
				size--;
				/* TODO: Make this a bit better, use helper functions */
				read_reg(index, &data);

				if (index == 0) {
					index = get_reg_size();
				}
				if ((str - first_str) + 2 + strlen(TX_END_STR) < buf_size) {
					str += sprintf(str, "%02X", data);
				} else {
					return ERANGE;
				}
			}
			sprintf(str, TX_END_STR);
			return EOK;
		}
	}
	return EUNKNOWN;
}

static error_t _cmd_write_reg(char *str, uint16_t buf_size, uint8_t access) {
	uint32_t arg_count = 0;
	error_t err;

	err = _valid_args(str, &arg_count, buf_size);
	if (err == EOK) {
		if (arg_count < 2) {
			err = ENODATA;
		} else {
			char *arg_str = str + strlen(WRITE_REG_CMD);
			uint32_t val;
			uint32_t index = _fast_atou(&arg_str, ' ');
			if (index >= get_reg_size()) {
				err = EOVERFLOW;
			} else {
				arg_count--;
				while (arg_count != 1) {
					val = _fast_atou(&arg_str, ' ');
					DIS_INT;
					err = write_reg(index, val, access);
					EN_INT;
					if (err != EOK) {
						return err;
					}
					index++;
					if (index == get_reg_size()) {
						index = 0;
					}
					arg_count--;
				}
				val = _fast_atou(&arg_str, RX_END_CHAR);
				DIS_INT;
				err = write_reg(index, val, access);
				EN_INT;
				if (err == EOK) {
					sprintf(str, "%d%s", EOK, TX_END_STR);
				}
			}
		}
	}
	return err;
}

static error_t _cmd_execute(char *str) {
	error_t err = execute_reg_change();
	if (err == EOK) {
		sprintf(str, "%d%s", EOK, TX_END_STR);
	}
	return err;
}

static error_t _valid_args(char *str, uint32_t *arg_count, uint16_t buf_size) {
	char *arg_str = str + strlen(WRITE_REG_CMD);
	uint32_t val;

	*arg_count = 0;
	while ((arg_str - str) < buf_size - strlen(WRITE_REG_CMD)) {
		char *end_check_str = arg_str;
		val = _fast_atou(&end_check_str, ' ');
		if (val == ATOU_ERROR) {
			val = _fast_atou(&arg_str, RX_END_CHAR);
			if (val == ATOU_ERROR) {
				return EINVAL;
			} else if (val > get_reg_size()) {
				return EOVERFLOW;
			} else {
				(*arg_count)++;
				return EOK;
			}
		} else if (val > get_reg_size()) {
			return EOVERFLOW;
		} else {
			arg_str = end_check_str;
			(*arg_count)++;
		}
	}
	return EMSGSIZE;
}

static error_t _cmd_reset() {
	SOFT_RESET;
	return EUNKNOWN;
}

uint32_t _fast_atou(char **str, char terminator) {
	uint32_t val = 0;
	char *first_str = *str;

	if (**str == terminator) {
		return ATOU_ERROR;
	}

	if ((*str)[0] == '0' && (*str)[1] == 'x') {
		*str += 2;
		while (**str != terminator) {
			if ((uint32_t) (*str - first_str) >= ATOU_MAX_CHAR) {
				return ATOU_ERROR;
			} else if (IS_NUM(**str)) {
				val = (val << 4) + (**str - '0');
				(*str)++;
			} else if (**str >= 'a' && **str <= 'f') {
				val = (val << 4) + (**str - 'a' + 10);
				(*str)++;
			} else if (**str >= 'A' && **str <= 'F') {
				val = (val << 4) + (**str - 'A' + 10);
				(*str)++;
			} else {
				return ATOU_ERROR;
			}
		}
	} else {
		while (**str != terminator) {
			if ((uint32_t) (*str - first_str) >= ATOU_MAX_CHAR) {
				return ATOU_ERROR;
			} else if (!IS_NUM(**str)) {
				return ATOU_ERROR;
			} else {
				val = val * 10 + (**str - '0');
				(*str)++;
			}
		}
	}
	(*str)++;
	return val;
}
