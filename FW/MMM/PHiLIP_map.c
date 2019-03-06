/**
 ******************************************************************************
 * @addtogroup PHiLIP_MMM
 * @{
 * @file      PHiLIP_map.c
 * @author    Kevin Weiss
 * @version   0.0.2
 * @date      2019-03-06
 * @}
 * @details   Generated from the memory map manager
 ******************************************************************************
 */

/* Includes ----------------------------------------------------------------*/
#include <stdint.h>
#include "PHiLIP_map.h"

/* Global variables --------------------------------------------------------*/
const char* const PHILIP_TYPE_NAME[] = { "uint8_t", "int8_t", "uint16_t", "int16_t", "uint32_t", "int32_t", "uint64_t", "int64_t", "char", "float", "double" }; /** < type_name enum */
const uint8_t  PHILIP_TYPE_SIZE[] = { 1, 1, 2, 2, 4, 4, 8, 8, 1, 4, 8 }; /** <  type_size const array */

const char* const MAP_T_NAME[] = { "user_reg", "sys.sn", "sys.fw_rev", "sys.if_rev", "sys.tick", "sys.build_time.second", "sys.build_time.minute", "sys.build_time.hour", "sys.build_time.day_of_month", "sys.build_time.day_of_week", "sys.build_time.month", "sys.build_time.year", "sys.build_time.res", "sys.device_num", "sys.status", "sys.status.update", "sys.mode", "sys.mode.init", "sys.mode.dut_rst", "sys.res", "i2c.mode", "i2c.mode.init", "i2c.mode.disable", "i2c.mode.addr_10_bit", "i2c.mode.general_call", "i2c.mode.no_clk_stretch", "i2c.mode.reg_16_bit", "i2c.mode.reg_16_big_endian", "i2c.mode.nack_data", "i2c.status", "i2c.status.ovr", "i2c.status.af", "i2c.status.berr", "i2c.status.gencall", "i2c.status.busy", "i2c.status.rsr", "i2c.clk_stretch_delay", "i2c.slave_addr_1", "i2c.slave_addr_2", "i2c.state", "i2c.reg_index", "i2c.start_reg_index", "i2c.r_count", "i2c.w_count", "i2c.r_ticks", "i2c.w_ticks", "i2c.s_ticks", "i2c.f_r_ticks", "i2c.f_w_ticks", "i2c.res", "spi.mode", "spi.mode.init", "spi.mode.disable", "spi.mode.cpha", "spi.mode.cpol", "spi.mode.if_type", "spi.mode.reg_16_bit", "spi.mode.reg_16_big_endian", "spi.status", "spi.status.bsy", "spi.status.ovr", "spi.status.modf", "spi.status.udr", "spi.status.index_err", "spi.state", "spi.reg_index", "spi.start_reg_index", "spi.r_count", "spi.w_count", "spi.transfer_count", "spi.frame_ticks", "spi.byte_ticks", "spi.res", "uart.mode", "uart.mode.init", "uart.mode.disable", "uart.mode.if_type", "uart.mode.stop_bits", "uart.mode.parity", "uart.mode.rts", "uart.mode.data_bits", "uart.baud", "uart.mask_msb", "uart.rx_count", "uart.tx_count", "uart.status", "uart.status.cts", "uart.status.pe", "uart.status.fe", "uart.status.nf", "uart.status.ore", "uart.res", "rtc.second", "rtc.minute", "rtc.hour", "rtc.day_of_month", "rtc.day_of_week", "rtc.month", "rtc.year", "rtc.res", "adc.0.mode", "adc.0.error_code", "adc.0.sample_rate", "adc.0.value", "adc.0.res", "adc.1.mode", "adc.1.error_code", "adc.1.sample_rate", "adc.1.value", "adc.1.res", "pwm.mode", "pwm.error_code", "pwm.duty", "pwm.freq", "pwm.res", "tmr.mode", "tmr.error_code", "tmr.duty", "tmr.freq", "tmr.hi_us", "tmr.lo_us", "res"}; /** < name const array */

const uint16_t MAP_T_OFFSET[] = { 0, 256, 268, 272, 276, 284, 285, 286, 287, 288, 289, 290, 291, 292, 296, 296, 297, 297, 297, 298, 320, 320, 320, 320, 320, 320, 320, 320, 320, 321, 321, 321, 321, 321, 321, 321, 322, 324, 326, 328, 330, 332, 334, 335, 336, 340, 344, 348, 352, 356, 384, 384, 384, 384, 384, 384, 384, 384, 385, 385, 385, 385, 385, 385, 386, 388, 390, 392, 393, 394, 395, 399, 403, 416, 416, 416, 416, 416, 416, 416, 416, 418, 422, 423, 425, 427, 427, 427, 427, 427, 427, 428, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 443, 444, 448, 456, 457, 459, 460, 464, 472, 473, 475, 476, 480, 488, 489, 491, 492, 496, 500, 504}; /** < offset const array */

const uint8_t MAP_T_TYPE[] = { 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 4, 4, 0, 2, 2, 2, 2, 2, 2, 2, 2, 4, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 4, 0, 0, 2, 0, 4, 0, 0, 2, 0, 4, 0, 0, 2, 0, 4, 4, 4, 0}; /** < type_name const array */

const uint16_t MAP_T_ARRAY_SIZE[] = { 256, 12, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 8, 0, 0, 0, 0, 8, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 520}; /** < array_size const array */

const uint8_t MAP_T_BIT_OFFSET[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 0, 0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 4, 5, 7, 8, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /** < bit_offset const array */

const uint8_t MAP_T_BITS[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 2, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /** < bits const array */

