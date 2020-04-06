"""
Tests for the basic PHiLIP interface
"""
import pytest
from conftest import _regtest


def test_send_and_parse_cmd(phil_base, regtest):
    """Tests basic send and parse command"""
    _regtest(regtest, phil_base.send_and_parse_cmd("rr 0 10"))
    _regtest(regtest, phil_base.send_and_parse_cmd("rr 0 2"))
    _regtest(regtest, phil_base.send_and_parse_cmd("rr 0 2", True))
    _regtest(regtest, phil_base.send_and_parse_cmd("rr 0 2", True, 0.1))
    _regtest(regtest, phil_base.send_and_parse_cmd("help"))
    _regtest(regtest, phil_base.get_version())


def test_read_bytes_success(phil_base, regtest):
    """Tests basic read bytes command"""
    _regtest(regtest, phil_base.read_bytes(0))
    _regtest(regtest, phil_base.read_bytes("1"))
    _regtest(regtest, phil_base.read_bytes(3, 2))
    _regtest(regtest, phil_base.read_bytes(4, "0"))
    _regtest(regtest, phil_base.read_bytes(5, 10))
    _regtest(regtest, phil_base.read_bytes(5, 8))
    _regtest(regtest, phil_base.read_bytes(5, 8, True))
    _regtest(regtest, phil_base.read_bytes(5, 8, True, 0.1))


def test_read_bytes_fail(phil_base):
    """Tests failure cases of read bytes"""
    assert phil_base.read_bytes(99999)['result'] == phil_base.RESULT_ERROR
    assert phil_base.read_bytes(0, 99999)['result'] == phil_base.RESULT_ERROR


def test_write_bytes_success(phil_base, regtest):
    """Tests write bytes command"""
    _regtest(regtest, phil_base.write_bytes(0, [9, 8, 7]))
    _regtest(regtest, phil_base.write_bytes(3, 99))
    _regtest(regtest, phil_base.write_bytes(7, "88"))
    _regtest(regtest, phil_base.read_bytes(0, 8, True))
    _regtest(regtest, phil_base.read_bytes(0, 8, True, 0.1))


def test_write_bytes_fail(phil_base):
    """Tests failure cases of write bytes command"""
    assert phil_base.write_bytes(99999, 0)['result'] == phil_base.RESULT_ERROR
    errl = [0] * 512
    assert phil_base.write_bytes(0, errl)['result'] == phil_base.RESULT_ERROR
    with pytest.raises(ValueError):
        phil_base.write_bytes(0, 'xxx')


def test_read_bits_success(phil_base, regtest):
    """Tests various read bits and compares with regession"""
    phil_base.write_bytes(0, [0x91, 0x1F, 0, 0, 0, 0, 0, 1])
    _regtest(regtest, phil_base.read_bits(0, 0, 1))
    _regtest(regtest, phil_base.read_bits(0, 0, 1, 0.1))
    _regtest(regtest, phil_base.read_bits(0, 0, 5))
    _regtest(regtest, phil_base.read_bits(0, 4, 1))
    _regtest(regtest, phil_base.read_bits(0, 4, 4))
    _regtest(regtest, phil_base.read_bits(0, 7, 9))
    _regtest(regtest, phil_base.read_bits(0, 0, 7))
    _regtest(regtest, phil_base.read_bits(0, 0, 8))
    _regtest(regtest, phil_base.read_bits(0, 0, 9))
    _regtest(regtest, phil_base.read_bits(0, 8, 1))
    _regtest(regtest, phil_base.read_bits(0, 8, 8))
    _regtest(regtest, phil_base.read_bits(0, 0, 16))
    _regtest(regtest, phil_base.read_bits(0, 0, 64))


def _assert_bits(phil, offset, bits, data):
    w_data = phil.write_bits(0, offset, bits, data)
    r_data = phil.read_bits(0, offset, bits)
    assert w_data['result'] == phil.RESULT_SUCCESS
    assert r_data['result'] == phil.RESULT_SUCCESS
    assert r_data['data'] == data


def test_write_bits_success(phil_base, regtest):
    """Test write bytes by writing user reg then comaparing it to a read value
    """
    _regtest(regtest, phil_base.write_bits(0, 0, 9, 0x102, 0.1))
    _assert_bits(phil_base, 0, 1, 1)
    _assert_bits(phil_base, 1, 2, 2)
    _assert_bits(phil_base, 2, 2, 3)
    _assert_bits(phil_base, 3, 15, 32000)


def test_base_functions(phil_base):
    """Tests if misc commands pass"""
    assert phil_base.execute_changes()['result'] == phil_base.RESULT_SUCCESS
    assert phil_base.reset_mcu()['result'] == phil_base.RESULT_SUCCESS
    assert phil_base.get_version()['version']
