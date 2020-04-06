#! /usr/bin/env python3
"""DAC Tests

The tester is a Digilent Analog Discovery 2.
The DUT_DAC pin must be connected to a filter as it is a digital PWM out.
This can be done with a low pass filter.

Pinout:
PHiLIP      Digilent Analog Discovery 2
DUT_DAC ------------ 1+
"""
from time import sleep
import statistics as sta
import pytest


@pytest.mark.parametrize("voltage", [0, 0.1, 1, 2, 2.5, 3, 3.3])
def test_dac_acc(phil, tester_dad2, voltage):
    max_voltage = 3.3
    offset = 0.05
    drive_percent = int(voltage/max_voltage * 10000)
    phil.write_and_execute("dac.level", drive_percent)

    sleep(0.5)
    samples = []
    for _ in range(256):
        samples.append(tester_dad2.anal_sample() + offset)
    ripple = max(samples) - min(samples)
    val_error = abs(sta.mean(samples) - voltage)

    assert val_error < 0.05, "error={}, meas={}".format(val_error,
                                                        sta.mean(samples))
    assert ripple < 0.2


def main():
    """Main program"""
    print(__doc__)


if __name__ == '__main__':
    main()
