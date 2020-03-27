"""Timing Tests

Note: The pins must be connected for the test to work
The tester is a Digilent Analog Discovery 2
Pinout:
PHiLIP      Digilent Analog Discovery 2
DUT_IC ------------ 0
DEBUG0 ------------ 1
DEBUG1 ------------ 2
DEBUG2 ------------ 3
"""
from time import sleep
import statistics as sta
import pytest


@pytest.mark.parametrize("pulse_time", [0.000001, 0.00001, 0.001])
@pytest.mark.parametrize("events", [1, 16, 128])
def test_pulse_dut_ic_both_acc(phil, tester_dad2, pulse_time, events):
    avg_time = _dut_ic_acc_check(phil, tester_dad2, pulse_time, 0, events)
    error = abs(avg_time - pulse_time)
    assert error < 0.0000002, "accuracy error: {}".format(error)


@pytest.mark.parametrize("pulse_time", [0.000001, 0.00001])
def test_pulse_dut_ic_both_jitter(phil, tester_dad2, pulse_time):
    jitter = _dut_ic_jitter_check(phil, tester_dad2, pulse_time, 0)
    assert jitter < 0.0000002, "jitter error: {}".format(jitter)


@pytest.mark.parametrize("pulse_time", [0.0000002, 0.000001, 0.00001, 0.001])
@pytest.mark.parametrize("trig_edge", [1, 2])
@pytest.mark.parametrize("events", [2, 16, 128])
def test_pulse_dut_ic_rise_fall_acc(phil, tester_dad2, pulse_time,
                                    trig_edge, events):
    avg_time = _dut_ic_acc_check(phil, tester_dad2, pulse_time/2,
                                 trig_edge, events)
    error = abs(avg_time - pulse_time)
    assert error < 0.000000028, "accuracy error: {}".format(error)


@pytest.mark.parametrize("pulse_time", [0.0000002, 0.000001, 0.00001])
@pytest.mark.parametrize("trig_edge", [1, 2])
def test_pulse_dut_ic_rise_fall_jitter(phil, tester_dad2,
                                       pulse_time, trig_edge):
    jitter = _dut_ic_jitter_check(phil, tester_dad2, pulse_time, trig_edge)
    assert jitter < 0.000000028, "jitter error: {}".format(jitter)


@pytest.mark.parametrize("gpio_num", [0, 1, 2])
@pytest.mark.parametrize("pulse_time", [0.00001, 0.001])
@pytest.mark.parametrize("events", [1, 128, 1024])
def test_pulse_gpio(phil, tester_dad2, gpio_num, pulse_time, events):
    phil.write_reg("gpio[{}].mode.pull".format(gpio_num), 1)
    phil.write_and_execute("gpio[{}].mode.io_type".format(gpio_num), 3)
    sleep(0.01)
    tester_dad2.pulse("DEBUG{}".format(gpio_num), pulse_time, count=events)
    sleep(0.01)
    trace = phil.read_trace()["data"]
    diffs = []
    index = 0
    for event in trace:
        if index != 0:
            diffs.append(event['source_diff'])
        assert event['source'] == "DEBUG{}".format(gpio_num)
        if index & 1:
            assert event['event'] == "RISING", \
                "event={}, expected={}, index={}".format(event['event'],
                                                         "RISING", index)
        else:
            assert event['event'] == "FALLING", \
                "event={}, expected={}, index={}".format(event['event'],
                                                         "FALLING", index)
        index += 1
    jitter = max(diffs) - min(diffs)
    error = abs(sta.mean(diffs) - pulse_time)
    assert error < 0.0000006, "accuracy error: {}\n{}".format(error, diffs)
    assert jitter < 0.0000006, "jitter error: {}\n{}".format(jitter, diffs)


def _dut_ic_acc_check(phil, tester_dad2, pulse_time, trig_edge, events):
    avg_diff = []
    phil.write_and_execute("tmr.mode.trig_edge", trig_edge)
    sleep(0.01)
    tester_dad2.pulse("DUT_IC", pulse_time, count=events)
    sleep(0.01)
    trace = phil.read_trace()["data"]
    index = 0
    for event in trace:
        if index != 0:
            avg_diff.append(event['source_diff'])
        assert event['source'] == "DUT_IC"
        if trig_edge == 1:
            assert event['event'] == "RISING"
        elif trig_edge == 2:
            assert event['event'] == "FALLING"
        elif index & 1:
            assert event['event'] == "RISING"
        else:
            assert event['event'] == "FALLING"
        index += 1
    return sta.mean(avg_diff)


def _dut_ic_jitter_check(phil, tester_dad2, pulse_time, trig_edge):
    sys_clk = phil.read_reg("sys.sys_clk")["data"]
    jitter = []
    for _ in range(128):
        if trig_edge != 0:
            phil.write_and_execute("tmr.mode.trig_edge", trig_edge)
            sleep(0.01)
        tester_dad2.pulse("DUT_IC", pulse_time, count=128)
        sleep(0.01)

        max_time = phil.read_reg("tmr.max_tick")["data"] / sys_clk
        min_time = phil.read_reg("tmr.min_tick")["data"] / sys_clk
        jitter.append(abs(max_time - min_time))
        phil.reset_mcu()

    return max(jitter)
