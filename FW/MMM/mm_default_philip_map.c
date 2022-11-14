/**
 ******************************************************************************
 * @addtogroup philip_map_mmm
 * @{
 * @file      mm_default_philip_map.c
 * @version   2.0.0
 * @}
 * @details   Generated from the memory map manager version 2.0.0
 ******************************************************************************
 */

/* includes ******************************************************************/
#include "mm_typedefs_philip_map.h"
#include "mm_default_philip_map.h"

/* functions *****************************************************************/
const void mm_defaults_init_philip_map(philip_map *init) {
    init->sys.device_num = MM_DEFAULT_PHILIP_MAP_SYS_DEVICE_NUM;
    init->i2c.slave_addr_1 = MM_DEFAULT_PHILIP_MAP_I2C_SLAVE_ADDR_1;
    init->i2c.slave_addr_2 = MM_DEFAULT_PHILIP_MAP_I2C_SLAVE_ADDR_2;
    init->adc.num_of_samples = MM_DEFAULT_PHILIP_MAP_ADC_NUM_OF_SAMPLES;
    init->pwm.h_ticks = MM_DEFAULT_PHILIP_MAP_PWM_H_TICKS;
    init->pwm.l_ticks = MM_DEFAULT_PHILIP_MAP_PWM_L_TICKS;
    init->dac.level = MM_DEFAULT_PHILIP_MAP_DAC_LEVEL;
    init->tmr.min_tick = MM_DEFAULT_PHILIP_MAP_TMR_MIN_TICK;
}