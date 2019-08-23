/*
 * MIT License
 *
 * Copyright 2019 Kevin Weiss for HAW Hamburg
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
 * @file			adc.c
 * @author			Kevin Weiss
 * @date			20.03.2019
 * @brief			Controls the adc peripheral.
 * @}
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

#include "adc.h"

/* Private enums/structs -----------------------------------------------------*/
/** @brief					The parameters for adc control */
typedef struct {
	ADC_TypeDef *adc_inst; /**< Handle for the adc device */
	adc_t *reg; /**< adc live application registers */
	adc_t *saved_reg; /**< adc saved application registers */
} adc_dev;
/** @} */

/* Private function prototypes -----------------------------------------------*/
static void _adc_it(adc_dev *dev);

/* Private variables ---------------------------------------------------------*/
static adc_dev dut_adc;

/* Functions -----------------------------------------------------------------*/
#if 1
void init_dut_adc(map_t *reg, map_t *saved_reg) {
	dut_adc.adc_inst = ADC2;
	ADC_TypeDef *adc_inst = dut_adc.adc_inst;
	dut_adc.reg = &(reg->adc);
	dut_adc.saved_reg = &(saved_reg->adc);

	__HAL_RCC_ADC2_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = DUT_ADC_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	HAL_GPIO_Init(DUT_ADC_GPIO_Port, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(ADC_IRQ, 0, 0);
	HAL_NVIC_EnableIRQ(ADC_IRQ);

	CLEAR_BIT(adc_inst->CR2, (ADC_CR2_ADON));
	while ((adc_inst->CR2 & ADC_CR2_ADON));

	adc_inst->CR2 |= ADC_DATAALIGN_RIGHT | ADC_SOFTWARE_START | ADC_CR2_CONTINUOUS(ENABLE);

#if 0
	if (READ_BIT(adc_inst->CR2,
		~(ADC_CR2_ADON | ADC_CR2_DMA |ADC_CR2_SWSTART | ADC_CR2_JSWSTART |
		ADC_CR2_JEXTTRIG | ADC_CR2_JEXTSEL | ADC_CR2_TSVREFE)) == tmp_cr2)
	{
	/* Maybe keep retrying until good? */
	}
#endif

	/* For Rank 1 to 6 */
    MODIFY_REG(adc_inst->SQR3, ADC_SQR3_RK(ADC_SQR3_SQ1, ADC_REGULAR_RANK_1),
    	ADC_SQR3_RK(ADC_CHANNEL, ADC_REGULAR_RANK_1));

    commit_dut_adc();
}
#endif

error_t commit_dut_adc() {
	ADC_TypeDef *adc_inst = dut_adc.adc_inst;
	if (dut_adc.reg->mode.init) {
		return ENOACTION;
	}

	CLEAR_BIT(adc_inst->CR2, (ADC_CR2_ADON));
	while ((dut_adc.adc_inst->CR2 & ADC_CR2_ADON));

	/* For channels 0 to 9 */
	if (dut_adc.reg->mode.fast_sample) {
		MODIFY_REG(adc_inst->SQR3,
			ADC_SMPR2(ADC_SMPR2_SMP0, ADC_CHANNEL),
			ADC_SMPR2(ADC_SAMPLETIME_1CYCLE_5, ADC_CHANNEL));
	}
	else {
		MODIFY_REG(adc_inst->SMPR2,
			ADC_SMPR2(ADC_SMPR2_SMP0, ADC_CHANNEL),
			ADC_SMPR2(ADC_SAMPLETIME_239CYCLES_5, ADC_CHANNEL));
	}

	adc_inst->CR2 |= ADC_CR2_ADON;
	while (!(adc_inst->CR2 & ADC_CR2_ADON));

	WRITE_REG(adc_inst->SR, ~(ADC_FLAG_EOC));

	if (dut_adc.reg->mode.enable) {
		adc_inst->CR1 |= ADC_IT_EOC;
	}
	else {
		adc_inst->CR1 &= ~ADC_IT_EOC;
	}
	adc_inst->CR2 |= (ADC_CR2_SWSTART | ADC_CR2_EXTTRIG);

	dut_adc.reg->mode.init = 1;
	dut_adc.reg->current_sum = 0;
	dut_adc.reg->index = 0;
	dut_adc.reg->sum = 0;

	copy_until_same(&dut_adc.saved_reg, &dut_adc.reg, sizeof(dut_adc.saved_reg));

	return EOK;
}

/* Interrupts ----------------------------------------------------------------*/
/**
 * @brief This function handles an ADC interrupt
 */
#if 1
void ADC_INT(void) {
	if (dut_adc.adc_inst->SR & ADC_FLAG_EOC) {
		_adc_it(&dut_adc);
		//HAL_ADC_IRQHandler(&dut_adc);
	}
}

static void _adc_it(adc_dev *dev) {
	dev->reg->sample = dev->adc_inst->DR;
	dev->reg->current_sum += dev->reg->sample;
	dev->reg->index++;
	if (dev->reg->index >= dev->saved_reg->num_of_samples) {
		dev->reg->sum = dev->reg->current_sum;
		dev->reg->index = 0;
		dev->reg->current_sum = 0;
		dev->reg->counter++;
	}
}
#endif
