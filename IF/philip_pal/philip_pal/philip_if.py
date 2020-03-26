# Copyright (c) 2019 Kevin Weiss, for HAW Hamburg  <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the MIT License. See the
# file LICENSE in the top level directory for more details.
# SPDX-License-Identifier:    MIT
"""Interfaces for PHiLIP in philip_pal
This handles the basic protocol interface to Philip and extended helpers for
Philip.
"""
import logging
import errno
import os
import json
import csv
import time
from ctypes import c_uint8, c_uint16, c_uint32, c_int8, c_int16, c_int32
from ast import literal_eval
from pathlib import Path
from .base_device import BaseDevice


class PhilipBaseIf():
    """Handles basic functions and commands for memory map interface

    Args:
        (*args, **kwargs) -> See base_device for documentation of args
    """
    RESULT_SUCCESS = 'Success'
    RESULT_ERROR = 'Error'
    RESULT_TIMEOUT = 'Timeout'

    def __init__(self, *args, **kwargs):
        self.dev = BaseDevice(*args, **kwargs)

    @staticmethod
    def _error_msg(data):
        if data not in errno.errorcode:
            return "Unknown Error[{}]".format(data)
        s_errcode = errno.errorcode[data]
        s_errmsg = os.strerror(data)
        return "{}-{} [{}]".format(s_errcode, s_errmsg, data)

    @staticmethod
    def _bytes_to_int(data_list):
        """
        >>> PhilipBaseIf._bytes_to_int([0,1])
        256
        """
        val = 0
        try:
            for index, num in enumerate(data_list):
                if num > 255 or num < 0:
                    raise ValueError
                val += num << (index * 8)
        except (ValueError, TypeError):
            return data_list
        return val

    @staticmethod
    def _try_data_to_int(cmd_info):
        if 'data' in cmd_info:
            cmd_info['data'] = PhilipBaseIf._bytes_to_int(cmd_info['data'])

    def _send_cmd(self, send_cmd, timeout, end_key='result',):
        self.dev.writeline(send_cmd)
        cmd_info = {}
        decode_error = False
        while end_key not in cmd_info:
            line = self.dev.readline(timeout)
            try:
                cmd_info.update(json.loads(line))
            except json.decoder.JSONDecodeError:
                decode_error = True
        if decode_error:
            raise json.decoder.JSONDecodeError
        return cmd_info

    def send_and_parse_cmd(self, send_cmd, to_byte_array=False, timeout=None):
        """Returns a dictionary with information from the event
        Args:
            send_cmd(str): The command to write to the device
            to_byte_array: If True and data is bytes leave it as an array
            timeout: Optional timeout value for command specific timeouts
        Returns:
            dict:
            The return hold dict values in the following keys::
            msg - The message from the response, only used for information.
            cmd - The command sent, used to track what has occurred.
            data - Parsed information of the data requested.
            result - Either success, error or timeout.
        """

        cmd_info = {'cmd': send_cmd}
        try:
            cmd_info.update(self._send_cmd(send_cmd, timeout=timeout))
            if to_byte_array is False:
                if 'data' in cmd_info:
                    self._try_data_to_int(cmd_info)
        except json.decoder.JSONDecodeError:
            cmd_info['result'] = self.RESULT_ERROR
            cmd_info['msg'] = "Failed to parse response"
        except TimeoutError:
            cmd_info['result'] = self.RESULT_TIMEOUT
        else:
            if cmd_info['result'] == 0:
                cmd_info['result'] = self.RESULT_SUCCESS
            else:
                cmd_info['data'] = cmd_info['result']
                cmd_info['result'] = self.RESULT_ERROR
                cmd_info['msg'] = self._error_msg(cmd_info['data'])
        return cmd_info

    def read_bytes(self, index, size=1, to_byte_array=False, timeout=None):
        """Reads bytes in the register map
        Args:
            index: Index of the memory map (address or offset of bytes)
            size: The amount of bytes to read
            to_byte_array: If True and data is bytes leave it as an array
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        return self.send_and_parse_cmd('rr {} {}'.format(index, size),
                                       to_byte_array, timeout)

    def read_bits(self, index, offset, bit_amount, timeout=None):
        """Read specific bits in the register map
        Args:
            index: Index of the memory map (address or offset of bytes)
            offset: The bit offset for the bitfield
            bit_amount: The amount of bits within the bitfield
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        bit_amount = int(bit_amount)
        offset = int(offset)
        index = int(index)
        bytes_to_read = int((bit_amount - 1 + offset)/8 + 1)
        bit_mask = (2 ** bit_amount) - 1
        cmd_info = self.read_bytes(index, bytes_to_read, timeout=timeout)
        if cmd_info['result'] == self.RESULT_SUCCESS:
            cmd_info['cmd'] = ['read_bits({},{},{})'.format(index, offset,
                                                            bit_amount),
                               cmd_info['cmd']]
            cmd_info['data'] = cmd_info['data'] >> offset
            cmd_info['data'] = cmd_info['data'] & bit_mask
        return cmd_info

    @staticmethod
    def _write_byte_arg_to_string(data, size):
        ret_str = ''
        if isinstance(data, list):
            for data_byte in data:
                for i in range(0, size):
                    ret_str += ' {}'.format(
                        (int(data_byte) >> ((i) * 8)) & 0xFF)
        else:
            for i in range(0, size):
                ret_str += ' {}'.format((int(data) >> ((i) * 8)) & 0xFF)
        return ret_str

    def write_bytes(self, index, data, size=1, timeout=None):
        """Writes bytes in the register map
        Args:
            index: Index of the memory map (address or offset of bytes)
            data: The data to be converted to bytes then written to the map
            size: The amount of bytes to write, used if data is not a list
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        cmd = "wr {}{}".format(index,
                               self._write_byte_arg_to_string(data, size))
        return self.send_and_parse_cmd(cmd, timeout=timeout)

# pylint: disable=R0913
    def write_bits(self, index, offset, bit_amount, data, timeout=None):
        """Modifies specific bits in the register map
        Args:
            index: Index of the memory map (address or offset of bytes)
            offset: The bit offset for the bitfield
            bit_amount: The amount of bits within the bitfield
            data: The data to be converted to bytes then written to the map
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        cmds = ['write_bits({},{},{},{})'.format(index, offset,
                                                 bit_amount, data)]
        bit_amount = int(bit_amount)
        offset = int(offset)
        index = int(index)
        bytes_to_read = int((bit_amount - 1 + offset)/8 + 1)
        cmd_info = self.read_bytes(index, bytes_to_read, timeout=timeout)
        if cmd_info['result'] != self.RESULT_SUCCESS:
            return cmd_info
        cmds.append(cmd_info['cmd'])
        bit_mask = int((2 ** bit_amount) - 1)
        bit_mask = bit_mask << offset
        cmd_info['data'] = cmd_info['data'] & (~bit_mask)
        shifted_data = cmd_info['data'] | ((data << offset) & bit_mask)
        cmd_info = self.write_bytes(index, shifted_data,
                                    bytes_to_read, timeout=timeout)
        cmds.append(cmd_info['cmd'])
        if cmd_info['result'] == self.RESULT_SUCCESS:
            cmd_info['cmd'] = 'write_bits {} {} {} {}'.format(index, offset,
                                                              bit_amount, data)
        cmd_info['cmd'] = cmds
        return cmd_info

    def execute_changes(self, timeout=None):
        """Executes or commits device configuration changes

        This will cause any changes in configuration to be applied.  For many
        periphs the .mode.init must be set to 0 for the periph to reinitialize.

        Args:
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        return self.send_and_parse_cmd("ex", timeout=timeout)

    def reset_mcu(self, timeout=None):
        """Resets the device

        Args:
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        return self.send_and_parse_cmd("mcu_rst", timeout=timeout)

    def get_version(self, timeout=None):
        """Get the version of the interface/memory map that is being used

        Args:
            timeout: Optional timeout value for command specific timeouts
        Returns:
            returns a dict with the {version: x.x.x}
        """
        return self.send_and_parse_cmd("version", timeout=timeout)

    def get_mem_map(self, index, timeout=None):
        """Gets the memory map information from the device

        Args:
            index: The index of the memory map record
            timeout: Optional timeout value for command specific timeouts
        Returns:
            Returns a dict with a memory map map record
        """
        return self.send_and_parse_cmd("mm {}".format(index), timeout=timeout)

    def get_mem_map_size(self, timeout=None):
        """Gets the amount of records in the device memory map

        Args:
            timeout: Optional timeout value for command specific timeouts
        Returns:
            returns a dict with the {data: number of records}
        """
        return self.send_and_parse_cmd("mm_size", timeout=timeout)


class PhilipExtIf(PhilipBaseIf):
    """Handles extended commands to PHiLIP device

    This handles extended commands that involve a combination of basic
    commands.  It also handles getting the memory map of the philip device.

    Args:
        map_path - keyword arg that allows specifying a path for the
                   memory map
        use_dev_map - keyword arg that scans the device for the internal
                      memory map (it may take longer and not have as much
                      information)
        (*args, **kwargs) -> See base_device for documentation of args
    """

    def __init__(self, *args, **kwargs):
        map_path = ''
        use_dev_map = False
        if 'map_path' in kwargs:
            map_path = kwargs.pop('map_path')
        if 'use_dev_map' in kwargs:
            use_dev_map = kwargs.pop('use_dev_map')

        super().__init__(*args, **kwargs)
        try:
            self.if_version = self.get_version()['version']
        except (TimeoutError, KeyError):
            logging.debug("No device detected, functionality will be limited")
            self.if_version = "UNKNOWN"
            return

        if map_path != '':
            self.mem_map = self.import_mm_from_csv(map_path)
        elif use_dev_map:
            self.mem_map = self.scan_dev_map()
        else:
            self.mem_map = self.mm_from_version(self.if_version)

        self._sys_clock = None

    def scan_dev_map(self):
        """Scans the device for memory map information

        Returns:
            Basic memory map from the device
        """
        prim_types = {'uint8_t': 1, 'int8_t': 1, 'uint16_t': 2, 'int16_t': 2,
                      'uint32_t': 4, 'int32_t': 4, 'uint64_t': 8, 'int64_t': 8,
                      'char': 1, 'float': 4, 'double': 8}
        res = self.get_mem_map_size()
        if res['result'] == self.RESULT_ERROR:
            raise NotImplementedError("Cannot find memory map on device")
        mm_size = res['data']
        mem_map = {}
        for map_index in range(0, mm_size):
            res = self.get_mem_map(map_index)
            res['type_size'] = prim_types[res['type']]
            if res['array_size'] == 0:
                res['array_size'] = ''
                res['total_size'] = ''
            else:
                res['total_size'] = res['type_size'] * res['array_size']
            if res['bits'] == 0:
                res['bits'] = ''
                res['bit_offset'] = ''
            res.pop('cmd')
            res.pop('result')
            mem_map[res['name']] = res
        return mem_map

    @staticmethod
    def _try_parse_int_list(list_int):
        for i, val in enumerate(list_int):
            try:
                list_int[i] = literal_eval(str(val))
            except (ValueError, TypeError, SyntaxError):
                pass

    @staticmethod
    def mm_from_version(version_str):
        """Gets the saved memory map given the version
        Args:
            version_str: The version string (x.x.x)
        Returns:
            Versioned memory map
        """
        version_str = version_str.replace('.', '_')
        rel_path = '/mem_map/PHiLIP_map_t_{}.csv'.format(version_str)
        version_path = str(Path(__file__).parents[0]) + rel_path
        return PhilipExtIf.import_mm_from_csv(version_path)

    @staticmethod
    def import_mm_from_csv(path):
        """Imports a memory map csv file

        Returns:
            memory map from the CSV"""
        mem_map = {}
        with open(path) as csvfile:
            rows = list(csv.reader(csvfile, quotechar="'"))
            for row in range(1, len(rows)):
                PhilipExtIf._try_parse_int_list(rows[row])
                cmd = dict(zip(rows[0], rows[row]))
                mem_map[rows[row][rows[0].index('name')]] = cmd
                logging.debug("Imported command: %r", cmd)
        return mem_map

    @staticmethod
    def _c_cast(num, prim_type):
        if prim_type == "uint8_t":
            num = c_uint8(num).value
        if prim_type == "int8_t":
            num = c_int8(num).value
        if prim_type == "uint16_t":
            num = c_uint16(num).value
        if prim_type == "int16_t":
            num = c_int16(num).value
        if prim_type == "uint32_t":
            num = c_uint32(num).value
        if prim_type == "int32_t":
            num = c_int32(num).value
        return num

    @staticmethod
    def _parse_array(data, type_size, prim_type):
        parsed_data = []
        try:
            elements = int(len(data)/type_size)
            for i in range(0, elements):
                num = int.from_bytes(data[i*type_size:(i+1)*type_size],
                                     byteorder='little')
                parsed_data.append(PhilipExtIf._c_cast(num, prim_type))
        except (ValueError, TypeError):
            return data
        return parsed_data

# pylint: disable=R0913
    def read_reg(self, cmd_name, offset=0, size=None,
                 to_byte_array=False, timeout=None):
        """Read a register defined by the memory map

        Args:
            cmd_name(str): The name of the register to read
            offset(int): The number of elements to offset in an array
            size(int): The number of elements to read in an array
            to_byte_array: If True and data is bytes leave it as an array
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        cmd = self.mem_map[cmd_name]
        response = None
        if size is None:
            if cmd['total_size'] == '':
                size = cmd['type_size']
            else:
                size = cmd['total_size']
        else:
            if cmd['total_size'] != '':
                size = int(cmd['type_size']) * int(size)
        if cmd['bits'] != '':
            response = self.read_bits(cmd['offset'],
                                      cmd['bit_offset'],
                                      cmd['bits'],
                                      timeout=timeout)
        elif cmd['total_size'] != '':
            offset = int(offset)
            offset *= int(cmd['type_size'])
            offset += int(cmd['offset'])
            response = self.read_bytes(offset, size, True, timeout)
            response['data'] = self._parse_array(response['data'],
                                                 cmd['type_size'],
                                                 cmd['type'])
        else:
            response = self.read_bytes(cmd['offset'], size, to_byte_array)
            if 'data' in response:
                response['data'] = self._c_cast(response['data'], cmd['type'])
        response['cmd'] = ['read_reg({},{},{})'.format(cmd_name, offset, size),
                           response['cmd']]
        return response

    def write_reg(self, cmd_name, data, offset=0, timeout=None):
        """Writes a register defined by the memory map

        Args:
            cmd_name(str): The name of the register to write
            data: The data to write to the register
            offset(int): The number of elements to offset in an array
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        cmd = self.mem_map[cmd_name]
        data = literal_eval(str(data))
        response = None
        if cmd['bits'] != '':
            response = self.write_bits(cmd['offset'],
                                       cmd['bit_offset'],
                                       cmd['bits'], data)
        elif cmd['total_size'] != '':
            offset = int(offset)
            offset *= int(cmd['type_size'])
            offset += int(cmd['offset'])
            response = self.write_bytes(offset, data,
                                        int(cmd['type_size']), timeout=timeout)
        else:
            response = self.write_bytes(cmd['offset'], data,
                                        int(cmd['type_size']), timeout=timeout)
        response['cmd'] = ['write_reg({},{},{})'.format(cmd_name, data,
                                                        offset),
                           response['cmd']]
        return response

    def read_struct(self, cmd_name, data_has_name=True, timeout=None):
        """Reads a set of registers defined by the memory map

        Args:
            cmd_name(str): The name if the structure to read
            data_has_cmd: Include the record name in the data
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        response = []
        for cmd in self.mem_map.keys():
            # Suppress unused variable warning

            if cmd.startswith(cmd_name) and not cmd.endswith('.res'):
                response.append(self.read_reg(cmd, timeout=timeout))
                if data_has_name:
                    response[-1]['data'] = {cmd: response[-1]['data']}
        return response

    def write_and_execute(self, cmd_name, data, timeout=None):
        """Writes the register and the init for the struct and executes changes

        Args:
            cmd_name(str): The name of the register to write
            data: The data to write to the register
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        response = []
        # could be better, now it assumes everying struct has mode.init
        if '.' in cmd_name:
            struct_name = cmd_name.split('.')[0]
            struct_name = struct_name + '.mode.init'
            response.append(self.write_reg(struct_name, 0, timeout=timeout))
        response.append(self.write_reg(cmd_name, data, timeout=timeout))
        response.append(self.execute_changes(timeout=timeout))
        return response

    def dut_reset(self, reset_time=0.3, timeout=None):
        """Provides a reset to the dut

        Toggles the dut reset pin for the reset time.

        Args:
            reset_time: The duration the dut is put in reset
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        reset_time = float(reset_time)
        response = []
        response.extend(self.write_and_execute('sys.mode.dut_rst', 1,
                                               timeout=timeout))
        time.sleep(reset_time)
        response.extend(self.write_and_execute('sys.mode.dut_rst', 0,
                                               timeout=timeout))
        return response

    def read_trace(self, to_ns=False):
        """Reads event trace from the dut

        Returns:
            see send_and_parse_cmd()
            formatted to return the time sorted events
        """
        if not self._sys_clock:
            self._sys_clock = self.read_reg('sys.sys_clk')['data']
        trace = []
        response = {"cmd": "read_trace()", "result": self.RESULT_SUCCESS}
        total_size = int(self.mem_map['trace.tick']['array_size'])
        # must have a chunk that will not cause buffer overflow
        chunk_size = 32 + 16
        index = 0
        while index + chunk_size < total_size:
            self._get_trace_events(trace, index, chunk_size, to_ns)
            index += chunk_size
        chunk_size = total_size - index
        if chunk_size:
            self._get_trace_events(trace, index, chunk_size, to_ns)

        sorted_events = sorted(trace, key=lambda x: x['time'])
        any_diff = 0
        ev_diff = {"DEBUG0": 0, "DEBUG1": 0, "DEBUG2": 0, "DUT_IC": 0}

        for event in sorted_events:
            event["diff"] = 0
            event["source_diff"] = 0
            if any_diff != 0:
                event["diff"] = event["time"] - any_diff
            if ev_diff[event['source']] != 0:
                event["source_diff"] = event["time"] - ev_diff[event['source']]
            ev_diff[event['source']] = event["time"]
            any_diff = event["time"]

        response['data'] = sorted(trace, key=lambda x: x['time'])
        return response

    def _get_trace_events(self, trace, index, chunk_size, to_ns):
        logging.debug("_get_trace_events(trace=?, index=%r, "
                      "chunk_size=%r, to_ns=%r)",
                      index, chunk_size, to_ns)
        # should be time in seconds
        trace_tick_divs = self.read_reg('trace.tick_div',
                                        index, chunk_size)['data']
        trace_sources = self.read_reg('trace.source',
                                      index, chunk_size)['data']
        trace_ticks = self.read_reg('trace.tick',
                                    index, chunk_size)['data']
        trace_values = self.read_reg('trace.value',
                                     index, chunk_size)['data']
        for _ in range(len(trace_ticks)):
            trace_event = {}
            trace_tick_div = trace_tick_divs[_]
            trace_source = trace_sources[_]
            trace_tick = trace_ticks[_]
            trace_value = trace_values[_]
            total_tick = trace_tick << trace_tick_div
            time_sec = float(total_tick) / self._sys_clock
            if to_ns:
                trace_event['time'] = int(time_sec * 10000000000)
            else:
                trace_event['time'] = round(time_sec, 9)
            if trace_source == 1:
                trace_event['source'] = 'DEBUG0'
            elif trace_source == 2:
                trace_event['source'] = 'DEBUG1'
            elif trace_source == 3:
                trace_event['source'] = 'DEBUG2'
            elif trace_source == 4:
                trace_event['source'] = 'DUT_IC'
            else:
                trace_event['source'] = trace_source

            if trace_value == 0:
                trace_event['event'] = "FALLING"
            elif trace_value == 1:
                trace_event['event'] = "RISING"
            else:
                trace_event['event'] = trace_value
            if trace_source != 0:
                trace.append(trace_event)
