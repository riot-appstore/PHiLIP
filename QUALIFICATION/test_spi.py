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
from philip_pal import Phil
import plotly.express as px
import pandas as pd

"""
    ATTENTION!
    user_reg has 256 entries but start addresses bigger
    than 127 are not usable.
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


def speed_tolerance(speed):
    tolerance = speed * 0.1
    border = {}
    border['low'] = speed - tolerance
    border['high'] = speed + tolerance
    return border


def test_spi_freq(phil: Phil, tester_dad2):
    measurement_if_type = 4
    dut_sck_pin = tester_dad2.get_pinout()["DUT_SCK"]
    input_freq = 10000
    plot_results = []
    error_freq_flag = False
    while input_freq < 5500000:
        time_step = (1/input_freq)/2
        for run in range(8):
            # change if_type to any other type to reset measurement
            phil.write_and_execute("spi.mode.if_type", 0)
            phil.write_and_execute("spi.mode.if_type", measurement_if_type)
            tester_dad2.pulse(pin=dut_sck_pin, time_l=time_step,
                              time_h=time_step, count=64)
            results = phil.get_spi_clk_stats()
            results['input_freq'] = input_freq
            results['run'] = str(run)
            results['error'] = results['mean'] - input_freq
            results['error_per'] = results['error'] / input_freq * 100
            if input_freq <= 1000000 & int(results['error_per']) <= 3:
                error_freq_flag = True
            elif input_freq <= 5000000 & int(results['error_per']) <= 10:
                error_freq_flag = True
            plot_results.append(results)
        input_freq *= 1.05

    df = pd.DataFrame(plot_results)
    fig = px.scatter(df, x="input_freq", y="mean", color="run",
                     error_x="stdev", error_y_minus="e_minus",
                     error_y="e_plus", log_x=True, log_y=True,
                     hover_data=['values'])
    fig.show()
    fig.write_html("fig_stats.html")
    fig = px.scatter(df, x="input_freq", y="error", color="run",
                     log_x=True, hover_data=['error_per'])
    fig.write_html("fig_error.html")
    fig.show()

    assert not error_freq_flag


# speed measure is only usable for frequencies up to 5MHz
@pytest.mark.parametrize("speed", [100000, 400000, 1000000, 5000000])
def test_spi_clock_speed(phil: Phil, tester_dad2, speed):
    measurement_if_type = 4
    dut_sck_pin = tester_dad2.get_pinout()["DUT_SCK"]
    time_step = (1/speed)/2
    speed_tolerances = speed_tolerance(speed)
    for pulses in [8, 16, 64]:
        # change if_type to any other type to reset measurement
        phil.write_and_execute("spi.mode.if_type", 0)
        phil.write_and_execute("spi.mode.if_type", measurement_if_type)
        tester_dad2.pulse(pin=dut_sck_pin, time_l=time_step, time_h=time_step,
                          count=pulses, init_value=0)
        frame_stats = phil.get_spi_clk_frame_stats()
        mean_freq = (frame_stats['mean'])
        assert phil.read_reg("spi.transfer_count")['data'] == int(pulses)
        assert ((mean_freq < speed_tolerances['high']) &
                (mean_freq > speed_tolerances['low']))
        assert len(frame_stats['values']) == int(pulses / 8)


def test_spi_specific_byte(phil: Phil, tester_dad2):
    dut_sck_pin = tester_dad2.get_pinout()["DUT_SCK"]
    pulses_per_byte = 8
    measurement_if_type = 4
    phil.write_and_execute("spi.mode.if_type", measurement_if_type)

    speed1 = 100000
    time_step1 = (1/speed1)/2
    tester_dad2.pulse(pin=dut_sck_pin, time_l=time_step1, time_h=time_step1,
                      count=pulses_per_byte, init_value=0)

    speed2 = 400000
    time_step2 = (1/speed2)/2
    tester_dad2.pulse(pin=dut_sck_pin, time_l=time_step2, time_h=time_step2,
                      count=pulses_per_byte, init_value=0)

    byte_stats1 = phil.get_spi_clk_byte_stats(byte=0)
    assert len(byte_stats1['values']) == pulses_per_byte - 1
    assert byte_stats1['mean'] == speed1

    byte_stats2 = phil.get_spi_clk_byte_stats(byte=1)
    assert len(byte_stats2['values']) == pulses_per_byte - 1
    assert byte_stats2['mean'] == speed2


def test_spi_deadtime(phil: Phil, tester_dad2):
    measurement_if_type = 4
    speed = 100000
    dut_sck_pin = tester_dad2.get_pinout()["DUT_SCK"]
    time_step = (1/speed)/2
    pulses = 8
    phil.write_and_execute("spi.mode.if_type", measurement_if_type)
    tester_dad2.pulse(pin=dut_sck_pin, time_l=time_step, time_h=time_step,
                      count=pulses, init_value=0)
    tester_dad2.pulse(pin=dut_sck_pin, time_l=time_step, time_h=time_step,
                      count=pulses, init_value=0)
    assert phil.read_reg("spi.transfer_count")['data'] == 2 * pulses
    deadtime = phil.get_spi_clk_deadtime_stats()
    assert len(deadtime['values']) == 1
    assert deadtime['mean'] > int(phil.sys_clk() / speed)


@pytest.mark.parametrize("bytes", [3, 4, 8])
def test_spi_multiple_deadtimes(phil: Phil, tester_dad2, bytes):
    pulses_per_byte = 8
    measurement_if_type = 4
    speed = 100000
    dut_sck_pin = tester_dad2.get_pinout()["DUT_SCK"]
    time_step = (1/speed)/2
    phil.write_and_execute("spi.mode.if_type", measurement_if_type)
    for i in range(bytes):
        tester_dad2.pulse(pin=dut_sck_pin, time_l=time_step, time_h=time_step,
                          count=pulses_per_byte, init_value=0)
    assert (phil.read_reg("spi.transfer_count")['data'] ==
            bytes * pulses_per_byte)
    deadtime = phil.get_spi_clk_deadtime_stats()
    assert len(deadtime['values']) == bytes - 1
    for i in range(len(deadtime['values'])):
        assert deadtime['values'][i] > int(phil.sys_clk() / speed)


def main():
    """Main program"""
    print(__doc__)


if __name__ == '__main__':
    main()
