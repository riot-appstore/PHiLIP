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
 * @file           : app_i2c.c
 * @author         : Kevin Weiss
 * @date           : 16.05.2018
 * @brief          : Application specific I2C functionality.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "app_access.h"
#include "app_errno.h"
#include "app_common.h"
#include "app_errno.h"
#include "app_typedef.h"
#include "app.h"
#include "app_debug.h"
#include "app_i2c.h"

/* Defines -------------------------------------------------------------------*/
#define CONVERT_7ADDR(x)	((x<<1)&0xFF)
#define CONVERT_10ADDR(x)	((x<<1)&0x07FF)

/* Private function prototypes -----------------------------------------------*/
static void _add_index(uint32_t *i);
static void _sub_index(uint32_t *i);
/* Private variables ---------------------------------------------------------*/
static uint32_t start_reg_index = 0;
static uint32_t reg_index = 0;
static bool set_index = false;
static I2C_HandleTypeDef *hi2c_inst = NULL;
static uint16_t clk_delay = 0;

/**
 * @brief
 *
 * @retval errno defined error code.
 */
error_t app_i2c_execute(i2c_t *i2c) {
	//TODO: Add range checks
	__HAL_I2C_DISABLE_IT(hi2c_inst, I2C_IT_EVT | I2C_IT_ERR);
	if (i2c->mode.addr_10_bit == 0) {
		hi2c_inst->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		hi2c_inst->Init.OwnAddress1 = CONVERT_7ADDR(i2c->slave_addr_1);
		hi2c_inst->Init.OwnAddress2 = CONVERT_7ADDR(i2c->slave_addr_2);
	} else {
		hi2c_inst->Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
		hi2c_inst->Init.OwnAddress1 = CONVERT_10ADDR(i2c->slave_addr_1);
		hi2c_inst->Init.OwnAddress2 = CONVERT_7ADDR(i2c->slave_addr_2);
	}

	if (i2c->mode.no_clk_stretch == 0) {
		hi2c_inst->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	} else {
		hi2c_inst->Init.NoStretchMode = I2C_NOSTRETCH_ENABLE;
	}

	if (i2c->mode.general_call == 0) {
		hi2c_inst->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	} else {
		hi2c_inst->Init.GeneralCallMode = I2C_GENERALCALL_ENABLE;
	}

	clk_delay = i2c->clk_stretch_delay;

	if (HAL_I2C_Init(hi2c_inst) != HAL_OK) {
		//must handle multiple repeats maybe?
		//_Error_Handler(__FILE__, __LINE__);
	}

	hi2c_inst->Instance->CR1 |= I2C_CR1_ACK;
	__HAL_I2C_ENABLE_IT(hi2c_inst, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
	return EOK;
}

/**
 * @brief
 *
 * @retval errno defined error code.
 */
error_t app_i2c_init(I2C_HandleTypeDef *hi2c) {
	reg_index = 0;
	start_reg_index = 0;
	hi2c_inst = hi2c;
	hi2c->Instance->CR1 |= I2C_CR1_ACK;
	__HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
	return EOK;
}

static HAL_StatusTypeDef I2C_Slave_ADDR(I2C_HandleTypeDef *hi2c) {
	/* Transfer Direction requested by Master */
	if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA) == RESET) {
		set_index = true;
	} else {
		reg_index = start_reg_index;
		if (clk_delay != 0) {
			delay_us(clk_delay);
		}
		uint8_t data;
		read_reg(reg_index, &data);
		hi2c->Instance->DR = data;
		_add_index(&reg_index);
	}

	return HAL_OK;
}

void i2c_err(I2C_HandleTypeDef* hi2c) {

	uint32_t sr1itflags = READ_REG(hi2c->Instance->SR1);
	uint32_t itsources = READ_REG(hi2c->Instance->CR2);

	if (((sr1itflags & I2C_FLAG_AF) != RESET)
			&& ((itsources & I2C_IT_ERR) != RESET)) {
		_sub_index(&reg_index);
	} else if ((itsources & I2C_IT_ERR) != RESET) {
		if (((sr1itflags & I2C_FLAG_BERR) != RESET)) {
			hi2c->ErrorCode |= HAL_I2C_ERROR_BERR;
			/* Clear BERR flag */
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_BERR);
		}

		/* I2C Arbitration Loss error interrupt occurred ---------------------------*/
		if ((sr1itflags & I2C_FLAG_ARLO) != RESET) {
			hi2c->ErrorCode |= HAL_I2C_ERROR_ARLO;
			/* Clear ARLO flag */
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ARLO);
		}

		/* I2C Over-Run/Under-Run interrupt occurred -------------------------------*/
		if ((sr1itflags & I2C_FLAG_OVR) != RESET) {
			hi2c->ErrorCode |= HAL_I2C_ERROR_OVR;
			/* Clear OVR flag */
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_OVR);
		}
#if 0
		/* Workaround: Start cannot be generated after a misplaced Stop */
		SET_BIT(hi2c->Instance->CR1, I2C_CR1_SWRST);
		if (HAL_I2C_Init(hi2c_inst) != HAL_OK) {}
		hi2c_inst->Instance->CR1 |= I2C_CR1_ACK;
		__HAL_I2C_ENABLE_IT(hi2c_inst, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
#endif
	}

	__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
	//hi2c->Instance->CR1 &= ~I2C_CR1_ACK; //should I?
}

void i2c_it(I2C_HandleTypeDef* hi2c) {
	uint32_t sr2itflags = READ_REG(hi2c->Instance->SR2);
	uint32_t sr1itflags = READ_REG(hi2c->Instance->SR1);
	uint32_t itsources = READ_REG(hi2c->Instance->CR2);

	if (((sr1itflags & I2C_FLAG_ADDR) != RESET)
			&& ((itsources & I2C_IT_EVT) != RESET)) {
		I2C_Slave_ADDR(hi2c);
	} else if (((sr1itflags & I2C_FLAG_STOPF) != RESET)
			&& ((itsources & I2C_IT_EVT) != RESET)) {
		__HAL_I2C_CLEAR_STOPFLAG(hi2c);
	} else if ((sr2itflags & I2C_FLAG_TRA) != RESET) {
		if (((sr1itflags & I2C_FLAG_TXE) != RESET)
				&& ((itsources & I2C_IT_BUF) != RESET)
				&& ((sr1itflags & I2C_FLAG_BTF) == RESET)) {
			uint8_t data;
			read_reg(reg_index, &data);
			hi2c->Instance->DR = data;
			_add_index(&reg_index);
		} else if (((sr1itflags & I2C_FLAG_BTF) != RESET)
				&& ((itsources & I2C_IT_EVT) != RESET)) {
		}
	} else if (((sr1itflags & I2C_FLAG_RXNE) != RESET)
			&& ((itsources & I2C_IT_BUF) != RESET)
			&& ((sr1itflags & I2C_FLAG_BTF) == RESET)) {
		if (set_index == true) {
			set_index = false;
			start_reg_index = hi2c->Instance->DR;
			reg_index = start_reg_index;
		} else {
			uint8_t data = hi2c->Instance->DR;
			write_reg(reg_index, data, PERIPH_ACCESS);
			_add_index(&reg_index);
		}
	} else if (((sr1itflags & I2C_FLAG_BTF) != RESET)
			&& ((itsources & I2C_IT_EVT) != RESET)) {
		if (set_index == true) {
			set_index = false;
			start_reg_index = hi2c->Instance->DR;
			reg_index = start_reg_index;
		} else {
			uint8_t data = hi2c->Instance->DR;
			write_reg(reg_index, data, PERIPH_ACCESS);
			_add_index(&reg_index);
		}
	}
}

static void _add_index(uint32_t *i) {
	(*i)++;
	if (*i == get_reg_size()) {
		*i = 0;
	}
}

static void _sub_index(uint32_t *i) {
	if (*i == 0) {
		*i = get_reg_size();
	}
	(*i)--;
}
