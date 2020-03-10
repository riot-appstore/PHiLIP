"""
Tests for the extended PHiLIP interface
"""
from time import sleep

GPIO_NAME_LIST = [
    {"tmr": ["dut_ic"]},
    {"adc": ["dut_adc"]},
    {"i2c": ["dut_sda", "dut_scl"]},
    {"pwm": ["dut_pwm"]},
    {"dac": ["dut_dac"]},
    {"spi": ["dut_mosi", "dut_miso", "dut_sck", "dut_nss"]},
    {"uart": ["dut_rx", "dut_tx", "dut_cts", "dut_rts"]}]


def test_all_gpio_input_hi(phil_ex):
    for gpio in GPIO_NAME_LIST:
        k, pins = next(iter(gpio.items()))
        res = phil_ex.write_and_execute('{}.mode.disable'.format(k), 1)
        print(res)
        for pin in pins:

            res = phil_ex.write_and_execute('{}.{}.pull'.format(k, pin), 1)
            for result in res:
                assert result['result'] == phil_ex.RESULT_SUCCESS, \
                    "{}.{}.pull failed to change".format(k, pin)
            sleep(0.3)
            res = phil_ex.read_reg('{}.{}.level'.format(k, pin))
            print(res)
            assert res['data'] == int(1), \
                "{}.{}.level failed to be pulled up".format(k, pin)


def test_all_gpio_input_lo(phil_ex):
    for gpio in GPIO_NAME_LIST:
        k, pins = next(iter(gpio.items()))
        res = phil_ex.write_and_execute('{}.mode.disable'.format(k), 1)
        print(res)
        for pin in pins:
            res = phil_ex.write_and_execute('{}.{}.pull'.format(k, pin), 2)
            for result in res:
                assert result['result'] == phil_ex.RESULT_SUCCESS, \
                    "{}.{}.pull failed to change".format(k, pin)
            sleep(0.3)
            res = phil_ex.read_reg('{}.{}.level'.format(k, pin))
            print(res)
            assert res['data'] == int(0), \
                "{}.{}.level failed to be pulled down".format(k, pin)


def test_all_gpio_output_hi(phil_ex):
    for gpio in GPIO_NAME_LIST:
        k, pins = next(iter(gpio.items()))
        res = phil_ex.write_and_execute('{}.mode.disable'.format(k), 1)
        print(res)
        for pin in pins:
            res = phil_ex.write_and_execute('{}.{}.io_type'.format(k, pin), 1)
            for result in res:
                assert result['result'] == phil_ex.RESULT_SUCCESS, \
                    "{}.{}.io_type failed to change".format(k, pin)
            res = phil_ex.write_and_execute(
                '{}.{}.set_level'.format(k, pin), 1)
            for result in res:
                assert result['result'] == phil_ex.RESULT_SUCCESS, \
                    "{}.{}.set_level failed to change".format(k, pin)
            sleep(0.3)
            res = phil_ex.read_reg('{}.{}.level'.format(k, pin))
            print(res)
            assert res['data'] == int(1), \
                "{}.{}.level failed to be pulled down".format(k, pin)


def test_all_gpio_output_lo(phil_ex):
    for gpio in GPIO_NAME_LIST:
        k, pins = next(iter(gpio.items()))
        res = phil_ex.write_and_execute('{}.mode.disable'.format(k), 1)
        print(res)
        for pin in pins:
            res = phil_ex.write_and_execute('{}.{}.io_type'.format(k, pin), 1)
            for result in res:
                assert result['result'] == phil_ex.RESULT_SUCCESS, \
                    "{}.{}.io_type failed to change".format(k, pin)
            res = phil_ex.write_and_execute(
                '{}.{}.set_level'.format(k, pin), 0)
            for result in res:
                assert result['result'] == phil_ex.RESULT_SUCCESS, \
                    "{}.{}.set_level failed to change".format(k, pin)
            sleep(0.3)
            res = phil_ex.read_reg('{}.{}.level'.format(k, pin))
            print(res)
            assert res['data'] == int(0), \
                "{}.{}.level failed to be pulled down".format(k, pin)
