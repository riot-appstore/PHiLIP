/**
 ******************************************************************************
 * @addtogroup PHiLIP_MMM
 * @{
 * @file      PHiLIP_map.c
 * @author    Kevin Weiss
 * @version   0.0.2
 * @date      2019-04-21
 * @}
 * @details   Generated from the memory map manager version 0.0.9
 ******************************************************************************
 */

/* Includes ----------------------------------------------------------------*/
#include <stdint.h>
#include "PHiLIP_map.h"

/* Global variables --------------------------------------------------------*/
const char* const PHILIP_TYPE_NAME[] = { "uint8_t", "int8_t", "uint16_t", "int16_t", "uint32_t", "int32_t", "uint64_t", "int64_t", "char", "float", "double" }; /** < type_name enum */
const uint8_t  PHILIP_TYPE_SIZE[] = { 1, 1, 2, 2, 4, 4, 8, 8, 1, 4, 8 }; /** <  type_size const array */

const char* const MAP_T_NAME[] = { "user_reg", "sys.sn", "sys.fw_rev", "sys.if_rev", "sys.tick", "sys.build_time.second", "sys.build_time.minute", "sys.build_time.hour", "sys.build_time.day_of_month", "sys.build_time.day_of_week", "sys.build_time.month", "sys.build_time.year", "sys.build_time.res", "sys.device_num", "sys.sys_clk", "sys.status", "sys.status.update", "sys.status.board", "sys.mode", "sys.mode.init", "sys.mode.dut_rst", "sys.res", "i2c.mode", "i2c.mode.init", "i2c.mode.disable", "i2c.mode.addr_10_bit", "i2c.mode.general_call", "i2c.mode.no_clk_stretch", "i2c.mode.reg_16_bit", "i2c.mode.reg_16_big_endian", "i2c.mode.nack_data", "i2c.status", "i2c.status.ovr", "i2c.status.af", "i2c.status.berr", "i2c.status.gencall", "i2c.status.busy", "i2c.status.rsr", "i2c.clk_stretch_delay", "i2c.slave_addr_1", "i2c.slave_addr_2", "i2c.state", "i2c.reg_index", "i2c.start_reg_index", "i2c.r_count", "i2c.w_count", "i2c.r_ticks", "i2c.w_ticks", "i2c.s_ticks", "i2c.f_r_ticks", "i2c.f_w_ticks", "i2c.res", "spi.mode", "spi.mode.init", "spi.mode.disable", "spi.mode.cpha", "spi.mode.cpol", "spi.mode.if_type", "spi.mode.reg_16_bit", "spi.mode.reg_16_big_endian", "spi.status", "spi.status.bsy", "spi.status.ovr", "spi.status.modf", "spi.status.udr", "spi.status.clk", "spi.status.start_clk", "spi.status.end_clk", "spi.status.index_err", "spi.state", "spi.reg_index", "spi.start_reg_index", "spi.r_count", "spi.w_count", "spi.transfer_count", "spi.frame_ticks", "spi.byte_ticks", "spi.prev_ticks", "spi.res", "uart.mode", "uart.mode.init", "uart.mode.disable", "uart.mode.if_type", "uart.mode.stop_bits", "uart.mode.parity", "uart.mode.rts", "uart.mode.data_bits", "uart.baud", "uart.mask_msb", "uart.rx_count", "uart.tx_count", "uart.status", "uart.status.cts", "uart.status.pe", "uart.status.fe", "uart.status.nf", "uart.status.ore", "uart.res", "rtc.mode", "rtc.mode.init", "rtc.mode.disable", "rtc.second", "rtc.minute", "rtc.hour", "rtc.day", "rtc.set_second", "rtc.set_minute", "rtc.set_hour", "rtc.set_day", "rtc.res", "adc.mode", "adc.mode.init", "adc.mode.enable", "adc.mode.fast_sample", "adc.num_of_samples", "adc.counter", "adc.index", "adc.sample", "adc.sum", "adc.current_sum", "adc.res", "pwm.mode", "pwm.mode.init", "pwm.mode.disable", "pwm.status", "pwm.duty_cycle", "pwm.period", "pwm.h_ticks", "pwm.l_ticks", "pwm.res", "dac.mode", "dac.mode.init", "dac.mode.disable", "dac.status", "dac.level", "dac.res", "gpio[0].mode", "gpio[0].mode.init", "gpio[0].mode.io_type", "gpio[0].mode.level", "gpio[0].mode.pull", "gpio[0].mode.tick_div", "gpio[0].status", "gpio[0].status.level", "gpio[0].res", "gpio[1].mode", "gpio[1].mode.init", "gpio[1].mode.io_type", "gpio[1].mode.level", "gpio[1].mode.pull", "gpio[1].mode.tick_div", "gpio[1].status", "gpio[1].status.level", "gpio[1].res", "gpio[2].mode", "gpio[2].mode.init", "gpio[2].mode.io_type", "gpio[2].mode.level", "gpio[2].mode.pull", "gpio[2].mode.tick_div", "gpio[2].status", "gpio[2].status.level", "gpio[2].res", "trace.index", "trace.tick_div", "trace.source", "trace.value", "trace.tick", "res"}; /** < name const array */

const uint16_t MAP_T_OFFSET[] = { 0, 256, 268, 272, 276, 284, 285, 286, 287, 288, 289, 290, 291, 292, 296, 300, 300, 300, 301, 301, 301, 302, 320, 320, 320, 320, 320, 320, 320, 320, 320, 321, 321, 321, 321, 321, 321, 321, 322, 324, 326, 328, 330, 332, 334, 335, 336, 340, 344, 348, 352, 356, 384, 384, 384, 384, 384, 384, 384, 384, 385, 385, 385, 385, 385, 385, 385, 385, 385, 386, 388, 390, 392, 393, 394, 395, 399, 403, 407, 416, 416, 416, 416, 416, 416, 416, 416, 418, 422, 423, 425, 427, 427, 427, 427, 427, 427, 428, 432, 432, 432, 433, 434, 435, 436, 438, 439, 440, 441, 443, 448, 448, 448, 448, 449, 453, 454, 458, 460, 464, 468, 480, 480, 480, 481, 482, 484, 488, 492, 496, 512, 512, 512, 513, 514, 516, 528, 528, 528, 528, 528, 528, 530, 530, 531, 532, 532, 532, 532, 532, 532, 534, 534, 535, 536, 536, 536, 536, 536, 536, 538, 538, 539, 540, 544, 576, 608, 672, 800}; /** < offset const array */

const uint8_t MAP_T_TYPE[] = { 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 0, 0, 0, 4, 4, 4, 0, 2, 2, 2, 2, 2, 2, 2, 2, 4, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 0, 0, 4, 0, 4, 2, 4, 4, 0, 0, 0, 0, 0, 2, 4, 4, 4, 0, 0, 0, 0, 0, 2, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 2, 2, 2, 2, 2, 2, 0, 0, 0, 4, 0, 0, 2, 4, 0}; /** < type_name const array */

const uint16_t MAP_T_ARRAY_SIZE[] = { 256, 12, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 32, 32, 32, 32, 224}; /** < array_size const array */

const uint8_t MAP_T_BIT_OFFSET[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 1, 2, 3, 4, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 0, 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 4, 5, 7, 8, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 3, 4, 6, 0, 0, 0, 0, 0, 1, 3, 4, 6, 0, 0, 0, 0, 0, 1, 3, 4, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0}; /** < bit_offset const array */

const uint8_t MAP_T_BITS[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 1, 2, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 2, 1, 2, 5, 0, 1, 0, 0, 1, 2, 1, 2, 5, 0, 1, 0, 0, 1, 2, 1, 2, 5, 0, 1, 0, 0, 0, 0, 0, 0, 0}; /** < bits const array */

