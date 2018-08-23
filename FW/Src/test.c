/*
 * test.c
 *
 *  Created on: Jul 23, 2018
 *      Author: kevinweiss
 */

#include "test.h"

#if defined(TEST_MODE)

#include <string.h>

#include "stm32f1xx_hal.h"

#include "port_uart.h"
#include "app_shell_if.h"

#include "port.h"
#include "app_access.h"
#include "app_errno.h"
#include "app_typedef.h"
#include "app_common.h"
#include "app.h"
#include "app_debug.h"

#define TOTAL_REG_SIZE (1+1+2+2+4+4)
char error_msg[128];

void test_failed() {
	while (1);
}

void _reg_increase_test() {
	uint32_t index[6];
	uint32_t sizes[6] = {1, 1, 2, 2, 4, 4};
	index[0] = offsetof(map_t, user_reg);
	for (int i = 1; i < 6; i++) {
		index[i] = index[i-1] + sizes[i - 1];
	}


	uint8_t t_data0 = 0;
	int8_t t_data1 = 0;
	uint16_t t_data2 = 0;
	int16_t t_data3 = 0;
	uint32_t t_data4 = 0;
	int32_t t_data5 = 0;

	uint8_t data0 = 0;
	int8_t data1 = 0;
	uint16_t data2 = 0;
	int16_t data3 = 0;
	uint32_t data4 = 0;
	int32_t data5 = 0;

	uint8_t clear_buf[TOTAL_REG_SIZE] = {0};
	write_regs(index[0], clear_buf, TOTAL_REG_SIZE, IF_ACCESS);
	for (uint32_t i = 0; i < 0x1FF; i++) {
		read_regs(index[0], (uint8_t*)&data0, sizes[0]);
		if (data0 != t_data0) test_failed();
		t_data0++;
		if (increase_reg_uint8(index[0], IF_ACCESS) != EOK) test_failed();

		read_regs(index[1], (uint8_t*)&data1, sizes[1]);
		if (data1 != t_data1) test_failed();
		t_data1++;
		if (increase_reg_int8(index[1], IF_ACCESS) != EOK) test_failed();
	}

	for (uint32_t i = 0; i < 0x1FFFF; i++) {
		read_regs(index[2], (uint8_t*)&data2, sizes[2]);
		if (data2 != t_data2) test_failed();
		t_data2++;
		if (increase_reg_uint16(index[2], IF_ACCESS) != EOK) test_failed();

		read_regs(index[3], (uint8_t*)&data3, sizes[3]);
		if (data3 != t_data3) test_failed();
		t_data3++;
		if (increase_reg_int16(index[3], IF_ACCESS) != EOK) test_failed();
	}

	for (uint32_t i = 0; i < 0x1FFFF; i++) {
		read_regs(index[4], (uint8_t*)&data4, sizes[4]);
		if (data4 != t_data4) test_failed();
		t_data4++;
		if (increase_reg_uint32(index[4], IF_ACCESS) != EOK) test_failed();

		read_regs(index[5], (uint8_t*)&data5, sizes[5]);
		if (data5 != t_data5) test_failed();
		t_data5++;
		if (increase_reg_int32(index[5], IF_ACCESS) != EOK) test_failed();
	}

	t_data4 = 0x7FFFFFF0;
	t_data5 = 0x7FFFFFF0;
	write_regs(index[4], (uint8_t*)&t_data4, sizeof(t_data4), IF_ACCESS);
	write_regs(index[5], (uint8_t*)&t_data5, sizeof(t_data5), IF_ACCESS);
	for (uint32_t i = 0; i < 0x1FFFF; i++) {
		read_regs(index[4], (uint8_t*)&data4, sizes[4]);
		if (data4 != t_data4) test_failed();
		t_data4++;
		if (increase_reg_uint32(index[4], IF_ACCESS) != EOK) test_failed();

		read_regs(index[5], (uint8_t*)&data5, sizes[5]);
		if (data5 != t_data5) test_failed();
		t_data5++;
		if (increase_reg_int32(index[5], IF_ACCESS) != EOK) test_failed();
	}

	t_data4 = 0xFFFFFFF0;
	t_data5 = 0xFFFFFFF0;
	write_regs(index[4], (uint8_t*)&t_data4, sizeof(t_data4), IF_ACCESS);
	write_regs(index[5], (uint8_t*)&t_data5, sizeof(t_data5), IF_ACCESS);
	for (uint32_t i = 0; i < 0x1FFFF; i++) {
		read_regs(index[4], (uint8_t*)&data4, sizes[4]);
		if (data4 != t_data4) test_failed();
		t_data4++;
		if (increase_reg_uint32(index[4], IF_ACCESS) != EOK) test_failed();

		read_regs(index[5], (uint8_t*)&data5, sizes[5]);
		if (data5 != t_data5) test_failed();
		t_data5++;
		if (increase_reg_int32(index[5], IF_ACCESS) != EOK) test_failed();
	}
}


void _reg_decrease_test() {
	uint32_t index[6];
	uint32_t sizes[6] = {1, 1, 2, 2, 4, 4};
	index[0] = offsetof(map_t, user_reg);
	for (int i = 1; i < 6; i++) {
		index[i] = index[i-1] + sizes[i - 1];
	}


	uint8_t t_data0 = 0;
	int8_t t_data1 = 0;
	uint16_t t_data2 = 0;
	int16_t t_data3 = 0;
	uint32_t t_data4 = 0;
	int32_t t_data5 = 0;

	uint8_t data0 = 0;
	int8_t data1 = 0;
	uint16_t data2 = 0;
	int16_t data3 = 0;
	uint32_t data4 = 0;
	int32_t data5 = 0;

	uint8_t clear_buf[TOTAL_REG_SIZE] = {0};
	write_regs(index[0], clear_buf, TOTAL_REG_SIZE, IF_ACCESS);
	for (uint32_t i = 0; i < 0x1FF; i++) {
		read_regs(index[0], (uint8_t*)&data0, sizes[0]);
		if (data0 != t_data0) test_failed();
		t_data0--;
		if (decrease_reg_uint8(index[0], IF_ACCESS) != EOK) test_failed();

		read_regs(index[1], (uint8_t*)&data1, sizes[1]);
		if (data1 != t_data1) test_failed();
		t_data1--;
		if (decrease_reg_int8(index[1], IF_ACCESS) != EOK) test_failed();
	}

	for (uint32_t i = 0; i < 0x1FFFF; i++) {
		read_regs(index[2], (uint8_t*)&data2, sizes[2]);
		if (data2 != t_data2) test_failed();
		t_data2--;
		if (decrease_reg_uint16(index[2], IF_ACCESS) != EOK) test_failed();

		read_regs(index[3], (uint8_t*)&data3, sizes[3]);
		if (data3 != t_data3) test_failed();
		t_data3--;
		if (decrease_reg_int16(index[3], IF_ACCESS) != EOK) test_failed();
	}

	for (uint32_t i = 0; i < 0x1FFFF; i++) {
		read_regs(index[4], (uint8_t*)&data4, sizes[4]);
		if (data4 != t_data4) test_failed();
		t_data4--;
		if (decrease_reg_uint32(index[4], IF_ACCESS) != EOK) test_failed();

		read_regs(index[5], (uint8_t*)&data5, sizes[5]);
		if (data5 != t_data5) test_failed();
		t_data5--;
		if (decrease_reg_int32(index[5], IF_ACCESS) != EOK) test_failed();
	}

	t_data4 = 0x7FFFFFF0;
	t_data5 = 0x7FFFFFF0;
	write_regs(index[4], (uint8_t*)&t_data4, sizeof(t_data4), IF_ACCESS);
	write_regs(index[5], (uint8_t*)&t_data5, sizeof(t_data5), IF_ACCESS);
	for (uint32_t i = 0; i < 0x1FFFF; i++) {
		read_regs(index[4], (uint8_t*)&data4, sizes[4]);
		if (data4 != t_data4) test_failed();
		t_data4--;
		if (decrease_reg_uint32(index[4], IF_ACCESS) != EOK) test_failed();

		read_regs(index[5], (uint8_t*)&data5, sizes[5]);
		if (data5 != t_data5) test_failed();
		t_data5--;
		if (decrease_reg_int32(index[5], IF_ACCESS) != EOK) test_failed();
	}

	t_data4 = 0xFFFFFFF0;
	t_data5 = 0xFFFFFFF0;
	write_regs(index[4], (uint8_t*)&t_data4, sizeof(t_data4), IF_ACCESS);
	write_regs(index[5], (uint8_t*)&t_data5, sizeof(t_data5), IF_ACCESS);
	for (uint32_t i = 0; i < 0x1FFFF; i++) {
		read_regs(index[4], (uint8_t*)&data4, sizes[4]);
		if (data4 != t_data4) test_failed();
		t_data4--;
		if (decrease_reg_uint32(index[4], IF_ACCESS) != EOK) test_failed();

		read_regs(index[5], (uint8_t*)&data5, sizes[5]);
		if (data5 != t_data5) test_failed();
		t_data5--;
		if (decrease_reg_int32(index[5], IF_ACCESS) != EOK) test_failed();
	}
}


void run_test() {
	_reg_increase_test();
	_reg_decrease_test();
}
#else
void run_test(void){

}
#endif
