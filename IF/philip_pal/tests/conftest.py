"""Conf Test for pytest

Declares fixtures and common functions
"""
from pprint import pformat
import pytest
from philip_pal.philip_if import PhilipBaseIf
from philip_pal.philip_if import PhilipExtIf


def _try_reset(phil):
    result = phil.RESULT_TIMEOUT
    for attempts in range(1, 3):
        result = phil.reset_mcu(timeout=(attempts * 0.5))['result']
        if result == phil.RESULT_SUCCESS:
            break
    assert result == phil.RESULT_SUCCESS


@pytest.fixture(scope="module")
def phil_init_base():
    phil = PhilipBaseIf()
    yield phil
    phil.dev.close()


@pytest.fixture(scope="function")
def phil_base(phil_init_base):
    # Provide extra reset since a test may fail assert and need to resync
    _try_reset(phil_init_base)
    yield phil_init_base


@pytest.fixture(scope="module")
def phil_init_ex():
    phil = PhilipExtIf()
    yield phil
    phil.dev.close()


@pytest.fixture(scope="function")
def phil_ex(phil_init_ex):
    # Provide extra reset since a test may fail assert and need to resync
    _try_reset(phil_init_ex)
    yield phil_init_ex


def _regtest(regtest, w_obj):
    regtest.write(pformat(w_obj) + '\n')
