#! /usr/bin/env python3
"""UART qualification tests

The tester is a UART to Serial converter such as FTDI.

Pinout:
PHiLIP      USB to Serial Converter
DUT_RX ------------ TX
DUT_TX ------------ RX
DUT_CTS ----------- RTS
DUT_RTS ----------- CTS
"""
import threading
import time
import pytest
import serial


def _read_reg_success(phil, reg_name):
    result = phil.read_reg(reg_name)
    assert result['result'] == phil.RESULT_SUCCESS
    return result['data']


def _write_reg_success(phil, reg_name, reg_val):
    assert phil.write_reg(reg_name, reg_val)['result'] == phil.RESULT_SUCCESS


def _commit_uart(phil):
    assert phil.write_reg('uart.mode.init', 0)['result'] == phil.RESULT_SUCCESS
    assert phil.execute_changes()['result'] == phil.RESULT_SUCCESS


# pylint: disable=R0913
def _setup_phil(phil, baudrate=115200, bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
                rtscts=True, if_type=0):
    if rtscts:
        _write_reg_success(phil, 'uart.mode.rts', 0)
    else:
        _write_reg_success(phil, 'uart.mode.rts', 1)
    if parity == serial.PARITY_EVEN:
        _write_reg_success(phil, 'uart.mode.parity', 1)
    elif parity == serial.PARITY_ODD:
        _write_reg_success(phil, 'uart.mode.parity', 2)
    if bytesize == serial.SEVENBITS:
        _write_reg_success(phil, 'uart.mode.data_bits', 1)
    if stopbits == serial.STOPBITS_TWO:
        _write_reg_success(phil, 'uart.mode.stop_bits', 1)
    _write_reg_success(phil, 'uart.baud', baudrate)
    _write_reg_success(phil, 'uart.mode.if_type', if_type)

    _commit_uart(phil)


def _setup_tester(tester, **kwargs):
    port = tester.port
    if 'baudrate' not in kwargs:
        kwargs['baudrate'] = 115200
    tester = serial.Serial(port, timeout=0.3, write_timeout=1, **kwargs)
    tester.reset_input_buffer()
    tester.reset_output_buffer()


@pytest.mark.parametrize("data_set", [b'\x30\x30\x0a',
                                      b'\x0a',
                                      b'\x42'*50 + b'\x0a'])
@pytest.mark.parametrize("kwargs", [
    {},
    {"rtscts": True},
    {"parity": serial.PARITY_EVEN},
    {"parity": serial.PARITY_ODD},
    {"bytesize": serial.SEVENBITS, "parity": serial.PARITY_EVEN},
    {"bytesize": serial.SEVENBITS, "parity": serial.PARITY_ODD},
    {"stopbits": serial.STOPBITS_TWO}])
@pytest.mark.parametrize("baudrate", [9600, 19200, 115200, 921600])
def test_echo(phil, tester, data_set, kwargs, baudrate):
    """Tests basic echo in different modes"""
    _setup_phil(phil, baudrate=baudrate, **kwargs)
    _setup_tester(tester, baudrate=baudrate, **kwargs)

    tester.write(data_set)
    assert tester.readline() == data_set
    assert _read_reg_success(phil, "uart.rx_count") == len(data_set)
    assert _read_reg_success(phil, "uart.tx_count") == len(data_set)


def test_cts_signal(phil, tester):
    """Tests the clear to send signal can get triggered when data flooded"""
    _setup_phil(phil, baudrate=921600, if_type=3)
    _setup_tester(tester, baudrate=921600, rtscts=True)
    assert _read_reg_success(phil, 'uart.status.cts') == 0
    # Fill up buffer until cts is high
    time.sleep(0.5)
    assert _read_reg_success(phil, 'uart.status.cts') == 1


def test_rts_signal(phil, tester):
    """Tests ready to send signal prevents hardware from sending"""
    _setup_phil(phil, rtscts=False)
    _setup_tester(tester, rtscts=True)

    data_set = b'\x30\x30\x0a'
    tmr = threading.Timer(0.25, _setup_phil, [phil], {"rtscts": True})
    tmr.start()
    start_time = time.time()
    tester.write(data_set)
    line = tester.readline()
    tmr.cancel()
    assert line == data_set
    elapse_time = time.time() - start_time
    assert elapse_time < 0.3 and elapse_time > 0.2


@pytest.mark.parametrize("baudrate", [9600, 19200, 921600])
def test_wrong_baud(phil, tester, baudrate):
    """Tests mismatch baudrate expect failure"""
    _setup_phil(phil, baudrate=baudrate)
    data_set = b'\x20\x21\x22\x23\x24\x25\x0a'
    tester.write(data_set)
    line = tester.readline()
    assert data_set != line
    if baudrate > 115200:
        assert _read_reg_success(phil, "uart.status.fe") == 1


@pytest.mark.parametrize("parity", [serial.PARITY_EVEN, serial.PARITY_ODD])
def test_parity_error(phil, tester, parity):
    """Tests incorrect parity settings trigger errors"""
    _setup_phil(phil, parity=parity)
    data_set = b'\x20\x21\x22\x23\x24\x25\x0a'
    tester.write(data_set)
    line = tester.readline()
    assert data_set != line
    assert _read_reg_success(phil, "uart.status.pe") == 1


def test_two_stopbits(phil, tester):
    """Tests two stopbits are sensed
    This requires special settings since a delay could look like 2 stop bits"""
    _setup_phil(phil, stopbits=serial.STOPBITS_TWO)
    _setup_tester(tester, parity=serial.PARITY_ODD)
    data_set = b'\x74\x74\x74\x0a'
    tester.write(data_set)
    line = tester.readline()
    assert data_set == line


def test_ext_echo(phil, tester):
    """Tests extended echo mode that adds one to the data received"""
    _setup_phil(phil, if_type=1)
    data_set = b'\x30\x30\x0a'
    tester.write(data_set)
    line = tester.readline()
    assert line == b'\x31\x31\x0a'


def test_reg(phil, tester):
    """Tests read reg mode can write and read to registers"""
    _setup_phil(phil, if_type=2)
    test_str_wr = 'wr 1 42\n'
    test_str_rr = 'rr 1 1\n'
    tester.write(test_str_wr.encode('utf-8'))
    assert tester.readline() == b'{"result":0}\n'
    tester.write(test_str_rr.encode('utf-8'))
    assert tester.readline() == b'{"data":[42], "result":0}\n'


def main():
    """Main program"""
    print(__doc__)


if __name__ == '__main__':
    main()
