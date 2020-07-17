#! /usr/bin/env python3
# Copyright (c) 2019 Kevin Weiss, for HAW Hamburg  <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the MIT License. See the
# file LICENSE in the top level directory for more details.
# SPDX-License-Identifier:    MIT

"""
This script handles interfacing to the PHiLIP device.  It parses data and
exposes from the PhilipExtIf class to be run in a shell.

The purpose of this script is allow easy setup and manual usage of PHiLIP.

Usage
-----

```
usage: philip_shell.py  [-h]
                        [--loglevel {debug,info,warning,error,fatal,critical}]
                        [--port PORT]
                        [--filter-data]

optional arguments:
  -h, --help            show this help message and exit
  --loglevel {debug,info,warning,error,fatal,critical}
                        Python logger log level (default: warning)
  --port, -p
                        Specify the serial port
  --data_only, -do
                        Filters data from philip responses to only display
                        what is needed (default: False)
```
"""
import os
import logging
import argparse
try:
    import readline
except ImportError:
    readline = None
    logging.warning("readline package could not be found!")
    logging.warning("History will not be available.")
import statistics as sta
from tabulate import tabulate
from philip_pal.philip_if import PhilipIf
from mm_pal import MmCmd, serial_connect_wizard, write_history_file


_HISTFILE = os.path.join(os.path.expanduser("~"), ".philip_history")
__author__ = "Kevin Weiss"
__email__ = "weiss.kevin604@gmail.com"


class PhilipCli(MmCmd):
    """Command loop for the PHiLIP interface

    Args:
        port - Serial port for the PHiLIP, if None connection wizard tries to
               connent
    """
    prompt = 'PHiLIP: '

    def __init__(self, **kwargs):
        self.phil_logger = logging.getLogger(self.__class__.__name__)
        if 'dev_driver' in kwargs:
            super().__init__(kwargs['dev_driver'])
            return
        port = kwargs.get('port', None)
        if port is None:
            super().__init__(serial_connect_wizard(PhilipIf, **kwargs),
                             history_file=_HISTFILE)
        else:
            super().__init__(PhilipIf(**kwargs), history_file=_HISTFILE)
        self.logger.debug("__init__(%r)", kwargs)

    def do_write_and_execute(self, arg):
        """Writes the register and the init for the struct and executes changes

        Usage:
            write_and_execute <cmd_name> <data> [timeout]

        Args:
            cmd_name: The name of the register to write
            data: The data to write to the register
            timeout: Optional timeout value for command specific timeouts
        """
        self._print_func_result(self.dev_driver.write_and_execute, arg)

    def complete_write_and_execute(self, text, line, begidx, endidx):
        """Completes arg with memory map record names"""
        begidx = begidx
        endidx = endidx
        return self._complete_map(text, line)

    def do_dut_reset(self, arg):
        """Provides a reset to the dut

        Toggles the dut reset pin for the reset time.

        Usage:
            dut_reset [reset_time] [timeout]

        Args:
            reset_time: Optional duration the dut is put in reset
            timeout: Optional timeout value for command specific timeouts
        """
        self._print_func_result(self.dev_driver.dut_reset, arg)

    def do_read_trace(self, arg):
        """Reads event trace from the dut

        Usage:
            read_trace
        """
        try:
            results = self.dev_driver.read_trace()
        except KeyError as exc:
            print('Could not parse argument {}'.format(exc))
        except (TypeError, ValueError, SyntaxError) as exc:
            print(exc)
        else:
            if len(results) == 0:
                return
            headers = ['time', 'diff', 'source_diff', 'source', 'event']
            table_data = []
            diffs = []
            for event in results["data"]:
                row_data = []
                for key_name in headers:
                    if key_name == 'diff':
                        diffs.append(event[key_name])
                    row_data.append(event[key_name])
                table_data.append(row_data)
            print(tabulate(table_data, headers=headers, floatfmt=".9f"))

            try:
                if len(diffs) > 1:
                    diffs = diffs[1:]
                    print("\nDifference Stats")
                    print("     min: {:.9f}".format(min(diffs)))
                    print("     max: {:.9f}".format(max(diffs)))
                    print("    mean: {:.9f}".format(sta.mean(diffs)))
                    print("  median: {:.9f}".format(sta.median(diffs)))
                    print("   stdev: {:.9f}".format(sta.stdev(diffs)))
                    print("variance: {:.9f}".format(sta.variance(diffs)))
            except ValueError:
                pass

    def do_show_pinout(self, arg):
        """Prints the pinout for the connected board

        Usage:
            show_pinout
        """
        try:
            if arg:
                showboard = int(arg)
            else:
                showboard = self.dev_driver.read_reg('sys.status.board')['data']
            if showboard == 1:
                print("""
PHILIP-B -> BLUEPILL

                    ____
                 ___|__|___
 DUT_RST = B12 - |        | - GND
 DUT_CTS = B13 - |        | - GND
 DUT_RTS = B14 - |        | - 3V3
USER_BTN = B15 - |        | - NRST
   DUT_IC = A8 - |        | - B11 = DUT_RX
    IF_TX = A9 - |        | - B10 = DUT_TX
   IF_RX = A10 - |        | - B1 = PM_V_ADC
  USB_DM = A11 - |        | - B0 = PM_HI_ADC
  USB_DP = A12 - |        | - A7 = PM_LO_ADC
 DUT_NSS = A15 - |        | - A6 = DUT_ADC
  DUT_SCK = B3 - |        | - A5 = TEST_FAIL
 DUT_MISO = B4 - |        | - A4 = TEST_WARN
 DUT_MOSI = B5 - |        | - A3 = TEST_PASS
  DUT_SCL = B6 - |        | - A2 = DEBUG2
  DUT_SDA = B7 - |        | - A1 = DEBUG1
  DUT_PWM = B8 - |        | - A0 = DEBUG0
  DUT_DAC = B9 - |        | - C15
            5V - |        | - C14
           GND - |        | - C13 = LED0
           3V3 - |        | - VBAT
                 __________
                    ||||
""")
            else:
                print("""
PHILIP-N -> NUCLEO-F103RB
CN6

                            DUT_SCL = PB8 = SCL/D15 -
                            DUT_SDA = PB9 = SDA/D14 -
                                               AVDD -
                                                GND -
-                              LED0 = PA5 = SCK/D13 -
- IOREF                                    MISO/D12 -
- NRST                                 PWM/MOSI/D11 -
- 3V3                                    PWM/CS/D10 -
- 5V                                         PWM/D9 -
- GND                             DUT_TX = PA9 = D8 -
- GND                                           |CN5|
- VIN                             DUT_IC = PA8 = D7 -
|CN6|                                        PWM/D6 -
- A0 = PA0 = TEST_WARN        DEBUG1 = PB4 = PWM/D5 -
- A1 = PA1 = TEST_FAIL            DEBUG2 = PB5 = D4 -
- A2 = PA4 = TEST_PASS        DEBUG0 = PB3 = PWM/D3 -
- A3 = PB0 = DUT_ADC             DUT_RX = PA10 = D2 -
- A4 = PC1 = PM_HI_ADC          IF_TX = PA2 = TX/D1 -
- A5 = PC0 = PM_V_ADC           IF_RX = PA3 = RX/D0 -
|CN8|                                          |CN9|

          -1 -                  DUT_DAC -1 - DUT_PWM
          -2 - DEBUG2           DUT_SCL -2 -
          -3 -                  DUT_SDA -3 -
          -4 -                          -4 -
          -5 -                          -5 -
          -6 -                     LED0 -6 - DUT_RTS
          -7 -                          -7 - DUT_CTS
          -8 -                          -8 - DUT_NSS
          -9 -                          -9 -
          -10-                          -10-
          -11-                   DUT_TX -11- DUT_RST
 USER_BTN -12-                   DUT_IC -12-
          -13-                          -13- DUT_MOSI
          -14- TEST_WARN         DEBUG1 -14- DUT_MISO
          -15- TEST_FAIL                -15- DUT_SCK
          -16- TEST_PASS         DEBUG0 -16-
          -17- DUT_ADC           DUT_RX -17-
PM_LO_ADC -18- PM_HI_ADC          IF_TX -18-
          -19- PM_V_ADC           IF_RX -19-
          |CN7|                         |CN10|
""")
        except (ValueError) as exc:
            print(exc)


def _exit_cmd_loop():
    write_history_file(_HISTFILE)


def log_level_module_control(pargs):
    """Enable logs depending on modules.

    Args:
        pargs: arguments from argparse
    """
    if pargs.loglevel:
        loglevel = logging.getLevelName(pargs.loglevel.upper())
        if pargs.logmodules is not None:
            logging.basicConfig()
            for logname in pargs.logmodules:
                logger = logging.getLogger(logname)
                logger.setLevel(loglevel)
        else:
            logging.basicConfig(level=loglevel)

def main():
    """Main program"""
    parser = argparse.ArgumentParser()

    parser.add_argument('--loglevel', default='INFO',
                        help='Python logger log level, defaults to INFO.')
    parser.add_argument('--logmodules', nargs='+', default=None,
                        help='Modules to enable logging.')
    parser.add_argument('--port', '-p', default=None,
                        help='Serial device name, defaults to None.')
    parser.add_argument('--mm_path', default=None,
                        help='Path to memory map, defaults to None.')
    pargs = parser.parse_args()
    log_level_module_control(pargs)
    try:
        PhilipCli(port=pargs.port).cmdloop()
        _exit_cmd_loop()
    except KeyboardInterrupt:
        _exit_cmd_loop()
        print("")


if __name__ == '__main__':
    main()
