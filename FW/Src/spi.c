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
 * @file			spi.c
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			Controls the spi peripheral.
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

#include "spi.h"

/* Private enums/structs -----------------------------------------------------*/
/** @brief  							The state settings of the SPI */
enum SPI_STATE {
	SPI_INITIALIZED, /**< spi has been initialized */
	SPI_FRAME_STARTED, /**< spi NSS pin just lowered */
	SPI_WRITING, /**< spi writing to reg */
	SPI_READING, /**< spi reading reg */
	SPI_TRANSFERING, /**< spi transferring data */
	SPI_FRAME_FINISHED /**< spi cs pin up and finished */
};

/** @brief	Interface type for the spi */
enum SPI_IF_TYPE {
	SPI_IF_TYPE_REG, /**< Access registers with spi */
	SPI_IF_TYPE_HS, /**< Preloads reg address to 0 for high speed tests */
	SPI_IF_TYPE_ECHO, /**< Echos SPI bytes */
	SPI_IF_TYPE_CONST /**< Always output user reg 0 */
};

/** @brief	The parameters for spi control */
typedef struct {
	SPI_HandleTypeDef hspi; /**< Handle for the spi device */
	spi_t *reg; /**< spi live application registers */
	spi_t *saved_reg; /**< spi saved application registers */
	void (*if_mode_int)(void); /**< Interrupt function pointer */
	uint8_t initial_byte; /**< The byte that is output first */
} spi_dev;
/** @} */

/* Private function prototypes -----------------------------------------------*/
static inline error_t _commit_spi(spi_dev *dev);
static void _spi_echo_int();
static void _spi_hs_int();
static void _spi_reg_int();
static void _spi_const_int();

/* Private defines -----------------------------------------------------------*/
/** @brief	Checks the write direction */
#define SPI_ADDR_MASK	(0x80)

/* Private variables ---------------------------------------------------------*/
static spi_dev dut_spi;

/* Functions -----------------------------------------------------------------*/
/**
 * @brief		Initializes spi registers.
 *
 * @param[in]	reg			Pointer to live register memory map
 * @param[in]	saved_reg	Pointer to saved register memory map
 * @note		Populates spi defaults registers and assigns spi register
 * 				pointers.
 */
void init_dut_spi(map_t *reg, map_t *saved_reg) {
	SPI_HandleTypeDef *hspi = &dut_spi.hspi;

	hspi->Instance = DUT_SPI;
	hspi->Init.Mode = SPI_MODE_SLAVE;
	hspi->Init.Direction = SPI_DIRECTION_2LINES;
	hspi->Init.DataSize = SPI_DATASIZE_8BIT;
	hspi->Init.NSS = SPI_NSS_HARD_INPUT;
	hspi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
	hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi->Init.TIMode = SPI_TIMODE_DISABLE;
	hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi->Init.CRCPolynomial = -0;

	dut_spi.reg = &(reg->spi);
	dut_spi.saved_reg = &(saved_reg->spi);
	commit_dut_spi();
}

/**
 * @brief		Commits the spi registers and executes operations.
 *
 * @pre			spi must first be initialized with init_dut_spi()
 * @return      EOK if init occurred
 * @return      ENOACTION if no init was triggered
 *
 * @note		Only executes actions if the spi.mode.init is set.
 */
error_t commit_dut_spi() {
	return _commit_spi(&dut_spi);
}

static inline error_t _commit_spi(spi_dev *dev) {
	SPI_HandleTypeDef *hspi_inst = &dev->hspi;
	spi_t *spi = dev->reg;

	if (!spi->mode.init) {
		__HAL_SPI_DISABLE_IT(hspi_inst,
				SPI_IT_RXNE | SPI_CR2_ERRIE | SPI_CR2_TXEIE);

		hspi_inst->Init.CLKPhase = SPI_PHASE_1EDGE;
		if (spi->mode.cpha) {
			hspi_inst->Init.CLKPhase = SPI_PHASE_2EDGE;
		}

		hspi_inst->Init.CLKPolarity = SPI_POLARITY_LOW;
		if (spi->mode.cpol) {
			hspi_inst->Init.CLKPolarity = SPI_POLARITY_HIGH;
		}
		dut_spi.reg->frame_ticks = 0;
		dut_spi.reg->prev_ticks = 0;
        spi->w_count = 0;
        spi->r_count = 0;
		dev->initial_byte = SPI_NO_DATA_BYTE;
		if (spi->mode.if_type == SPI_IF_TYPE_REG) {
			dev->if_mode_int = _spi_reg_int;
		} else if (spi->mode.if_type == SPI_IF_TYPE_HS) {
			dev->if_mode_int = _spi_hs_int;
		} else if (spi->mode.if_type == SPI_IF_TYPE_ECHO) {
			dev->if_mode_int = _spi_echo_int;
		} else {
			read_reg(0, &(dev->initial_byte));
			dev->if_mode_int = _spi_const_int;
		}

		spi->mode.init = 1;
		copy_until_same(dev->saved_reg, dev->reg, sizeof(*(dev->saved_reg)));

		__HAL_SPI_DISABLE(hspi_inst);
		if (!dev->reg->mode.disable) {
			HAL_SPI_Init(hspi_inst);
			__HAL_SPI_ENABLE(hspi_inst);
			hspi_inst->Instance->DR = dev->initial_byte;
			__HAL_SPI_ENABLE_IT(hspi_inst,
					SPI_IT_RXNE | SPI_CR2_ERRIE | SPI_CR2_TXEIE);
		}

		return EOK;
	}
	return ENOACTION;
}

/**
 * @brief		Updates the spi input levels.
 */
void update_dut_spi_inputs() {
	dut_spi.reg->status.clk = HAL_GPIO_ReadPin(DUT_SCK);
}
/* Interrupts ----------------------------------------------------------------*/
/**
 * @brief This function handles DUT_SPI global interrupt.
 */
void DUT_SPI_INT(void) {
	dut_spi.if_mode_int();
}

static void _spi_reg_int() {
	SPI_HandleTypeDef *hspi = &(dut_spi.hspi);
	spi_t *spi = dut_spi.reg;

	if (spi->state == SPI_FRAME_STARTED) {
		if (hspi->Instance->SR & SPI_FLAG_RXNE) {
			uint32_t data = hspi->Instance->DR;
			unprotected_read_uint8(data & (~SPI_ADDR_MASK),
					(uint8_t*) &(hspi->Instance->DR));
			if (data & SPI_ADDR_MASK) {
				spi->state = SPI_READING;
			} else {
				spi->state = SPI_WRITING;
			}
			spi->start_reg_index = data & (~SPI_ADDR_MASK);
			spi->reg_index = spi->start_reg_index;
			spi->transfer_count++;
		}
	} else if (spi->state == SPI_READING || spi->state == SPI_WRITING) {
		uint32_t itflag = hspi->Instance->SR;
		if (itflag & SPI_FLAG_TXE) {
			unprotected_read_uint8(spi->reg_index + 1,
					(uint8_t*) &(hspi->Instance->DR));
		}
		if (itflag & SPI_FLAG_RXNE) {
			uint8_t data = hspi->Instance->DR;
			if (spi->state == SPI_WRITING) {
				if (write_reg(spi->reg_index, data, PERIPH_ACCESS)) {
					spi->status.index_err = 1;
				}
				spi->w_count++;
			} else {
				spi->r_count++;
			}
			add_index(&dut_spi.reg->reg_index);
			spi->transfer_count++;
		}
	}
}

static void _spi_hs_int() {
	SPI_HandleTypeDef *hspi = &(dut_spi.hspi);
	spi_t *spi = dut_spi.reg;
	uint32_t itflag = hspi->Instance->SR;

	if (itflag & SPI_FLAG_TXE) {
		unprotected_read_uint8(spi->reg_index,
				(uint8_t*) &(hspi->Instance->DR));
		dut_spi.reg->state = SPI_TRANSFERING;
	}
	if (itflag & SPI_FLAG_RXNE) {
		if (write_reg(spi->reg_index, (uint8_t) (hspi->Instance->DR),
		PERIPH_ACCESS)) {
			spi->status.index_err = 1;
		}
		add_index(&dut_spi.reg->reg_index);
		spi->transfer_count++;
		// get ticks per byte
		dut_spi.reg->state = SPI_TRANSFERING;
	}

}

static void _spi_echo_int() {
	if (dut_spi.hspi.Instance->SR & SPI_FLAG_RXNE) {
		uint32_t tmp = dut_spi.hspi.Instance->DR;
		dut_spi.hspi.Instance->DR = tmp;
		dut_spi.reg->transfer_count++;
	}
}

#pragma GCC push_options
#pragma GCC optimize ("O3")
static void _spi_const_int() {
	if (dut_spi.hspi.Instance->SR & SPI_FLAG_RXNE) {
		(void)dut_spi.hspi.Instance->DR;
		dut_spi.reg->byte_ticks = get_tick32() - dut_spi.reg->prev_ticks;
		dut_spi.reg->prev_ticks += dut_spi.reg->byte_ticks;
		dut_spi.reg->transfer_count++;
	}
}
#pragma GCC pop_options

/**
 * @brief Interrupt for the spi line raise or lower
 */
#pragma GCC push_options
#pragma GCC optimize ("O3")
void dut_nss_int() {
	if (HAL_GPIO_ReadPin(DUT_NSS)) {
		/* Finished frame */
		uint32_t itflag = dut_spi.hspi.Instance->SR;

		dut_spi.reg->frame_ticks = get_tick32() - dut_spi.reg->frame_ticks;
		dut_spi.reg->status.end_clk = HAL_GPIO_ReadPin(DUT_SCK);

		/* read error flags */
		memset(&dut_spi.reg->status, 0, sizeof(dut_spi.reg->status));
		if (itflag & SPI_SR_MODF) {
			dut_spi.reg->status.modf = 1;
		}
		if (itflag & SPI_SR_BSY) {
			dut_spi.reg->status.bsy = 1;
		}
		if (itflag & SPI_SR_OVR) {
			dut_spi.reg->status.ovr = 1;
		}
		if (itflag & SPI_SR_UDR) {
			dut_spi.reg->status.udr = 1;
		}

		/* clear status and errors */
		uint32_t tmp = dut_spi.hspi.Instance->DR;
		(void) tmp;
		dut_spi.hspi.Instance->DR = dut_spi.initial_byte;

		dut_spi.reg->state = SPI_FRAME_FINISHED;
	} else {
		/* Starting frame */
		if (dut_spi.reg->mode.if_type == SPI_IF_TYPE_CONST) {
			dut_spi.reg->transfer_count = 0;
			dut_spi.reg->frame_ticks = get_tick32();
			dut_spi.reg->prev_ticks = dut_spi.reg->frame_ticks;
			if (dut_spi.hspi.Instance->SR & SPI_FLAG_RXNE) {
				(void)dut_spi.hspi.Instance->DR;
				dut_spi.reg->transfer_count = 1;
			}
			return;
		}
		dut_spi.reg->status.start_clk = HAL_GPIO_ReadPin(DUT_SCK);
		dut_spi.hspi.Instance->DR = dut_spi.initial_byte;
		dut_spi.reg->transfer_count = 0;
		if (dut_spi.reg->mode.if_type == SPI_IF_TYPE_ECHO) {
			return;
		}
		dut_spi.reg->reg_index = 0;
		dut_spi.reg->state = SPI_FRAME_STARTED;
	}
}
#pragma GCC pop_options
