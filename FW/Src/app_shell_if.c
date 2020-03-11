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

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "PHiLIP_map.h"
#include "app_access.h"
#include "app_errno.h"
#include "app_common.h"
#include "app_reg.h"
#include "map_if.h"

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

/** @brief   Prints the version of the interface */
#define VERSION_CMD		"-v\n"

/** @brief   Prints the version of the interface */
#define VERSION_CMD2	"version\n"

/** @brief   Prints the version of the interface */
#define VERSION_CMD3	"ver\n"

/** @brief   Prints the version of the interface */
#define VERSION_CMD4	"--v\n"

/** @brief   Prints a help menu */
#define HELP_CMD		"help\n"

/** @brief   Gives properties of the memory map for a given index */
#define MEMORY_MAP_CMD	"mm "

/** @brief   Gives the amount of records in the memory map */
#define MM_SIZE_CMD		"mm_size\n"

/** @brief   Reads values given a record name */
#define READ_KEY_CMD	"r "

/** @brief   Writes a value to a record name */
#define WRITE_KEY_CMD	"w "
/** @} */

/** @brief   Maximum characters for parsing strings to numbers */
#define ATOU_MAX_CHAR	10

/** @brief   Error code for parsing strings to numbers */
#define ATOU_ERROR		0xFFFFFFFF

/** @brief   Macro for max values of a byte */
#define BYTE_MAX		((uint8_t)0xFF)

/** @brief   Is a ascii number */
#define IS_NUM(x)		(x >= '0' && x <= '9')

/** @brief   Macro to check if command string matches */
#define IS_COMMAND(x)	(memcmp(str, x, strlen(x)) == 0)

/* Private function prototypes -----------------------------------------------*/
static error_t _cmd_read_reg(char *str, uint16_t buf_size);
static error_t _cmd_write_reg(char *str, uint16_t buf_size, uint8_t access);
static error_t _cmd_read_key(char *str, uint16_t buf_size);
static error_t _cmd_write_key(char *str, uint16_t buf_size, uint8_t access);
static error_t _cmd_execute(char *str);
static error_t _cmd_reset();
static error_t _cmd_mm_print(char *str, uint16_t buf_size);
static error_t _cmd_print_help(char *str);
static error_t _cmd_print_version(char *str);

static error_t _valid_args(char *str, uint32_t *arg_count, uint16_t buf_size);
static uint32_t _fast_atou(char **str, char terminator);

static void _json_result(char *str, error_t result);

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

	if (str[buf_size - 1] != 0) {
		_json_result(str, EMSGSIZE);
	}

	if (IS_COMMAND(READ_REG_CMD)) {
		err = _cmd_read_reg(str, buf_size);
	} else if (IS_COMMAND(WRITE_REG_CMD)) {
		err = _cmd_write_reg(str, buf_size, access);
	} else if (IS_COMMAND(READ_KEY_CMD)) {
		err = _cmd_read_key(str, buf_size);
	} else if (IS_COMMAND(WRITE_KEY_CMD)) {
		err = _cmd_write_key(str, buf_size, access);
	} else if (IS_COMMAND(MEMORY_MAP_CMD)) {
		err = _cmd_mm_print(str, buf_size);
	} else if (IS_COMMAND(MM_SIZE_CMD)) {
		sprintf(str, "{\"data\":%u,\"result\":0}\n", MAP_T_NUM_OF_RECORDS);
		err = EOK;
	} else if (IS_COMMAND(HELP_CMD)) {
		err = _cmd_print_help(str);
	} else if (IS_COMMAND(VERSION_CMD) || IS_COMMAND(VERSION_CMD2) ||
	IS_COMMAND(VERSION_CMD3) || IS_COMMAND(VERSION_CMD4)) {
		err = _cmd_print_version(str);
	} else if (IS_COMMAND(EXECUTE_CMD)) {
		if (!(access & IF_ACCESS)) {
			err = EACCES;
		} else {
			err = _cmd_execute(str);
		}
	} else if (IS_COMMAND(RESET_CMD)) {
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
		_json_result(str, err);
	}

	return err;
}

static error_t _cmd_read_reg(char *str, uint16_t buf_size) {
	char *first_str = str;
	char *arg_str = str + strlen(READ_REG_CMD);
	uint32_t index = _fast_atou(&arg_str, ' ');

	if (index == ATOU_ERROR) {
		return EINVAL;
	} else {
		uint32_t size = _fast_atou(&arg_str, RX_END_CHAR);
		if (size == ATOU_ERROR) {
			return EINVAL;
		} else if ((size * 2) + strlen(TX_END_STR) + strlen("0,0x")
				>= buf_size) {
			return ERANGE;
		} else if (index + size > get_reg_size()) {
			return EOVERFLOW;
		} else {
			uint8_t data;
			str += sprintf(str, "{\"data\":[");
			while (size--) {
				/* TODO: Make this a bit better, use helper functions */
				read_reg(index++, &data);

				if ((str - first_str) + 4 + strlen("], \"result\":0}\n")
						< buf_size) {
					str += sprintf(str, "%u", data);
					if (size) {
						str += sprintf(str, ",");
					}
				} else {
					return ERANGE;
				}
			}
			sprintf(str, "], \"result\":0}\n");
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
					_json_result(str, EOK);
				}
			}
		}
	}
	return err;
}

static error_t _cmd_read_key(char *str, uint16_t buf_size) {
	char *arg_str = str + strlen(READ_KEY_CMD);
	uint32_t array_index = 0;
	uint32_t data = 0;
	error_t err = EUNKNOWN;
	for (; *arg_str != 0 && *arg_str != ' ' && *arg_str != RX_END_CHAR;
			arg_str++)
		;
	if (*arg_str == ' ') {
		*arg_str = 0;
		arg_str++;
		array_index = _fast_atou(&(arg_str), RX_END_CHAR);
		if (array_index == ATOU_ERROR) {
			return EINVAL;
		}
	}
	*arg_str = 0;
	err = get_mm_val(str + strlen(READ_KEY_CMD), array_index, &data);
	/* If error the print will be overwritten */
	sprintf(str, "{\"data\":%lu,\"hex_data\":\"0x%lX\",\"result\":0}\n", data,
			data);
	return err;
}

static error_t _cmd_write_key(char *str, uint16_t buf_size, uint8_t access) {
	char *arg_str = str + strlen(WRITE_KEY_CMD);
	uint32_t array_index = 0;
	uint32_t data = 0;
	error_t err = EUNKNOWN;

	for (; *arg_str != 0 && *arg_str != ' ' && *arg_str != RX_END_CHAR;
			arg_str++)
		;
	if (*arg_str != ' ') {
		return EINVAL;
	}
	*arg_str = 0;
	arg_str++;
	char *end_check_str = arg_str;
	array_index = _fast_atou(&end_check_str, ' ');
	if (array_index == ATOU_ERROR) {
		array_index = 0;
	} else {
		arg_str = end_check_str;
	}
	data = _fast_atou(&arg_str, '\n');
	if (data == ATOU_ERROR) {
		return EINVAL;
	}

	err = set_mm_val(str + strlen(WRITE_KEY_CMD), array_index, data, access);
	/* If error the print will be overwritten */
	_json_result(str, EOK);
	return err;
}

static error_t _cmd_mm_print(char *str, uint16_t buf_size) {
	char *arg_str = str + strlen(MEMORY_MAP_CMD);
	uint32_t index = _fast_atou(&arg_str, '\n');
	return get_mm(index, str);

}

static error_t _cmd_execute(char *str) {
	error_t err = execute_reg_change();
	if (err == EOK) {
		_json_result(str, EOK);
	}
	return err;
}

static error_t _cmd_print_version(char *str) {
	sprintf(str, "{\"version\":\"%u.%u.%u\",\"result\":0}\n", IF_VERSION_MAJOR, IF_VERSION_MINOR,
			IF_VERSION_PATCH);

	return EOK;
}

static error_t _cmd_print_help(char *str) {
	sprintf(str,
			"rr <reg_offset> <size> : Reads bytes\n\
wr <reg_offest> <DATA0> [DATA1] ... [DATAn] : Writes bytes\n\
r <reg_key> [array_index]: Reads register\n\
w <reg_key> [array_index] <DATA> : Writes register\n\
ex : Executes config changes\n\
mcu_rst : Soft reset\n\
mm <index> : Prints mem map record \n\
mm_size : Amount of records in memory map\n\
version : Interface version\n");
	return EOK;
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

static void _json_result(char *str, error_t result) {
	sprintf(str, "{\"result\":%d}\n", result);
}
