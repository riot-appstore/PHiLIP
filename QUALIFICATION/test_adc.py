#! /usr/bin/env python3
"""ADC Tests

Note: The pins must be connected for the test to work
The tester is a Digilent Analog Discovery 2
Pinout:
PHiLIP      Digilent Analog Discovery 2
DUT_ADC ------------ W1
"""
from time import sleep
import statistics as sta
import pytest


@pytest.mark.parametrize("fast_sample", [0, 1])
@pytest.mark.parametrize("voltage", [0.0001, 0.02, 0.1, 1, 2, 2.5, 3, 3.2, 3.3])
def test_anal_acc(phil, tester_dad2, fast_sample, voltage):
    smp_cnt = phil.read_reg("adc.num_of_samples")["data"]
    max_voltage = 3.3
    max_adc_val = 4096 * smp_cnt
    samples = []
    for _ in range(32):
        if fast_sample:
            phil.write_and_execute("adc.mode.fast_sample", fast_sample)
        tester_dad2.anal_output_volts(voltage)
        sleep(0.5)
        sample = phil.read_reg("adc.sum")["data"]
        samples.append(sample * max_voltage / max_adc_val)
        phil.reset_mcu()
    cal_jitter = max(samples) - min(samples)
    val_error = abs(sta.mean(samples) - voltage)
    per_error = abs(val_error / voltage) * 100

    assert (val_error < 0.025 or per_error < 5), "error={}, meas={}".format(val_error,
                                                         sta.mean(samples))
    assert cal_jitter < 0.025


def main():
    """Main program"""
    print(__doc__)


if __name__ == '__main__':
    main()
