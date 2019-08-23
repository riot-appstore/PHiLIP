"""
Tests for the extended PHiLIP interface
"""
from copy import deepcopy
from conftest import _regtest


def test_compare_scan_and_csv(phil_ex):
    """Tests the on board memory map against the CSV import"""
    ptypes = ['uint8_t', 'int8_t', 'uint16_t', 'int16_t', 'uint32_t',
              'int32_t', 'uint64_t', 'int64_t', 'char', 'float', 'double']
    scan_map = phil_ex.scan_dev_map()
    csv_map = deepcopy(phil_ex.mem_map)
    total_list = list(scan_map.keys())
    total_list.extend(list(csv_map.keys()))
    total_set = set(total_list)
    for cmd in sorted(total_set):
        scan = scan_map[cmd]
        csvm = csv_map[cmd]
        if csvm['type'] not in ptypes:
            csvm.pop('type')
            scan.pop('type')
        for record_type in scan:
            assert scan[record_type] == csvm[record_type]


def test_read_reg(phil_ex, regtest):
    """Tests the read reg function on all valid registers in the map"""
    for cmd in sorted(phil_ex.mem_map):
        read_val = phil_ex.read_reg(cmd)
        read_val.pop('cmd', None)
        read_val['name'] = cmd
        if 'DEVICE_SPECIFIC' in phil_ex.mem_map[cmd]['flag'] or \
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
                phil_ex.mem_map[cmd]['access'] == 0:
            continue
        if r_data != 0:
            continue
        phil_ex.write_reg(cmd, 1)
        assert phil_ex.read_reg(cmd)['data'] == 1, "cmd={}".format(cmd)


def test_philip_ext_misc_commands(phil_ex, regtest):
    """This tests the remaining commands

    Note: Connect the dut_rst to DEBUG0 pin to test traces
    """
    _regtest(regtest, phil_ex.read_struct("trace"))
    phil_ex.write_and_execute('gpio[0].mode.io_type', 3)
    phil_ex.dut_reset(0.3)
    traces = phil_ex.read_trace()
    if len(traces) == 0:
        raise IndexError("Could not get traces, connect DUT_RST to DEBUG0")
    sec_diff = traces['data'][1]['time'] - traces['data'][0]['time']
    assert sec_diff > 0.1 and sec_diff < 0.5
