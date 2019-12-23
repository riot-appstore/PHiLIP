# Copyright (c) 2018 Kevin Weiss, for HAW Hamburg  <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the MIT License. See the
# file LICENSE in the top level directory for more details.
# SPDX-License-Identifier:    MIT
"""Abstract Base Device for philip_pal
This module is the abstract device that interfaces to a driver.  It handles the
selection and initialization of the driver.

Example:
    class ApplicationDevice(BaseDevice):
        def show_example(self):
            self._write('Send Example')
            print(self._read())

    example_use_case = ApplicationDevice(driver_type='serial',
                                         port='my/port/name')
    example_use_case.show_example()
"""
from .serial_driver import SerialDriver


class BaseDevice:
    """Instance for devices to connect and utilize drivers.

    Args:
        driver_type(str): Selects the driver that is used on the devices.
            'serial' uses the standard serial port, all following arguments
            get passed through.
        (*args, **kwargs) -> See selected driver_type for documentation of args
    """

    def __init__(self, *args, **kwargs):
        self._driver = self._driver_from_config(*args, **kwargs)

    def open(self, *args, **kwargs):
        """Opens the device connection."""
        self._driver.open(*args, **kwargs)

    def close(self):
        """Closes the device connection."""
        self._driver.close()

    def readline(self, timeout=None):
        """Reads data from the driver.

        Args:
            timeout: Optional timeout value for command specific timeouts
        Returns:
            str: string of data if success, driver defined error if failed.
        """
        if timeout is None:
            return self._driver.readline()
        return self._driver.readline(float(timeout))

    def writeline(self, line):
        """Writes data to the driver.

        Args:
            line(str): Variable length argument list.
        """
        return self._driver.writeline(line)

    @staticmethod
    def _driver_from_config(*args, **kwargs):
        """Returns driver instance given configuration"""
        if 'driver_type' in kwargs:
            driver_type = kwargs.pop('driver_type')
            if driver_type == 'serial':
                return SerialDriver(*args, **kwargs)
            raise NotImplementedError()
        else:
            return SerialDriver(*args, **kwargs)
