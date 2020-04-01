"""Tests Static Methods for the PHiLIP inteface for philip pal

"""
import pytest
from pprint import pformat
from philip_pal.philip_if import PhilipBaseIf
from philip_pal.philip_if import PhilipExtIf

BYTE_LIST_DATA = [([0], 0),
                  ([1], 1),
                  ([2], 2),
                  (0, 0),
                  (1, 1),
                  ([255], 0xFF),
                  ([0, 1], 0x100),
                  ([255, 255], 0xFFFF),
                  ([0, 0, 1], 0x10000),
                  ([255, 255, 255], 0xFFFFFF),
                  ([0, 0, 0, 1], 0x1000000),
                  ([0, 0, 0, 0, 1], 0x100000000),
                  ([0, 0, 0, 0, 0, 1], 0x10000000000),
                  ([0, 0, 0, 0, 0, 0, 1], 0x1000000000000),
                  ([0, 0, 0, 0, 0, 0, 0, 1], 0x100000000000000),
                  ([0, 0, 0, 0, 0, 0, 0, 0], 0),
                  ([1, 0, 0, 0, 0, 0, 0, 0], 1),
                  ([1, 0, 0, 0], 1)]


@pytest.mark.parametrize("byte_list, expected", BYTE_LIST_DATA)
def test_bytes_to_int(byte_list, expected):
    """Tests byte list to int"""
    assert PhilipBaseIf()._bytes_to_int(byte_list) == expected


@pytest.mark.parametrize("byte_list, expected", BYTE_LIST_DATA)
def test_try_data_to_int(byte_list, expected):
    """Tests turning data from cmd_info to int"""
    cmd_info = {'data': byte_list}
    PhilipBaseIf()._try_data_to_int(cmd_info)
    assert cmd_info['data'] == expected


def test_write_byte_arg_to_string(regtest):
    """Tests writing different varients of bytes to the write byte cmd"""
    args_list = [(0, 1),
                 (0, 2),
                 (0, 3),
                 (0, 4),
                 (1, 4),
                 (0x100, 4),
                 (0x10000, 4),
                 (0x1000000, 4),
                 (0x100000000, 5),
                 (0x10000000000, 6),
                 (0x1000000000000, 7),
                 (0x100000000000000, 8),
                 ([1, 2, 3, 4], 1),
                 ([1, 2], 1),
                 ([1, 2, 3], 1),
                 ([1, 2, 3, 4, 5], 1),
                 ([1, 2, 3, 4, 6, 7, 8, 9, 10], 1),
                 ([1, 0, 0, 0], 1),
                 ([0x123], 1),
                 ([0x123], 2),
                 ([-1, -2], 1),
                 ([-1, -2], 2)]
    for args in args_list:
        result = PhilipBaseIf()._write_byte_arg_to_string(*args)
        regtest.write('data={}, size={}, result="{}"\n'.format(args[0],
                                                               args[1],
                                                               result))


@pytest.mark.parametrize("version_str", ['1.0.2'])
def test_mm_from_version(regtest, version_str):
    """Regression tests for all version of mem maps"""
    regtest.write(pformat(PhilipExtIf().mm_from_version(version_str)))


@pytest.mark.parametrize("data, type_size, prim_type, expected", [
    ([0], 1, "uint8_t", [0]),
    ([0, 0, 0, 0], 2, "uint16_t", [0, 0]),
    ([1], 1, "uint8_t", [1]),
    ([0, 1], 2, "uint16_t", [0x0100]),
    ([0, 1, 1, 1], 2, "uint16_t", [0x0100, 0x0101]),
    ([0, 0, 0, 1, 1, 0, 0, 1], 4, "uint32_t", [0x01000000, 0x01000001]),
    ([1, 2, 3, 4, 5, 6, 7, 8], 8, "uint64_t", [0x0807060504030201])])
def test_parse_array(data, type_size, prim_type, expected):
    """Tests parsing an array with various sizes"""
    assert PhilipExtIf()._parse_array(data, type_size, prim_type) == expected
