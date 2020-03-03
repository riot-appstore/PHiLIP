# Copyright (c) 2018 Kevin Weiss, for HAW Hamburg  <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the MIT License. See the
# file LICENSE in the top level directory for more details.
# SPDX-License-Identifier:    MIT
"""Serial Driver for philip_pal
This module handles generic connection and IO to the serial driver.
"""
import logging
import time
from serial import Serial, serial_for_url, SerialException
from serial.tools import list_ports


class SerialDriver:
    """Serial Port Driver

    Contains all reusable functions for connecting, sending and receiving
    data.  Arguments are passed through to the standard pyserial driver.  The
    defaults are changed.  Also if env variables are defined they get used as
    defaults.  Automatically opens the serial port on initialize.  If nothing
    is specified the port is set to the first available port.

    Args:
        (*args, **kwargs) -> See pyserial for documentation of args
    """
    DEFAULT_TIMEOUT = 0.5
    DEFAULT_BAUDRATE = 115200

    def __init__(self, *args, **kwargs):
        self._connect(*args, **kwargs)
        # Used to clear the cpu and mcu buffer from startup junk data
        time.sleep(0.05)
        self.writeline('')
        try:
            self.readline(0.3)
        except TimeoutError:
            pass

    def _connect(self, *args, **kwargs):
        if 'timeout' not in kwargs:
            kwargs['timeout'] = self.DEFAULT_TIMEOUT
        if len(args) < 2:
            if 'baudrate' not in kwargs:
                kwargs['baudrate'] = self.DEFAULT_BAUDRATE
        if len(args) == 0:
            if 'port' not in kwargs:
                kwargs['port'] = list_ports.comports()[0][0]
        logging.debug("Serial connection args %r -- %r", args, kwargs)
        try:
            self._dev = Serial(*args, **kwargs)
        except SerialException:
            self._dev = serial_for_url(*args, **kwargs)
        self._args = args
        self._kwargs = kwargs

    def open(self, *args, **kwargs):
        """Open a Serial Connection

        Args:
            (*args, **kwargs) -> See pyserial for documentation of args
        """
        logging.debug("Opening port")
        self._connect(*args, **kwargs)

    def close(self):
        """Close serial connection

        """
        logging.debug("Closing %s", self._dev.port)
        self._dev.close()

    def readline(self, timeout=None):
        """Read Line from Serial

        Read and decode to utf-8 data from the serial port.  Raises
        TimeoutError exception if an nothing is read.

        Args:
            timeout: Optional timeout value for command specific timeouts
        Returns:
            str: string of utf8 encoded data
        """
        if timeout is None:
            res_bytes = self._dev.readline()
        else:
            default_timeout = self._dev.timeout
            self._dev.timeout = timeout
            res_bytes = self._dev.readline()
            self._dev.timeout = default_timeout
        response = res_bytes.decode("utf-8", errors="replace")
        # Gets rid of leading zeros caused by reset
        response = response.strip('\0')
        if response == "":
            # try to reconnect if timeout occurs
            self.close()
            self._connect(*self._args, **self._kwargs)
            raise TimeoutError("Timeout during serial readline")
        logging.debug("Response: %s", response.replace('\n', ''))
        return response

    def writeline(self, line):
        """Write Line to Serial
        Writes line to the serial port and adds a newline and encode to utf-8.

        Args:
            line(str): string or list of bytes to send to the driver.
        """
        # Clear the input buffer in case it junk data go in creating an offset
        self._dev.reset_input_buffer()
        logging.debug("Sending: " + line)
        self._dev.write((line + '\n').encode('utf-8'))
