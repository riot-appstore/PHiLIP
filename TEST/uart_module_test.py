#!/usr/bin/python

# vim: tabstop=8 expandtab shiftwidth=4 softtabstop=4

import time
import logging
import argparse
from enum import Enum

import serial

from riot_pal import LLMemMapIf, PHILIP_MEM_MAP_PATH


class BptUartModes(Enum):
    '''BPT UART test modes.'''
    ECHO = 0
    ECHO_EXT = 1
    REG_ACCESS = 2
    TX = 3


class UartModuleTester(LLMemMapIf):
    """UART module testing class."""

    def __init__(self, port=None, dut_port=None):
        super(UartModuleTester, self).__init__(
            PHILIP_MEM_MAP_PATH, 'serial', port, 115200)
        self.dev = serial.serial_for_url(
            dut_port, baudrate=115200, timeout=1)

    def setup_dut_uart(self, baudrate=115200, databits=serial.EIGHTBITS,
                       parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
                       rtscts=False):
        """Setup DUT UART."""
        self.dev.baudrate = baudrate
        self.dev.databits = databits
        self.dev.parity = parity
        self.dev.stopbits = stopbits
        self.dev.rtscts = rtscts
        self.dev.reset_input_buffer()
        self.dev.reset_output_buffer()

    def get_bpt_uart_status(self):
        cmd_info = self.read_reg('uart.status.cts')
        if cmd_info['result'] != 'Success':
            logging.error(
                "Failed to read status: {}".format(cmd_info['result']))
            return False, 0

        return True, cmd_info['data']

    def setup_bpt(self, mode=BptUartModes.ECHO.value, baudrate=115200,
                  databits=serial.EIGHTBITS, parity=serial.PARITY_NONE,
                  stopbits=serial.STOPBITS_ONE, rts=True):

        """Setup BPT."""
        # setup testing mode
        cmd_info = self.write_reg('uart.mode', int(mode))
        if cmd_info['result'] != 'Success':
            logging.error(
                "Failed to set UART mode: {}".format(cmd_info['result']))
            return False

        # setup baudrate
        cmd_info = self.write_reg('uart.baud', int(baudrate))
        if cmd_info['result'] != 'Success':
            logging.error(
                "Failed to set UART mode: {}".format(cmd_info['result']))
            return False

        if databits == serial.SEVENBITS:
            cmd_info = self.write_reg('uart.ctrl.data_bits', 1)
            if cmd_info['result'] != 'Success':
                logging.error(
                    "Failed to set data bits: {}".format(cmd_info['result']))
                return False
        elif databits == serial.EIGHTBITS:
            cmd_info = self.write_reg('uart.ctrl.data_bits', 0)
            if cmd_info['result'] != 'Success':
                logging.error(
                    "Failed to set data bits: {}".format(cmd_info['result']))
                return False

        if parity == serial.PARITY_NONE:
            cmd_info = self.write_reg('uart.ctrl.parity', 0)
            if cmd_info['result'] != 'Success':
                logging.error(
                    "Failed to set parity: {}".format(cmd_info['result']))
                return False
        elif parity == serial.PARITY_EVEN:
            cmd_info = self.write_reg('uart.ctrl.parity', 1)
            if cmd_info['result'] != 'Success':
                logging.error(
                    "Failed to set parity: {}".format(cmd_info['result']))
                return False
        elif parity == serial.PARITY_ODD:
            cmd_info = self.write_reg('uart.ctrl.parity', 2)
            if cmd_info['result'] != 'Success':
                logging.error(
                    "Failed to set parity: {}".format(cmd_info['result']))
                return False

        if stopbits == serial.STOPBITS_ONE:
            cmd_info = self.write_reg('uart.ctrl.stop_bits', 0)
            if cmd_info['result'] != 'Success':
                logging.error(
                    "Failed to set stop bits: {}".format(cmd_info['result']))
                return False
        elif stopbits == serial.STOPBITS_TWO:
            cmd_info = self.write_reg('uart.ctrl.stop_bits', 1)
            if cmd_info['result'] != 'Success':
                logging.error(
                    "Failed to set stop bits: {}".format(cmd_info['result']))
                return False

        # invert RTS level as it is a low active signal
        if rts:
            cmd_info = self.write_reg('uart.ctrl.rts', 0)
            if cmd_info['result'] != 'Success':
                logging.error(
                    "Failed to set RTS: {}".format(cmd_info['result']))
                return False
        else:
            cmd_info = self.write_reg('uart.ctrl.rts', 1)
            if cmd_info['result'] != 'Success':
                logging.error(
                    "Failed to set RTS: {}".format(cmd_info['result']))
                return False

        # reset status
        cmd_info = self.write_reg('uart.status', 0x00)
        if cmd_info['result'] != 'Success':
            logging.error(
                "Failed to clear status: {}".format(cmd_info['result']))
            return False

        # apply changes
        cmd_info = self.execute_changes()
        if cmd_info['result'] != 'Success':
            logging.error(
                "Failed to excute changes: {}".format(cmd_info['result']))
            return False

        return True

    def echo_test(self):
        if not self.setup_bpt():
            return False

        self.setup_dut_uart()

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        return True

    def echo_test_with_hw_cf(self):
        if not self.setup_bpt(rts=True):
            return False

        self.setup_dut_uart(rtscts=True)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        return True

    def echo_test_with_hw_cf_rts_low(self):
        if not self.setup_bpt(rts=False):
            return False

        self.setup_dut_uart(rtscts=True)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            return True

        if result != test_str:
            return False

        logging.debug('Received string: {}'.format(result))

        return False

    def cts_signal_test(self):
        if not self.setup_bpt(baudrate=921600, mode=BptUartModes.TX.value):
            return False

        self.setup_dut_uart(baudrate=921600, rtscts=True)

        time.sleep(2)

        res, status = self.get_bpt_uart_status()
        if not res:
            return False

        if status != 1:
            return False

        if not self.setup_bpt():
            return False

        return True

    def echo_test_wrong_baudrate(self):
        if not self.setup_bpt(baudrate=9600):
            return False

        self.setup_dut_uart()

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.debug("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return True

        if result != test_str:
            logging.debug("Wrong string received")
            return True

        logging.debug('Received string: {}'.format(result))

        return False

    def echo_test_even_parity(self):
        if not self.setup_bpt(parity=serial.PARITY_EVEN):
            return False

        self.setup_dut_uart(parity=serial.PARITY_EVEN)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        return True

    def echo_test_odd_parity(self):
        if not self.setup_bpt(parity=serial.PARITY_ODD):
            return False

        self.setup_dut_uart(parity=serial.PARITY_ODD)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        return True

    def echo_test_even_parity_7_bit(self):
        if not self.setup_bpt(
                parity=serial.PARITY_EVEN, databits=serial.SEVENBITS):
            return False

        self.setup_dut_uart(
            parity=serial.PARITY_EVEN, databits=serial.SEVENBITS)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        return True

    def echo_test_odd_parity_7_bit(self):
        if not self.setup_bpt(
                parity=serial.PARITY_ODD, databits=serial.SEVENBITS):
            return False

        self.setup_dut_uart(
            parity=serial.PARITY_ODD, databits=serial.SEVENBITS)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        return True

    def echo_test_wrong_parity(self):
        if not self.setup_bpt(parity=serial.PARITY_EVEN):
            return False

        self.setup_dut_uart(parity=serial.PARITY_ODD)

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.debug("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return True

        if result != test_str:
            logging.debug("Wrong string received")
            return True

        logging.debug('Received string: {}'.format(result))

        return False

    def echo_test_two_stopbits(self):
        if not self.setup_bpt(baudrate=600, stopbits=serial.STOPBITS_TWO):
            return False

        self.setup_dut_uart(baudrate=600, parity=serial.PARITY_ODD)

        test_str = b'\x74\x74\x74\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != test_str:
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        return True

    def echo_ext_test(self):
        if not self.setup_bpt(mode=BptUartModes.ECHO_EXT.value):
            return False

        self.setup_dut_uart()

        test_str = b'\x30\x30\x0a'
        try:
            self.dev.write(test_str)
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if len(result) != len(test_str):
            logging.error("Received wrong character number: {} instead of {}".format(len(result), len(test_str)))
            return False

        if result != b'\x31\x31\x0a':
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        return True

    def reg_read_test(self):
        if not self.setup_bpt(mode=BptUartModes.REG_ACCESS.value):
            return False

        self.setup_dut_uart()

        test_str_wr = 'wr 1 10 1\n'
        test_str_rr = 'rr 1 1\n'
        try:
            self.dev.write(test_str_wr.encode('utf-8'))
            self.dev.write(test_str_rr.encode('utf-8'))
            result = self.dev.readline()
        except Exception as err:
            logging.error(err)
            return False

        if result != b'0,0x0A\n':
            logging.error("Wrong string received")
            return False

        logging.debug('Received string: {}'.format(result))

        return True


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--log", help='Set the logging level (DEBUG, INFO, WARN)')
    parser.add_argument("--port", help='Serial port for BPT communication')
    parser.add_argument("--dutport", help='Serial port for DUT communication')
    args = parser.parse_args()

    if args.log is not None:
        loglevel = args.log
        numeric_level = getattr(logging, loglevel.upper(), None)
        if not isinstance(numeric_level, int):
            raise ValueError('Invalid log level: %s' % loglevel)
        logging.basicConfig(level=loglevel)

    test = UartModuleTester(port=args.port, dut_port=args.dutport)

    if test.echo_test():
        logging.info("Echo test: OK")
    else:
        logging.info("Echo test: failed")

    if test.echo_test_with_hw_cf():
        logging.info("Echo test hw fc: OK")
    else:
        logging.info("Echo test hw fc: failed")

    if test.echo_test_with_hw_cf_rts_low():
        logging.info("Echo test hw fc RTS low: OK")
    else:
        logging.info("Echo test hw fc RTS low: failed")

    if test.echo_test_wrong_baudrate():
        logging.info("Echo test wrong baudrate: OK")
    else:
        logging.info("Echo test wrong baudrate: failed")

    if test.echo_test_even_parity():
        logging.info("Echo test with even parity: OK")
    else:
        logging.info("Echo test with even parity: failed")

    if test.echo_test_even_parity_7_bit():
        logging.info("Echo test with even parity 7-bit: OK")
    else:
        logging.info("Echo test with even parity 7-bit: failed")

    if test.echo_test_odd_parity():
        logging.info("Echo test with odd parity: OK")
    else:
        logging.info("Echo test with odd parity: failed")

    if test.echo_test_odd_parity_7_bit():
        logging.info("Echo test with odd parity 7-bit: OK")
    else:
        logging.info("Echo test with odd parity 7-bit: failed")

    if test.echo_test_wrong_parity():
        logging.info("Echo test with wrong parity: OK")
    else:
        logging.info("Echo test with wrong parity: failed")

    if test.echo_test_two_stopbits():
        logging.info("Echo test with two stopbits: OK")
    else:
        logging.info("Echo test with two stopbits: failed")

    if test.echo_ext_test():
        logging.info("Echo extended test: OK")
    else:
        logging.info("Echo extended test: failed")

    if test.reg_read_test():
        logging.info("Register read test: OK")
    else:
        logging.info("Register read test: failed")

    if test.cts_signal_test():
        logging.info("CTS signal test: OK")
    else:
        logging.info("CTS signal test: failed")


if __name__ == "__main__":
    main()
