"""I2C Tests

Note: The pins must be connected for the test to work
The tester is a Digilent Analog Discovery 2
Pinout:
PHiLIP      Digilent Analog Discovery 2
DUT_SCL ------------ 4
DUT_SDA ------------ 5
"""
import random
import pytest
from digilent_device import DigilentAnalogDiscovery2


@pytest.mark.parametrize("size", [1, 3, 16, 128])
def test_i2c_read_bytes(phil, tester_dad2, size):
    bytes_read = tester_dad2.i2c_read_bytes(size)
    bytes_expect = phil.read_reg("user_reg", 0, size)["data"]
    read_count = phil.read_reg("i2c.r_count")["data"]
    assert read_count == size
    for _ in range(size):
        assert bytes_read[_] == bytes_expect[_]


@pytest.mark.parametrize("reg", [0, 1, 42, 128-16])
@pytest.mark.parametrize("size", [1, 3, 16])
def test_i2c_read_regs(phil, tester_dad2, reg, size):
    bytes_read = tester_dad2.i2c_read_regs(reg, size)
    bytes_expect = phil.read_reg("user_reg", reg, size)["data"]
    read_count = phil.read_reg("i2c.r_count")["data"]
    write_count = phil.read_reg("i2c.w_count")["data"]
    assert read_count == size
    assert write_count == 1
    for _ in range(size):
        assert bytes_read[_] == bytes_expect[_]


@pytest.mark.parametrize("reg", [1, 43])
@pytest.mark.parametrize("endian", [0, 1])
def test_i2c_read_regs_16_bit(phil, tester_dad2, reg, endian):
    phil.write_reg("i2c.mode.reg_16_bit", 1)
    phil.write_and_execute("i2c.mode.reg_16_big_endian", endian)
    size = 10

    test_reg = reg
    if not endian:
        test_reg = ((reg >> 8) & 0xFF) | (reg << 8)
    bytes_read = tester_dad2.i2c_read_regs(test_reg, size, reg16=True)
    bytes_expect = phil.read_reg("user_reg", reg, size)["data"]
    write_count = phil.read_reg("i2c.w_count")["data"]
    read_count = phil.read_reg("i2c.r_count")["data"]
    assert write_count == 2
    assert read_count == size
    for _ in range(size):
        assert bytes_read[_] == bytes_expect[_], \
            "index={}, data={}, expected={}".format(_, bytes_read[_],
                                                    bytes_expect[_])


@pytest.mark.parametrize("size", [1, 3, 16, 128])
def test_i2c_write_bytes(phil, tester_dad2, size):
    data = random.sample(range(0, 255), size)
    reg_index = 0
    write_data = data.copy()
    # Add register number to write to make deterministic
    write_data.insert(0, reg_index)
    tester_dad2.i2c_write_bytes(write_data)
    bytes_expect = phil.read_reg("user_reg", reg_index, size)["data"]
    write_count = phil.read_reg("i2c.w_count")["data"]
    assert write_count == size + 1
    for _ in range(size):
        assert data[_] == bytes_expect[_], \
            "index={}, data={}, expected={}".format(_, data[_],
                                                    bytes_expect[_])


@pytest.mark.parametrize("reg", [0, 1, 42, 128-16])
@pytest.mark.parametrize("size", [1, 3, 16])
def test_i2c_write_regs(phil, tester_dad2, reg, size):
    data = random.sample(range(0, 255), size)
    tester_dad2.i2c_write_regs(reg, data)
    bytes_expect = phil.read_reg("user_reg", reg, size)["data"]
    write_count = phil.read_reg("i2c.w_count")["data"]
    assert write_count == size + 1
    for _ in range(size):
        assert data[_] == bytes_expect[_], \
            "index={}, data={}, expected={}".format(_, data[_],
                                                    bytes_expect[_])


@pytest.mark.parametrize("reg", [1, 35])
@pytest.mark.parametrize("endian", [0, 1])
def test_i2c_write_regs_16_bit(phil, tester_dad2, reg, endian):
    phil.write_reg("i2c.mode.reg_16_bit", 1)
    phil.write_and_execute("i2c.mode.reg_16_big_endian", endian)
    size = 5
    data = random.sample(range(0, 255), size)

    test_reg = reg
    if not endian:
        test_reg = ((reg >> 8) & 0xFF) | (reg << 8)
    tester_dad2.i2c_write_regs(test_reg, data, reg16=True)
    bytes_expect = phil.read_reg("user_reg", reg, size)["data"]
    write_count = phil.read_reg("i2c.w_count")["data"]
    assert write_count == 2 + size
    for _ in range(size):
        assert data[_] == bytes_expect[_], \
            "index={}, data={}, expected={}".format(_, data[_],
                                                    bytes_expect[_])


def test_i2c_clk_stretch_en(phil, tester_dad2):
    reg = 42
    expected_data = phil.read_reg("user_reg", reg, 1)['data'][0]

    phil.write_and_execute("i2c.clk_stretch_delay", 0x7FFF)
    assert expected_data == tester_dad2.i2c_read_regs(reg)[0]


def test_i2c_clk_stretch_dis(phil):
    reg = 42
    tester_dad2 = DigilentAnalogDiscovery2(clk_str=0)
    tester_dad2.i2c_read_regs(reg)
    phil.write_and_execute("i2c.clk_stretch_delay", 0x7FFF)
    with pytest.raises(OSError):
        tester_dad2.i2c_read_regs(reg)


@pytest.mark.parametrize("i2c_speed", [10000, 100000, 400000])
@pytest.mark.parametrize("size", [2, 5, 8])
def test_i2c_verify_i2c_speed(phil, i2c_speed, size):
    sys_clk = phil.read_reg("sys.sys_clk")["data"]

    tester_dad2 = DigilentAnalogDiscovery2(i2c_speed=i2c_speed)

    tester_dad2.i2c_read_bytes(size)
    read_time = phil.read_reg("i2c.r_ticks")["data"]/sys_clk
    full_read_time = phil.read_reg("i2c.f_r_ticks")["data"]/sys_clk

    expected_read_time = 9/i2c_speed
    expected_full_read_time = (9 * size + 9)/i2c_speed

    byte_error = abs(read_time - expected_read_time)
    byte_error_tol = expected_read_time/2
    assert byte_error < byte_error_tol

    frame_error = abs(full_read_time - expected_full_read_time)
    frame_error_tol = expected_read_time + expected_read_time * size/8
    assert frame_error < frame_error_tol


@pytest.mark.parametrize("cmd", ["i2c.slave_addr_1", "i2c.slave_addr_2"])
@pytest.mark.parametrize("addr", [0x08, 0x09, 0x11, 0x77])
def test_i2c_addr(phil, tester_dad2, cmd, addr):
    phil.write_and_execute(cmd, addr)
    tester_dad2.i2c_read_bytes(addr=addr)

    with pytest.raises(OSError):
        tester_dad2.i2c_read_bytes(addr=0x7F)
