"""
Tests for the extended PHiLIP interface
"""
from conftest import _regtest


def test_read_reg(phil_ex, regtest):
    """Tests the read reg function on all valid registers in the map"""
    for cmd in sorted(phil_ex.mem_map):
        read_val = phil_ex.read_reg(cmd)
        read_val.pop('cmd', None)
        read_val['name'] = cmd
        if 'DEVICE_SPECIFIC' in phil_ex.mem_map[cmd]['flag'] or \
                cmd.endswith(".res") or \
                'VOLATILE' in phil_ex.mem_map[cmd]['flag']:
            read_val.pop('data', None)
        elif phil_ex.mem_map[cmd]['default']:
            assert read_val['data'] == int(phil_ex.mem_map[cmd]['default'])
        _regtest(regtest, read_val)


def test_philip_ext_write_reg(phil_ex):
    """Tests the extended register write on valid registers with access"""
    for cmd in sorted(phil_ex.mem_map):
        r_data = phil_ex.read_reg(cmd)
        if 'data' not in r_data:
            continue
        r_data = r_data['data']
        if isinstance(r_data, list) or \
                'DEVICE_SPECIFIC' in phil_ex.mem_map[cmd]['flag'] or \
                'VOLATILE' in phil_ex.mem_map[cmd]['flag'] or \
                cmd.endswith(".res") or \
                phil_ex.mem_map[cmd]['access'] == 0:
            continue
        if r_data != 0:
            continue
        phil_ex.write_reg(cmd, 1)
        assert phil_ex.read_reg(cmd)['data'] == 1, "cmd={}".format(cmd)
