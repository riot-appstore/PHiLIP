/*
 * MIT License
 *
 * Copyright 2018 Kevin Weiss
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

/*
 ******************************************************************************
 * @file           : serial_com.c
 * @author         : Kevin Weiss
 * @date           : 16.05.2018
 * @brief          : Serial communication handling.
 ******************************************************************************
 *
 * This initializes and runs the serial communication protocol for interfacing
 * to registers.  It used the STM HAL and a UART for the IO.  It also uses DMA.
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>



#include "app_access.h"
#include "app_errno.h"
#include "app_common.h"
#include "app.h"
#include "app_shell_if.h"

/* Defines -------------------------------------------------------------------*/

#define READ_REG_CMD	"rr "
#define WRITE_REG_CMD	"wr "
#define EXECUTE_CMD		"ex\n"
#define RESET_CMD		"mcu_rst\n"

#define ATOU_MAX_CHAR	5
#define ATOU_ERROR		0xFFFFFFFF
#define BYTE_MAX		((uint8_t)0xFF)


#define IS_NUM(x)			(x >= '0' && x <= '9')

/* Private function prototypes -----------------------------------------------*/
static error_t parse_command(char *str, uint16_t buf_size, uint8_t access);
static error_t _cmd_read_reg(char *str, uint16_t buf_size);
static error_t _cmd_write_reg(char *str, uint16_t buf_size, uint8_t access);
static error_t _cmd_execute(char *str);
static error_t _cmd_reset();

static error_t _valid_args(char *str, uint32_t *arg_count, uint16_t buf_size);
static uint32_t _fast_atou(char **str, char terminator);

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
error_t parse_input(uint8_t mode, char *str, uint16_t buf_size, uint8_t access) {
	int i;
	switch(mode) {
	case MODE_ECHO:
		return EOK;
	case MODE_ECHO_EXT:
		for (i = 0; i < strlen(str) - 1; i++) {
			str[i]++;
		}
		return EOK;
	case MODE_REG:
		return parse_command(str, buf_size, access);
	}
	return EPROTONOSUPPORT;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t parse_command(char *str, uint16_t buf_size, uint8_t access) {
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
	if (str[buf_size - 1] != 0){
		err = EMSGSIZE;
	}

	if (err != EOK) {
		sprintf(str, "%d%s", err, TX_END_STR);
	}

	return err;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
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
		} else if ((size * 2) + strlen(TX_END_STR)
				+ strlen("0,0x") >= buf_size) {
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

				DIS_INT;
				read_reg(index, &data);
				EN_INT;

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

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
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

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
static error_t _cmd_execute(char *str) {
	error_t err = execute_reg_change();
	if (err == EOK) {
		sprintf(str, "%d%s", EOK, TX_END_STR);
	}
	return err;
}

/**
 * @brief Private function
 *
 * @retval errno defined error code.
 */
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
			} else if (val > BYTE_MAX) {
				return EOVERFLOW;
			} else {
				(*arg_count)++;
				return EOK;
			}
		} else if (val > BYTE_MAX) {
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

/**
 * @brief Private function, only handles controlled inputs.
 *
 * @retval numerical value from the string.
 */
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
