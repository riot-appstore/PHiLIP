#! /usr/bin/env python3
"""UART qualification tests

The tester is a UART to Serial converter such as FTDI.

Pinout:
PHiLIP
DUT_RST ────┐
DUT_IC  ────┘
"""


def test_dut_rst(phil):
    """Toggles the DUT_RST pin and confirms with a PU"""
    phil.dut_reset(1)
    trace = phil.read_trace()['data']
    elapse_time = trace[1]['time'] - trace[0]['time']
    assert elapse_time > 0.9 and elapse_time < 1.1


def main():
    """Main program"""
    print(__doc__)


if __name__ == '__main__':
    main()
