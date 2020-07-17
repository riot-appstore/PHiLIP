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
import time
from pathlib import Path
from mm_pal import MmIf, import_mm_from_csv, RESULT_SUCCESS


__author__ = "Kevin Weiss"
__email__ = "weiss.kevin604@gmail.com"


class PhilipIf(MmIf):
    """Handles basic functions and commands for memory map interface

    Args:
        (*args, **kwargs) -> See base_device for documentation of args
    """

    def __init__(self, *args, **kwargs):
        self.phil_logger = logging.getLogger(self.__class__.__name__)
        super().__init__(*args, **kwargs)

        # Sometimes initial version string is incorrect so try again
        self.if_version = self.get_version()['version']

        if self.mem_map is None:
            self.phil_logger.debug("Getting mem map from version %r",
                                   self.if_version)
            version_str = self.if_version.replace('.', '_')
            rel_path = '/mem_map/PHiLIP_map_t_{}.csv'.format(version_str)
            version_path = str(Path(__file__).parents[0]) + rel_path
            self.mem_map = import_mm_from_csv(version_path)

        self._sys_clock = None

    def write_and_execute(self, cmd_name, data, timeout=None):
        """Writes the register and the init for the struct and executes changes

        Args:
            cmd_name(str): The name of the register to write
            data: The data to write to the register
            timeout: Optional timeout value for command specific timeouts
        Returns:
            see send_and_parse_cmd()
        """
        self.phil_logger.debug("write_and_execute(cmd_name=%r,data=%r,\
                               timeout=%r)", cmd_name, data, timeout)
        response = []
        # could be better, now it assumes every struct has mode.init
        if '.' in cmd_name:
            struct_name = cmd_name.split('.')[0]
            struct_name = struct_name + '.mode.init'
            try:
                response.append(self.write_reg(struct_name, 0,
                                timeout=timeout))
            except (TimeoutError):
                pass
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
        self.phil_logger.debug("dut_reset(reset_time=%r,timeout=%r)",
                               reset_time, timeout)
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
        self.phil_logger.debug("read_trace(to_ns=%r)", to_ns)
        if not self._sys_clock:
            self._sys_clock = self.read_reg('sys.sys_clk')['data']
        trace = []
        response = {"cmd": "read_trace()", "result": RESULT_SUCCESS}
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
        self.phil_logger.debug("_get_trace_events(trace=?, index=%r, "
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
