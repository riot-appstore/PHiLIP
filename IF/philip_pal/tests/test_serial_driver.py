"""Tests Serial Driver for philip pal

    This test should be run with only one PHiLIP device plugged in.
"""
import threading
from serial import SerialException
from serial.tools import list_ports
import pytest
from philip_pal.serial_driver import SerialDriver


WORKING_PORT = list_ports.comports()[0][0]
WORKING_BAUD = 115200
FAILING_PORT = 'ERROR'
FAILING_BAUD = 19200


def test_init_success():
    """Test working serial connections."""
    test_port = WORKING_PORT
    test_baud = WORKING_BAUD
    SerialDriver()
    SerialDriver(test_port)
    SerialDriver(port=test_port)
    SerialDriver(test_port, test_baud)
    SerialDriver(test_port, baudrate=test_baud)
    SerialDriver(baudrate=test_baud, port=test_port, timeout=2)


def test_read_write_success():
    """Test proper write and read response."""
    ser_drvr = SerialDriver()
    ser_drvr.writeline('')
    ser_drvr.readline()
    ser_drvr.close()


def test_read_timeout():
    """Test getting a timeout if cannot readline."""
    ser_drvr = SerialDriver()
    with pytest.raises(TimeoutError):
        ser_drvr.readline()


def test_read_timeout_change_greater():
    """Test getting a timeout if cannot readline."""
    ser_drvr = SerialDriver()

    tmr = threading.Timer(1.5, ser_drvr.writeline, [""])
    tmr.start()
    ser_drvr.readline(timeout=2)
    tmr.cancel()


def test_read_timeout_change_lower():
    ser_drvr = SerialDriver()

    tmr = threading.Timer(0.25, ser_drvr.writeline, [""])
    tmr.start()
    with pytest.raises(TimeoutError):
        ser_drvr.readline(timeout=0.1)
    tmr.cancel()


def test_init_fail():
    """Test failing arguments for serial connections."""
    test_port = FAILING_PORT
    test_baud = FAILING_BAUD
    with pytest.raises(SerialException):
        SerialDriver(test_port)
    with pytest.raises(TypeError):
        SerialDriver(port=test_port)
    with pytest.raises(SerialException):
        SerialDriver(test_port, test_baud)
    with pytest.raises(SerialException):
        SerialDriver(test_port, baudrate=test_baud)
    with pytest.raises(TypeError):
        SerialDriver(baudrate=test_baud, port=test_port, timeout=2)


def test_open_close_success():
    """Tests that the port can be opened properly"""
    ser_drvr = SerialDriver()
    ser_drvr.close()
    ser_drvr.open()
    ser_drvr.close()
