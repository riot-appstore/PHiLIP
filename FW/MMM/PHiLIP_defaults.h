/**
 ******************************************************************************
 * @addtogroup PHiLIP_MMM
 * @{
 * @file      PHiLIP_defaults.h
 * @author    Kevin Weiss
 * @version   0.0.2
 * @date      2019-04-21
 * @details   Generated from the memory map manager version 0.0.9
 ******************************************************************************
 */

#ifndef PHILIP_DEFAULTS_H
#define PHILIP_DEFAULTS_H

/* Defines -------------------------------------------------------------------*/
/** @brief default for sys.device_num: The philip device designator - A constant number to identify philip firmware */
#define DEFAULT_SYS_DEVICE_NUM 17061
/** @brief default for i2c.slave_addr_1: Primary slave address */
#define DEFAULT_I2C_SLAVE_ADDR_1 85
/** @brief default for i2c.slave_addr_2: Secondary slave address */
#define DEFAULT_I2C_SLAVE_ADDR_2 66
/** @brief default for adc.num_of_samples: Number of sample in the sum */
#define DEFAULT_ADC_NUM_OF_SAMPLES 1024
/** @brief default for pwm.h_ticks: Settable high time in sys clock ticks */
#define DEFAULT_PWM_H_TICKS 10000
/** @brief default for pwm.l_ticks: Settable low time in sys clock ticks */
#define DEFAULT_PWM_L_TICKS 10000
/** @brief default for dac.level: The percent/100 of output level */
#define DEFAULT_DAC_LEVEL 5000

/** @brief Assign defaults for map_t */
void init_defaults_map_t(map_t *init);

#endif /* PHILIP_DEFAULTS_H */
/** @} */
