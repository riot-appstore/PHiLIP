from riot_pal.ll_mem_map_if import LLMemMapIf
from pprint import pformat



def _try_parse_int(val):
    try:
        return int(val, 0)
    except (ValueError, TypeError):
        return val


def try_add(val):
    """Attempts to add a number to a value.

    Args:
        val(int, str, list): Value to add the number to.

    Return:
        int: If successful returns the val + 1.  If failed just retruns the val.
    """
    try:
        return _try_parse_int(val)+1
    except TypeError:
        try:
            val[0] = _try_parse_int(val[0])+1
            return val
        except TypeError:
            return val



def test_philip_mem_map_read_reg(x):
    """Data driven test for all possible PHiLIP read reg commands."""
    print(pformat(x.reset_mcu()) + '\n')

    for cmd in sorted(x.cmd_list):
        cmd_vals = x.cmd_list[cmd]
        read_val = x.read_reg(cmd, to_byte_array=True)
        print(pformat(read_val) + '\n')



def test_philip_mem_map_write_reg(x):
    """Data driven test for all possible PHiLIP write reg commands.  Also attempts to
    change data by adding 1.
    """
    print(pformat(x.reset_mcu()) + '\n')

    for cmd in sorted(x.cmd_list):
        cmd_vals = x.cmd_list[cmd]
        read_val = x.read_reg(cmd)
        write_ret = x.write_reg(cmd, try_add(read_val['data']))
        read_val = x.read_reg(cmd)
        print(pformat(write_ret) + '\n')
        print(pformat(read_val) + '\n')







def _parse_to_type_array(data, type_size):
    parsed_data = []

    elements = int(len(data)/type_size)
    for i in range(0, elements):
        parsed_data.append(int.from_bytes(data[i:(i+1)*type_size],
                                          byteorder='big'))

    return parsed_data


#x = LLMemMapIf("map_t.csv")
#print(x.send_cmd("rr 0 2"))
data = b'\x00\x01'
print(int.from_bytes(data[0:(1+1)*2],byteorder='little'))
print([1, 0])
x = LLMemMapIf("map_t.csv")
#print(x.send_cmd("wr 440 1"))

test_philip_mem_map_read_reg(x)
print("===================================================================")
print("---------------------- WRITE TEST ---------------------------------")
print("===================================================================")
test_philip_mem_map_write_reg(x)
print("===================================================================")
print("---------------------- TEST ---------------------------------")
print("===================================================================")
print(pformat(x.write_reg('user_reg', 1, offset=0)))
print(pformat(x.read_reg('user_reg', size=32)))
x.cmd_list['user_reg']['type_size'] = 2
print(pformat(x.write_reg('user_reg', [1], offset=1)))
print(pformat(x.read_reg('user_reg', size=32)))
x.close()
