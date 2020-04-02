"""SPI Tests

Note: The pins must be connected for the test to work
The tester is a Digilent Digital Discovery
Pinout:
PHiLIP      Digilent Digital Discovery
DUT_MOSI ------------ 27
DUT_MISO ------------ 28
DUT_SCK  ------------ 29
DUT_NSS  ------------ 30
"""
import random
import pytest
from digilent_device import DigilentDigitalDiscovery


"""
    ATTENTION!
    user_reg has 256 entries but start adresses bigger
    than 127 are not useable. 
    When writing from an index bigger 127 nothing happens.
    When reading from an index bigger 127 then 127=0, 128=1,...
    It's still possible to read and write beyond 127 if startadress
    is 127 or lower.
"""


""" 
TODO:    spi_read_bytes not working?

@pytest.mark.parametrize("size", [1, 3, 16, 128])
def test_spi_read_bytes(phil, tester_dd, size):
    bytes_read = tester_dd.spi_read_bytes(size)
    bytes_expect = phil.read_reg("user_reg", 0, size)["data"]
    read_count = phil.read_reg("spi.r_count")["data"]
    assert read_count == size
    for _ in range(size):
        assert bytes_read[_] == bytes_expect[_]
"""


@pytest.mark.parametrize("reg", [0, 1, 42, 127])
@pytest.mark.parametrize("size", [1, 3, 16, 128])
def test_spi_read_regs(phil, tester_dd, reg, size):
    bytes_read = tester_dd.spi_read_regs(reg, size)
    bytes_expect = phil.read_reg("user_reg", reg, size)["data"]
    read_count = phil.read_reg("spi.r_count")["data"]
    transfer_count = phil.read_reg("spi.transfer_count")["data"]
    assert read_count == size
    assert transfer_count == size + 1
    for _ in range(size):
        assert bytes_read[_] == bytes_expect[_]


@pytest.mark.parametrize("size", [1, 3, 16, 128])
def test_spi_write_bytes(phil, tester_dd, size):
    data = random.sample(range(0, 255), size)
    reg_index = 0
    write_data = data.copy()
    # Add register number to write to make deterministic
    write_data.insert(0, reg_index)
    tester_dd.spi_write_bytes(write_data)
    bytes_expect = phil.read_reg("user_reg", reg_index, size)["data"]
    write_count = phil.read_reg("spi.w_count")["data"]
    transfer_count = phil.read_reg("spi.transfer_count")["data"]
    assert write_count == size
    assert transfer_count == size + 1
    for _ in range(size):
        assert data[_] == bytes_expect[_], \
            "index={}, data={}, expected={}".format(_, data[_],
                                                    bytes_expect[_])


@pytest.mark.parametrize("reg", [0, 1, 42, 127])
@pytest.mark.parametrize("size", [1, 3, 16, 128])
def test_spi_write_regs(phil, tester_dd, reg, size):
    data = random.sample(range(0, 255), size)
    tester_dd.spi_write_regs(reg, data)
    bytes_expect = phil.read_reg("user_reg", reg, size)["data"]
    write_count = phil.read_reg("spi.w_count")["data"]
    transfer_count = phil.read_reg("spi.transfer_count")["data"]
    assert write_count == size
    assert transfer_count == size + 1
    for _ in range(size):
        assert data[_] == bytes_expect[_], \
            "index={}, data={}, expected={}".format(_, data[_],
                                                    bytes_expect[_])


@pytest.mark.parametrize("reg", [0, 1, 42, 127])
@pytest.mark.parametrize("size", [1, 3, 16])
@pytest.mark.parametrize("mode", [0, 1, 2, 3])
def test_spi_if_type_0_all_modes(phil, tester_dd, reg, size, mode):
    #setup if_type 0
    #type 0 is default
    assert (phil.read_reg("spi.mode.if_type")["data"]) == 0
    phil.write_and_execute("spi.dut_nss.pull", 1)
    #setup modes
    clk_pol = (mode & (1 << 1)) >> 1
    clk_pha = mode & 1
    phil.write_and_execute("spi.mode.cpol", clk_pol)
    phil.write_and_execute("spi.mode.cpha", clk_pha)
    tester_dd.driver.spi_setup(tester_dd.pins["DUT_MOSI"],
                                tester_dd.pins["DUT_MISO"], 
                                tester_dd.pins["DUT_SCK"],
                                clk_pol=clk_pol, clk_pha=clk_pha)
    assert clk_pol == phil.read_reg("spi.mode.cpol")["data"]
    assert clk_pha == phil.read_reg("spi.mode.cpha")["data"]
    #read and write
    write_data = random.sample(range(0, 255), size)
    tester_dd.spi_write_regs(reg, write_data)
    read_data = tester_dd.spi_read_regs(reg, size)
    assert write_data == read_data


@pytest.mark.parametrize("size", [1, 3, 16, 128, 255])
@pytest.mark.parametrize("mode", [0, 1, 2, 3])
def test_spi_if_type_1_all_modes(phil, tester_dd, size, mode):
    #setup if_type 1
    phil.write_and_execute("spi.mode.if_type", 1)
    assert (phil.read_reg("spi.mode.if_type")["data"]) == 1
    #setup modes
    clk_pol = (mode & (1 << 1)) >> 1
    clk_pha = mode & 1
    phil.write_and_execute("spi.mode.cpol", clk_pol)
    phil.write_and_execute("spi.mode.cpha", clk_pha)
    tester_dd.driver.spi_setup(tester_dd.pins["DUT_MOSI"],
                                tester_dd.pins["DUT_MISO"], 
                                tester_dd.pins["DUT_SCK"],
                                clk_pol=clk_pol, clk_pha=clk_pha)
    assert clk_pol == phil.read_reg("spi.mode.cpol")["data"]
    assert clk_pha == phil.read_reg("spi.mode.cpha")["data"]
    #read and write
    reg_index = 0
    write_data = random.sample(range(0, 255), size)
    read_data = tester_dd.spi_xfer(write_data)
    bytes_expect = phil.read_reg("user_reg", reg_index, size)["data"]
    for _ in range(size):
        assert write_data == bytes_expect, \
            "index={}, data={}, expected={}".format(_, write_data[_],
                                                    bytes_expect[_])


@pytest.mark.parametrize("size", [1, 3, 16, 128, 255])
@pytest.mark.parametrize("mode", [0, 1, 2, 3])
def test_spi_if_type_2_all_modes(phil, tester_dd, size, mode):
    #setup if_type 2
    phil.write_and_execute("spi.mode.if_type", 2)
    assert (phil.read_reg("spi.mode.if_type")["data"]) == 2
    #setup modes
    clk_pol = (mode & (1 << 1)) >> 1
    clk_pha = mode & 1
    phil.write_and_execute("spi.mode.cpol", clk_pol)
    phil.write_and_execute("spi.mode.cpha", clk_pha)
    tester_dd.driver.spi_setup(tester_dd.pins["DUT_MOSI"],
                                tester_dd.pins["DUT_MISO"], 
                                tester_dd.pins["DUT_SCK"],
                                clk_pol=clk_pol, clk_pha=clk_pha)
    assert clk_pol == phil.read_reg("spi.mode.cpol")["data"]
    assert clk_pha == phil.read_reg("spi.mode.cpha")["data"]
    #read and write
    dummy = 255
    write_data = random.sample(range(0, 255), size)
    write_data.insert(size+1, dummy)
    read_data = tester_dd.spi_xfer(write_data)[1:]
    assert read_data == write_data[:size]