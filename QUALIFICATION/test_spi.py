"""SPI Tests

The tester is a Digilent Analog Discovery 2

Pinout:
PHiLIP      Digilent Analog Discovery 2
DUT_MOSI ------------ 6
DUT_MISO ------------ 7
DUT_SCK  ------------ 8
DUT_NSS  ------------ 9
"""
import random
import pytest
from digilent_device import DigilentAnalogDiscovery2

"""
    ATTENTION!
    user_reg has 256 entries but start adresses bigger
    than 127 are not useable.
    When writing from an index bigger 127 nothing happens.
    When reading from an index bigger 127 then 127=0, 128=1,...
    It's still possible to read and write beyond 127 if startadress
    is 127 or lower.
"""

@pytest.mark.parametrize("reg", [0, 1, 42, 127])
@pytest.mark.parametrize("size", [1, 3, 16, 128])
def test_spi_read_regs(phil, tester_dad2, reg, size):
    bytes_read = tester_dad2.spi_read_regs(reg, size)
    bytes_expect = phil.read_reg("user_reg", reg, size)["data"]
    read_count = phil.read_reg("spi.r_count")["data"]
    transfer_count = phil.read_reg("spi.transfer_count")["data"]
    assert read_count == size
    assert transfer_count == size + 1
    for _ in range(size):
        assert bytes_read[_] == bytes_expect[_]


@pytest.mark.parametrize("size", [1, 3, 16, 128])
def test_spi_write_bytes(phil, tester_dad2, size):
    data = random.sample(range(0, 255), size)
    reg_index = 0
    write_data = data.copy()
    # Add register number to write to make deterministic
    write_data.insert(0, reg_index)
    tester_dad2.spi_write_bytes(write_data)
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
def test_spi_write_regs(phil, tester_dad2, reg, size):
    data = random.sample(range(0, 255), size)
    tester_dad2.spi_write_regs(reg, data)
    bytes_expect = phil.read_reg("user_reg", reg, size)["data"]
    write_count = phil.read_reg("spi.w_count")["data"]
    transfer_count = phil.read_reg("spi.transfer_count")["data"]
    assert write_count == size
    assert transfer_count == size + 1
    for _ in range(size):
        assert data[_] == bytes_expect[_], \
            "index={}, data={}, expected={}".format(_, data[_],
                                                    bytes_expect[_])


@pytest.mark.parametrize("reg", [0, 41])
@pytest.mark.parametrize("size", [1, 15])
@pytest.mark.parametrize("pol", [0, 1])
@pytest.mark.parametrize("pha", [0, 1])
def test_spi_if_type_0_all_modes(phil, reg, size, pol, pha):
    phil.write_reg("spi.mode.cpol", pol)
    phil.write_and_execute("spi.mode.cpha", pha)
    tester_dad2 = DigilentAnalogDiscovery2(clk_pol=pol, clk_pha=pha)
    write_data = random.sample(range(0, 255), size)
    tester_dad2.spi_write_regs(reg, write_data)
    read_data = tester_dad2.spi_read_regs(reg, size)
    assert write_data == read_data


def main():
    """Main program"""
    print(__doc__)


if __name__ == '__main__':
    main()
