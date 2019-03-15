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
 * @addtogroup periph
 * @{
 * @file			i2c.c
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			Controls the i2c peripheral.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "port.h"
#include "app_errno.h"
#include "app_access.h"
#include "app_common.h"
#include "app_defaults.h"
#include "app_reg.h"

#include "i2c.h"

/* Private enums/structs -----------------------------------------------------*/
/** @brief  							The state settings of the I2C */
enum I2C_STATE {
	I2C_INITIALIZED, /**< i2c has been initialized */
	I2C_READING_DATA, /**< i2c currently reading data */
	I2C_WRITE_ADDRESS_RECEIVED, /**< i2c received write address */
	I2C_WRITE_1ST_REG_BYTE_RECEIVED, /**< i2c waiting for next addr byte */
	I2C_WRITING_DATA, /**< i2c currently writing data */
	I2C_ADDR_NACK, /**< i2c has nacked */
	I2C_STOPPED /**< i2c stopped successfully */
};

/** @brief					The parameters for i2c control */
typedef struct {
	I2C_HandleTypeDef hi2c; /**< Handle for the i2c device */
	i2c_t *reg; /**< i2c live application registers */
	i2c_t *saved_reg; /**< i2c saved application registers */
} i2c_dev;
/** @} */

/* Private defines -----------------------------------------------------------*/
/** @brief	convert 7 but i2c address */
#define CONVERT_7ADDR(x)		((x<<1)&0xFF)

/** @brief	convert 10 but i2c address */
#define CONVERT_10ADDR(x)		((x<<1)&0x07FF)

/** @brief	Check all i2c writing states */
#define IS_STATE_WRITING(x)	 	(x == I2C_WRITE_ADDRESS_RECEIVED || \
								x == I2C_WRITE_1ST_REG_BYTE_RECEIVED || \
								x == I2C_WRITING_DATA)

/* Private function prototypes -----------------------------------------------*/
static inline error_t _commit_i2c(i2c_dev *dev);
static inline void _i2c_slave_addr(i2c_dev *dev);
static void _i2c_it(i2c_dev *dev);
static void _i2c_err(i2c_dev *dev);

/* Global functions ----------------------------------------------------------*/
extern void _Error_Handler(char *, int);

/* Private variables ---------------------------------------------------------*/
static i2c_dev dut_i2c;

/* Functions -----------------------------------------------------------------*/
/**
 * @brief		Initializes i2c registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @param[in]	saved_reg	Pointer to saved register memory map
 * @note		Populates i2c defaults registers and assigns i2c register
 * 				pointers.
 */
void init_dut_i2c(map_t *reg, map_t *saved_reg) {

	dut_i2c.hi2c.Instance = DUT_I2C;
	dut_i2c.hi2c.Init.DualAddressMode = I2C_DUALADDRESS_ENABLE;
	dut_i2c.hi2c.Init.ClockSpeed = 400000;
	dut_i2c.hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;

	dut_i2c.reg = &(reg->i2c);
	dut_i2c.saved_reg = &(saved_reg->i2c);

	dut_i2c.reg->slave_addr_1 = DEFAULT_I2C_SLAVE_ID_1;
	dut_i2c.reg->slave_addr_2 = DEFAULT_I2C_SLAVE_ID_2;

	commit_dut_i2c();
}

/**
 * @brief		Commits the i2c registers and executes operations.
 *
 * @pre			i2c must first be initialized with init_dut_i2c()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the i2c.mode.init is set.
 */
error_t commit_dut_i2c() {
	return _commit_i2c(&dut_i2c);
}

static inline error_t _commit_i2c(i2c_dev *dev) {
	I2C_HandleTypeDef *hi2c_inst = &dev->hi2c;
	i2c_t *i2c = dev->reg;
	if (!i2c->mode.init) {
		__HAL_I2C_DISABLE_IT(hi2c_inst, I2C_IT_EVT | I2C_IT_ERR);

		if (dev->reg->mode.addr_10_bit == 0) {
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

		if (HAL_I2C_Init(hi2c_inst) != HAL_OK) {
			_Error_Handler(__FILE__, __LINE__);
		}

		hi2c_inst->Instance->CR1 |= I2C_CR1_ACK;
		__HAL_I2C_ENABLE_IT(hi2c_inst, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
		i2c->mode.init = 1;
		copy_until_same(dev->saved_reg, dev->reg, sizeof(*(dev->saved_reg)));
		return EOK;
	}
	return ENOACTION;
}

/* Interrupts ----------------------------------------------------------------*/
/**
 * @brief This function handles i2c_dut event interrupt.
 */
void DUT_I2C_EV_INT(void) {
	_i2c_it(&dut_i2c);
}

/**
 * @brief This function handles i2c_dut error interrupt.
 */
void DUT_I2C_ERR_INT(void) {
	_i2c_err(&dut_i2c);
}

static void _i2c_slave_addr(i2c_dev *dev) {
	I2C_HandleTypeDef *hi2c = &(dev->hi2c);
	i2c_t *i2c = dev->reg;
	/* Transfer Direction requested by Master */
	if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA) == RESET) {
		i2c->f_w_ticks = get_tick32();
		i2c->state = I2C_WRITE_ADDRESS_RECEIVED;
		i2c->w_count = 0;
	} else {
		i2c->reg_index = i2c->start_reg_index;
		if (i2c->clk_stretch_delay != 0) {
			delay_us(i2c->clk_stretch_delay);
		}
		unprotected_read_uint8(i2c->reg_index, (uint8_t*) &hi2c->Instance->DR);
		add_index(&i2c->reg_index);
		if (IS_STATE_WRITING(i2c->state)) {
			i2c->f_w_ticks = get_tick32() - i2c->s_ticks;
		}
		i2c->f_r_ticks = get_tick32();
		i2c->state = I2C_READING_DATA;
		i2c->r_count = 1;
	}
	i2c->s_ticks = get_tick32();
	if (READ_REG(hi2c->Instance->SR2) & I2C_FLAG_GENCALL) {
		i2c->status.gencall = 1;
	} else {
		i2c->status.gencall = 0;
	}
	if (i2c->status.busy) {
		i2c->status.rsr = 1;
	} else {
		i2c->status.rsr = 0;
	}
}

static void _i2c_it(i2c_dev *dev) {
	I2C_HandleTypeDef *hi2c = &dev->hi2c;
	i2c_t *i2c = dev->reg;
	//i2c_t *saved_i2c = dev->saved_reg;
	uint32_t sr2itflags = READ_REG(hi2c->Instance->SR2);
	uint32_t sr1itflags = READ_REG(hi2c->Instance->SR1);
	uint32_t itsources = READ_REG(hi2c->Instance->CR2);

	if (((sr1itflags & I2C_FLAG_ADDR) != RESET)
			&& ((itsources & I2C_IT_EVT) != RESET)) {
		_i2c_slave_addr(dev);
	} else if (((sr1itflags & I2C_FLAG_STOPF) != RESET)
			&& ((itsources & I2C_IT_EVT) != RESET)) {
		__HAL_I2C_CLEAR_STOPFLAG(hi2c);
		if(i2c->state == I2C_READING_DATA) {
			i2c->f_r_ticks = get_tick32() - i2c->s_ticks;
		}
		else if (IS_STATE_WRITING(i2c->state)){
			i2c->f_w_ticks = get_tick32() - i2c->s_ticks;
		}
		i2c->state = I2C_STOPPED;
	} else if ((sr2itflags & I2C_FLAG_TRA) != RESET) {
		if (((sr1itflags & I2C_FLAG_TXE) != RESET)
				&& ((itsources & I2C_IT_BUF) != RESET)
				&& ((sr1itflags & I2C_FLAG_BTF) == RESET)) {
			unprotected_read_uint8(i2c->reg_index, (uint8_t*) &hi2c->Instance->DR);
			add_index(&(i2c->reg_index));
			i2c->r_count++;
			i2c->r_ticks = get_tick32() - i2c->f_r_ticks ;
			i2c->f_r_ticks = get_tick32();
		} else if (((sr1itflags & I2C_FLAG_BTF) != RESET)
				&& ((itsources & I2C_IT_EVT) != RESET)) {
		}
	} else if (((((sr1itflags & I2C_FLAG_RXNE) != RESET)
			&& ((itsources & I2C_IT_BUF) != RESET)
			&& ((sr1itflags & I2C_FLAG_BTF) == RESET)))
			|| (((sr1itflags & I2C_FLAG_BTF) != RESET)
					&& ((itsources & I2C_IT_EVT) != RESET))) {
		i2c->w_ticks = get_tick32() - i2c->f_w_ticks ;
		i2c->f_w_ticks = get_tick32();
		if (i2c->state == I2C_WRITE_ADDRESS_RECEIVED) {
			if (i2c->mode.reg_16_bit) {
				i2c->state = I2C_WRITE_1ST_REG_BYTE_RECEIVED;
				if (i2c->mode.reg_16_big_endian) {
					i2c->start_reg_index = hi2c->Instance->DR;
				} else {
					i2c->start_reg_index = (hi2c->Instance->DR << 8);
				}
			} else {
				i2c->start_reg_index = hi2c->Instance->DR;
				i2c->reg_index = i2c->start_reg_index;
				i2c->state = I2C_WRITING_DATA;
			}
		} else {
			write_reg(i2c->reg_index, hi2c->Instance->DR, PERIPH_ACCESS);
			i2c->w_count++;
			add_index(&(i2c->reg_index));
		}
	}
	if (READ_REG(hi2c->Instance->SR2) & I2C_FLAG_BUSY) {
		i2c->status.busy = 1;
	} else {
		i2c->status.busy = 0;
	}
}

static void _i2c_err(i2c_dev *dev) {
	I2C_HandleTypeDef *hi2c = &dev->hi2c;
	i2c_t *i2c = dev->reg;
	//i2c_t *saved_i2c = dev->saved_reg;

	uint32_t sr1itflags = READ_REG(hi2c->Instance->SR1);
	uint32_t itsources = READ_REG(hi2c->Instance->CR2);

	if(i2c->state == I2C_READING_DATA) {
		i2c->f_r_ticks = get_tick32() - i2c->s_ticks;
	}
	else if (IS_STATE_WRITING(i2c->state)){
		i2c->f_w_ticks = get_tick32() - i2c->s_ticks;
	}
	if (((sr1itflags & I2C_FLAG_AF) != RESET)
			&& ((itsources & I2C_IT_ERR) != RESET)) {
		sub_index(&i2c->reg_index);
		i2c->status.af = 1;
		i2c->state = I2C_ADDR_NACK;
	} else if ((itsources & I2C_IT_ERR) != RESET) {
		if (((sr1itflags & I2C_FLAG_BERR) != RESET)) {
			i2c->status.berr = 1;
			/* Clear BERR flag */
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_BERR);
		}
		/* I2C Arbitration Loss error interrupt occurred ---------------------------*/
		if ((sr1itflags & I2C_FLAG_ARLO) != RESET) {
			/* Clear ARLO flag */
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ARLO);
		}

		/* I2C Over-Run/Under-Run interrupt occurred -------------------------------*/
		if ((sr1itflags & I2C_FLAG_OVR) != RESET) {
			i2c->status.ovr = 1;
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
		i2c->state = I2C_STOPPED;
	}

	__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);

	if (READ_REG(hi2c->Instance->SR2) & I2C_FLAG_BUSY) {
		i2c->status.busy = 1;
	} else {
		i2c->status.busy = 0;
	}
}
