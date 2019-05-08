"""Conf Test for pytest

Declares fixtures and common functions
"""
import pytest
import serial
from philip_pal import Phil


def pytest_addoption(parser):
    """Adds options like arg parse"""
    parser.addoption("--philip_port", action="store",
                     default="/dev/ttyACM0", help="The serial port for PHiLIP")
    parser.addoption("--tester_port", action="store",
                     default="/dev/ttyUSB0",
                     help="The serial port for the testing device")


@pytest.fixture(scope="module")
def phil_init(request):
    """Instance for the philip connection"""
    phil = Phil(request.config.getoption("--philip_port"))
    yield phil
    phil.reset_mcu()
    phil.dev.close()


@pytest.fixture(scope="function")
def phil(phil_init):
    """Instance for the philip connection, provides reset"""
    # Provide extra reset since a test may fail assert and need to resync
    result = phil_init.RESULT_TIMEOUT
    for attempts in range(1, 3):
        result = phil_init.reset_mcu(timeout=(attempts * 0.5))['result']
        if result == phil_init.RESULT_SUCCESS:
            break
    assert result == phil_init.RESULT_SUCCESS
    yield phil_init


@pytest.fixture(scope="function")
def tester(request):
    """Instance for the serial to usb tester"""
    dev = serial.Serial(port=request.config.getoption("--tester_port"),
                        baudrate=115200, timeout=0.3, write_timeout=1)
    yield dev
    dev.close()
