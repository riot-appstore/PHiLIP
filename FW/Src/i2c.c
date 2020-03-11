/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/**
 ******************************************************************************
 * @addtogroup periph
 * @{
 * @file			i2c.c
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			Controls the reg peripheral.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include <errno.h>

#include "stm32f1xx_hal.h"

#include "PHiLIP_typedef.h"
#include "port.h"
#include "app_access.h"
#include "app_common.h"
#include "app_defaults.h"
#include "app_reg.h"

#include "gpio.h"

#include "i2c.h"

/* Private enums/structs ******************************************************/
/** @brief  							The state settings of the I2C */
enum I2C_STATE {
	I2C_INITIALIZED, /**< reg has been initialized */
	I2C_READING_DATA, /**< reg currently reading data */
	I2C_WRITE_ADDRESS_RECEIVED, /**< reg received write address */
	I2C_WRITE_1ST_REG_BYTE_RECEIVED, /**< reg waiting for next addr byte */
	I2C_WRITING_DATA, /**< reg currently writing data */
	I2C_ADDR_NACK, /**< reg has nacked */
	I2C_STOPPED /**< reg stopped successfully */
};

/** @brief					The parameters for reg control */
typedef struct {
	I2C_HandleTypeDef hi2c; /**< Handle for the reg device */
	i2c_t *reg; /**< reg live application registers */
	i2c_mode_t mode; /**< Committed mode */
	uint16_t clk_stretch_delay; /**< Committed clock stretch delay in us */
} i2c_dev;
/** @} */

/* Private macros *************************************************************/
/** @brief	convert 7 but reg address */
#define CONVERT_7ADDR(x)		((x<<1)&0xFF)

/** @brief	convert 10 but reg address */
#define CONVERT_10ADDR(x)		((x<<1)&0x07FF)

/** @brief	Check all reg writing states */
#define IS_STATE_WRITING(x)	 	(x == I2C_WRITE_ADDRESS_RECEIVED || \
								x == I2C_WRITE_1ST_REG_BYTE_RECEIVED || \
								x == I2C_WRITING_DATA)

/* Private function prototypes ************************************************/
static inline void _i2c_slave_addr(i2c_dev *dev);
static void _i2c_it(i2c_dev *dev);
static void _i2c_err(i2c_dev *dev);

/* Private variables **********************************************************/
static i2c_dev dut_i2c;

/******************************************************************************/
/*           Initialization DUT_I2C                                           */
/******************************************************************************/
void init_dut_i2c(map_t *reg) {
	dut_i2c.reg = &(reg->i2c);

	dut_i2c.hi2c.Instance = DUT_I2C_INST;
	dut_i2c.hi2c.Init.DualAddressMode = I2C_DUALADDRESS_ENABLE;
	dut_i2c.hi2c.Init.ClockSpeed = 400000; /* Not needed because its slave */
	dut_i2c.hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;

	commit_dut_i2c();
}

/******************************************************************************/
void init_dut_i2c_msp() {
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	DUT_I2C_GPIO_AF_REMAP();

	GPIO_InitStruct.Pin = DUT_SCL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(DUT_SCL_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = DUT_SDA_Pin;
	HAL_GPIO_Init(DUT_SDA_GPIO_Port, &GPIO_InitStruct);

	DUT_I2C_CLK_EN();

	HAL_NVIC_SetPriority(DUT_I2C_EV_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(DUT_I2C_EV_IRQ);
	HAL_NVIC_SetPriority(DUT_I2C_ERR_IRQ, DEFAULT_INT_PRIO, 0);
	HAL_NVIC_EnableIRQ(DUT_I2C_ERR_IRQ);
}

void deinit_dut_i2c_msp() {
	HAL_GPIO_DeInit(DUT_SCL_GPIO_Port, DUT_SCL_Pin);
	HAL_GPIO_DeInit(DUT_SDA_GPIO_Port, DUT_SDA_Pin);

	HAL_NVIC_DisableIRQ(DUT_I2C_EV_IRQ);
	HAL_NVIC_DisableIRQ(DUT_I2C_ERR_IRQ);
}

/******************************************************************************/
error_t commit_dut_i2c() {
	I2C_HandleTypeDef *hi2c = &dut_i2c.hi2c;
	i2c_t *reg = dut_i2c.reg;

	if (reg->mode.init) {
		return 0;
	}
	__HAL_I2C_DISABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_ERR);

	if (reg->mode.addr_10_bit == 0) {
		hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		hi2c->Init.OwnAddress1 = CONVERT_7ADDR(reg->slave_addr_1);
		hi2c->Init.OwnAddress2 = CONVERT_7ADDR(reg->slave_addr_2);
	} else {
		hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_10BIT;
		hi2c->Init.OwnAddress1 = CONVERT_10ADDR(reg->slave_addr_1);
		hi2c->Init.OwnAddress2 = CONVERT_7ADDR(reg->slave_addr_2);
	}

	if (reg->mode.no_clk_stretch == 0) {
		hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	} else {
		hi2c->Init.NoStretchMode = I2C_NOSTRETCH_ENABLE;
	}

	if (reg->mode.general_call == 0) {
		hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	} else {
		hi2c->Init.GeneralCallMode = I2C_GENERALCALL_ENABLE;
	}

	if (reg->mode.disable) {
		HAL_I2C_DeInit(hi2c);
		if (init_basic_gpio(reg->dut_sda, DUT_SDA) != 0) {
			return EINVAL;
		}
		if (init_basic_gpio(reg->dut_scl, DUT_SCL) != 0) {
			return EINVAL;
		}
	}
	else {
		HAL_I2C_Init(hi2c);
	}

	hi2c->Instance->CR1 |= I2C_CR1_ACK;
	__HAL_I2C_ENABLE_IT(hi2c, I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR);
	reg->mode.init = 1;
	dut_i2c.mode.nack_data = reg->mode.nack_data;
	dut_i2c.mode.reg_16_big_endian = reg->mode.reg_16_big_endian;
	dut_i2c.mode.reg_16_bit = reg->mode.reg_16_bit;
	dut_i2c.clk_stretch_delay = reg->clk_stretch_delay;
	return 0;
}

void update_dut_i2c_inputs() {
	dut_i2c.reg->dut_sda.level = HAL_GPIO_ReadPin(DUT_SDA);
	dut_i2c.reg->dut_scl.level = HAL_GPIO_ReadPin(DUT_SCL);
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
	i2c_t *reg = dev->reg;
	/* Transfer Direction requested by Master */
	if (__HAL_I2C_GET_FLAG(hi2c, I2C_FLAG_TRA) == RESET) {
		reg->f_w_ticks = get_tick32();
		reg->state = I2C_WRITE_ADDRESS_RECEIVED;
		reg->w_count = 0;
		if (dev->mode.nack_data) {
			hi2c->Instance->CR1 &= ~I2C_CR1_ACK;
		}
	} else {
		reg->reg_index = reg->start_reg_index;
		if (dev->clk_stretch_delay != 0) {
			delay_us(reg->clk_stretch_delay);
		}
		unprotected_read_uint8(reg->reg_index, (uint8_t*) &hi2c->Instance->DR);
		add_index(&reg->reg_index);
		if (IS_STATE_WRITING(reg->state)) {
			reg->f_w_ticks = get_tick32() - reg->s_ticks;
		}
		reg->f_r_ticks = get_tick32();
		reg->state = I2C_READING_DATA;
		reg->r_count = 0;
	}
	reg->s_ticks = get_tick32();
	if (READ_REG(hi2c->Instance->SR2) & I2C_FLAG_GENCALL) {
		reg->status.gencall = 1;
	} else {
		reg->status.gencall = 0;
	}
	if (reg->status.busy) {
		reg->status.rsr = 1;
	} else {
		reg->status.rsr = 0;
	}
}

static void _i2c_it(i2c_dev *dev) {
	I2C_HandleTypeDef *hi2c = &dev->hi2c;
	i2c_t *reg = dev->reg;
	uint32_t sr2itflags = READ_REG(hi2c->Instance->SR2);
	uint32_t sr1itflags = READ_REG(hi2c->Instance->SR1);
	uint32_t itsources = READ_REG(hi2c->Instance->CR2);
	hi2c->Instance->CR1 |= I2C_CR1_ACK;
	if (((sr1itflags & I2C_FLAG_ADDR) != RESET)
			&& ((itsources & I2C_IT_EVT) != RESET)) {
		_i2c_slave_addr(dev);
	} else if (((sr1itflags & I2C_FLAG_STOPF) != RESET)
			&& ((itsources & I2C_IT_EVT) != RESET)) {
		__HAL_I2C_CLEAR_STOPFLAG(hi2c);
		if (IS_STATE_WRITING(reg->state)){
			reg->f_w_ticks = get_tick32() - reg->s_ticks;
		}
		else if(reg->state == I2C_READING_DATA) {
			reg->f_r_ticks = get_tick32() - reg->s_ticks;
		}
		reg->state = I2C_STOPPED;
	} else if ((sr2itflags & I2C_FLAG_TRA) != RESET) {
		if (((sr1itflags & I2C_FLAG_TXE) != RESET)
				&& ((itsources & I2C_IT_BUF) != RESET)
				&& ((sr1itflags & I2C_FLAG_BTF) == RESET)) {
			unprotected_read_uint8(reg->reg_index, (uint8_t*) &hi2c->Instance->DR);
			add_index(&(reg->reg_index));
			reg->r_count++;
			reg->r_ticks = get_tick32() - reg->f_r_ticks ;
			reg->f_r_ticks = get_tick32();
		} else if (((sr1itflags & I2C_FLAG_BTF) != RESET)
				&& ((itsources & I2C_IT_EVT) != RESET)) {
		}
	} else if (((((sr1itflags & I2C_FLAG_RXNE) != RESET)
			&& ((itsources & I2C_IT_BUF) != RESET)
			&& ((sr1itflags & I2C_FLAG_BTF) == RESET)))
			|| (((sr1itflags & I2C_FLAG_BTF) != RESET)
					&& ((itsources & I2C_IT_EVT) != RESET))) {
		reg->w_ticks = get_tick32() - reg->f_w_ticks ;
		reg->f_w_ticks = get_tick32();
		if (reg->state == I2C_WRITE_ADDRESS_RECEIVED) {
			if (dev->mode.reg_16_bit) {
				reg->state = I2C_WRITE_1ST_REG_BYTE_RECEIVED;
				if (dev->mode.reg_16_big_endian) {
					reg->start_reg_index = (hi2c->Instance->DR << 8);
				} else {
					reg->start_reg_index = hi2c->Instance->DR;
				}
			} else {
				reg->start_reg_index = hi2c->Instance->DR;
				reg->reg_index = reg->start_reg_index;
				reg->state = I2C_WRITING_DATA;
			}
			reg->w_count++;
		}
		else if (reg->state == I2C_WRITE_1ST_REG_BYTE_RECEIVED) {
			if (dev->mode.reg_16_big_endian) {
				reg->start_reg_index |= hi2c->Instance->DR;
			} else {
				reg->start_reg_index |= (hi2c->Instance->DR << 8);
			}
			reg->reg_index = reg->start_reg_index;
			reg->state = I2C_WRITING_DATA;
			reg->w_count++;

		}
		else {
			write_reg(reg->reg_index, hi2c->Instance->DR, PERIPH_ACCESS);
			reg->w_count++;
			add_index(&(reg->reg_index));
		}
	}
	if (READ_REG(hi2c->Instance->SR2) & I2C_FLAG_BUSY) {
		reg->status.busy = 1;
	} else {
		reg->status.busy = 0;
	}
}

static void _i2c_err(i2c_dev *dev) {
	I2C_HandleTypeDef *hi2c = &dev->hi2c;
	i2c_t *reg = dev->reg;

	uint32_t sr1itflags = READ_REG(hi2c->Instance->SR1);
	uint32_t itsources = READ_REG(hi2c->Instance->CR2);

	if(reg->state == I2C_READING_DATA) {
		reg->f_r_ticks = get_tick32() - reg->s_ticks;
	}
	else if (IS_STATE_WRITING(reg->state)){
		reg->f_w_ticks = get_tick32() - reg->s_ticks;
	}

	if (((sr1itflags & I2C_FLAG_AF) != RESET)
			&& ((itsources & I2C_IT_ERR) != RESET)) {
		sub_index(&reg->reg_index);
		reg->status.af = 1;
		reg->status.busy = 0;
		reg->state = I2C_ADDR_NACK;

	} else if ((itsources & I2C_IT_ERR) != RESET) {
		if (((sr1itflags & I2C_FLAG_BERR) != RESET)) {
			reg->status.berr = 1;
			/* Clear BERR flag */
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_BERR);
		}
		/* I2C Arbitration Loss error interrupt occurred ---------------------*/
		if ((sr1itflags & I2C_FLAG_ARLO) != RESET) {
			/* Clear ARLO flag */
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_ARLO);
		}

		/* I2C Over-Run/Under-Run interrupt occurred -------------------------*/
		if ((sr1itflags & I2C_FLAG_OVR) != RESET) {
			reg->status.ovr = 1;
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
		reg->state = I2C_STOPPED;
		if (READ_REG(hi2c->Instance->SR2) & I2C_FLAG_BUSY) {
			reg->status.busy = 1;
		} else {
			reg->status.busy = 0;
		}
	}
	__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
}
