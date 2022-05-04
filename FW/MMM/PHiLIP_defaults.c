/**
 *****************************************************************************"
 * @addtogroup PHiLIP_MMM
 * @{
 * @file      PHiLIP_defaults.c
 * @author    Kevin Weiss
 * @version   1.3.0
 * @}
 * @details   Generated from the memory map manager version 1.0.0
 *****************************************************************************"
 */
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

#include "PHiLIP_typedef.h"
#include "PHiLIP_defaults.h"

/* Functions -----------------------------------------------------------------*/
/** @brief Assign defaults for map_t */
void init_defaults_map_t(map_t *init) {
	init->sys.device_num = DEFAULT_SYS_DEVICE_NUM;
	init->i2c.slave_addr_1 = DEFAULT_I2C_SLAVE_ADDR_1;
	init->i2c.slave_addr_2 = DEFAULT_I2C_SLAVE_ADDR_2;
	init->adc.num_of_samples = DEFAULT_ADC_NUM_OF_SAMPLES;
	init->pwm.h_ticks = DEFAULT_PWM_H_TICKS;
	init->pwm.l_ticks = DEFAULT_PWM_L_TICKS;
	init->dac.level = DEFAULT_DAC_LEVEL;
	init->tmr.min_tick = DEFAULT_TMR_MIN_TICK;
}

