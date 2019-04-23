"""Tests Base Driver for philip pal

    This test should be run with only one PHiLIP device plugged in.
"""
import pytest
from philip_pal.base_device import BaseDevice

DEVICE_DRIVERS = ['serial']


def test_init_success():
    """Test working connections."""
    BaseDevice()
    BaseDevice(driver_type='serial')


def test_init_fail():
    """Test failing connections."""
    with pytest.raises(NotImplementedError):
        BaseDevice(driver_type='fail')


@pytest.mark.parametrize("driver_type_arg", DEVICE_DRIVERS)
def test_read_write_success(driver_type_arg):
    """Test proper write and read response."""
    dev = BaseDevice(driver_type=driver_type_arg)
    dev.writeline('')
    dev.readline()
    dev.close()


@pytest.mark.parametrize("driver_type_arg", DEVICE_DRIVERS)
def test_read_timeout(driver_type_arg):
    """Test getting a timeout if cannot readline."""
    dev = BaseDevice(driver_type=driver_type_arg)
    with pytest.raises(TimeoutError):
        dev.readline()


@pytest.mark.parametrize("driver_type_arg", DEVICE_DRIVERS)
def test_open_close_success(driver_type_arg):
    """Tests that the port can be opened properly"""
    ser_drvr = BaseDevice(driver_type=driver_type_arg)
    ser_drvr.close()
    ser_drvr.open()
    ser_drvr.close()
