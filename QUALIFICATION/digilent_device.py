"""Digilent Driver

Python wrapper for the Digilent Waveforms software for devices such as Digilent
Analog Discovery 2.
"""
from ctypes import c_uint, c_int, c_double, byref, c_uint8, c_bool
from ctypes import create_string_buffer, cdll
from time import sleep
import sys
import logging


class DigilentDriver():
    def __init__(self, *args, **kwargs):
        """Driver for basic interface for Diglent devices"""
        self.logger = logging.getLogger(self.__class__.__name__)
        self._get_driver(*args, **kwargs)
        self.i2c_speed = None

    def __del__(self):
        self.inst.FDwfDeviceClose(self.handle)

    def _get_driver(self, handle=-1):
        if sys.platform.startswith("win"):
            self.inst = cdll.dwf
        elif sys.platform.startswith("darwin"):
            lib_path = "/Library/Frameworks/dwf.framework/dwf"
            self.inst = cdll.LoadLibrary(lib_path)
        else:
            self.inst = cdll.LoadLibrary("libdwf.so")

        self.inst.FDwfDeviceCloseAll()
        version = create_string_buffer(16)
        self.version = self.inst.FDwfGetVersion(version)

        self.logger.debug("DWF Version: %r", version.value)
        self.handle = c_int()
        self.inst.FDwfDeviceOpen(c_int(handle), byref(self.handle))
        if self.handle.value == 0:
            self.logger.debug("Failed to open Digilent Device")
            szerr = create_string_buffer(512)
            self.inst.FDwfGetLastErrorMsg(szerr)
            raise ConnectionError(self.inst.FDwfGetLastErrorMsg(szerr))
        self.do_clk = c_double()
        self.inst.FDwfDigitalOutInternalClockInfo(self.handle,
                                                  byref(self.do_clk))
        self.logger.debug("do_clk=%r", self.do_clk)

    def _s_tick(self, secs):
        return c_uint(int(self.do_clk.value*secs))

    def pulse_pin(self, pin, time_l, time_h, init_value=0, count=1, run=False):
        self.logger.debug("pulse_pin(%r, %r, %r)", pin, time_l, time_h)
        max_tick = max(self._s_tick(time_l).value, self._s_tick(time_h).value)
        self.logger.debug("max_tick=%r", max_tick)
        div = int((max_tick - 1) / 0x7FFF) + 1
        self.logger.debug("div=%r", div)
        self.inst.FDwfDigitalOutIdleSet(self.handle, c_int(pin),
                                        c_uint(init_value))
        self.inst.FDwfDigitalOutCounterInitSet(self.handle, c_int(pin),
                                               c_uint(init_value == 0),
                                               c_uint(1))
        self.inst.FDwfDigitalOutDividerSet(self.handle, c_int(pin),
                                           c_uint(div))
        self.inst.FDwfDigitalOutCounterSet(self.handle, c_int(pin),
                                           self._s_tick(time_l / div),
                                           self._s_tick(time_h / div))
        self.logger.debug("ticks_l=%r", self._s_tick(time_l / div).value)
        self.logger.debug("ticks_h=%r", self._s_tick(time_h / div).value)
        self.inst.FDwfDigitalOutEnableSet(self.handle, c_int(pin), c_int(1))
        if run:
            run_time = time_h + time_l
            self.pulse_start(run_time * count)

    def pulse_start(self, run_time=1):
        self.logger.debug("pulse_start(run_time=%r)", run_time)
        self.inst.FDwfDigitalOutRepeatSet(self.handle, c_int(1))
        self.inst.FDwfDigitalOutRunSet(self.handle, c_double(run_time))
        self.inst.FDwfDigitalOutConfigure(self.handle, c_int(1))
        status = c_int()
        for _ in range(12):
            self.inst.FDwfDigitalOutStatus(self.handle, byref(status))
            if status.value == 2:
                break
            sleep(run_time/10)
        self.logger.debug("status=%r", status.value)

    def pins_reset(self):
        self.inst.FDwfDigitalOutReset(self.handle)

    def i2c_setup(self, scl_pin, sda_pin, speed=100000, clk_str=1):
        self.logger.debug("i2c_setup(scl_pin=%r, sda_pin=%r, speed=%r)",
                          scl_pin, sda_pin, speed)
        self.inst.FDwfDigitalI2cStretchSet(self.handle, c_int(clk_str))
        self.inst.FDwfDigitalI2cRateSet(self.handle, c_double(speed))
        self.inst.FDwfDigitalI2cSclSet(self.handle, c_int(scl_pin))
        self.inst.FDwfDigitalI2cSdaSet(self.handle, c_int(sda_pin))
        self.i2c_speed = speed

    def i2c_clear(self):
        ack = c_int()
        self.inst.FDwfDigitalI2cClear(self.handle, byref(ack))
        if ack.value == 0:
            raise IOError("i2c_clear: I2C ack failed, ensure pullup resistors")
        sleep((16 / self.i2c_speed) + 0.25)

    def i2c_reset(self):
        self.inst.FDwfDigitalI2cReset(self.handle)

    def i2c_read_nack_en(self, enable):
        self.inst.FDwfDigitalI2cReadNakSet(self.handle, c_int(enable))

    def i2c_read_regs(self, addr, reg, size=1, reg16=False):
        self.logger.debug("i2c_read_regs(addr=%r, reg=%r, size=%r, reg16=%r)",
                          addr, reg, size, reg16)
        write_data = [reg & 0xFF]
        if reg16:
            write_data.insert(0, ((reg >> 8) & 0xFF))
        write_data = (c_uint8 * len(write_data))(*write_data)
        read_data = (c_uint8 * size)()
        ack = c_int()
        self.inst.FDwfDigitalI2cWriteRead(self.handle, c_uint8(addr << 1),
                                          write_data, c_int(len(write_data)),
                                          read_data, c_int(size), byref(ack))
        if ack.value != 0:
            raise IOError("i2c_read_regs: I2C nack failed")
        self.logger.debug("ret=%r", list(read_data))
        return list(read_data)

    def i2c_write_regs(self, addr, reg, data, reg16=False):
        self.logger.debug("i2c_write_regs(addr=%r, reg=%r, data=%r, reg16=%r)",
                          addr, reg, data, reg16)
        write_data = data.copy()
        write_data.insert(0, (reg & 0xFF))
        if reg16:
            write_data.insert(0, ((reg >> 8) & 0xFF))
        size = len(write_data)
        write_data = (c_uint8 * size)(*write_data)
        ack = c_int()
        self.inst.FDwfDigitalI2cWrite(self.handle, c_uint8(addr << 1),
                                      write_data, c_int(size), byref(ack))
        if ack.value != 0:
            raise IOError("i2c_write_regs: I2C nack failed")

    def i2c_read_bytes(self, addr, size=1):
        self.logger.debug("i2c_read_bytes(addr=%r, size=%r)", addr, size)
        data = (c_uint8 * size)()
        ack = c_int()
        self.inst.FDwfDigitalI2cRead(self.handle, c_uint8(addr << 1), data,
                                     c_int(size), byref(ack))
        if ack.value != 0:
            raise IOError("i2c_read_bytes: I2C nack failed")
        self.logger.debug("ret=%r", list(data))
        return list(data)

    def i2c_write_bytes(self, addr, data):
        self.logger.debug("i2c_write_bytes(addr=%r, data=%r)", addr, data)
        size = len(data)
        write_data = (c_uint8 * size)(*data)
        ack = c_int()
        self.inst.FDwfDigitalI2cWrite(self.handle, c_uint8(addr << 1),
                                      write_data, c_int(size), byref(ack))
        if ack.value != 0:
            raise IOError("i2c_write_bytes: I2C nack failed")

    def spi_setup(self, mosi_pin, miso_pin, sck_pin, speed=100000,
                  clk_pol=0, clk_pha=0):
        self.logger.debug("spi_setup(mosi_pin=%r, miso_pin=%r, sck_pin=%r, "
                          "speed=%r, clk_pol=%r, clk_pha=%r)",
                          mosi_pin, miso_pin, sck_pin, speed, clk_pol, clk_pha)
        self.inst.FDwfDigitalSpiFrequencySet(self.handle, c_double(speed))
        mode = clk_pha + (clk_pol << 1)
        self.inst.FDwfDigitalSpiOrderSet(self.handle, c_int(1))
        self.inst.FDwfDigitalSpiModeSet(self.handle, c_int(mode))
        self.inst.FDwfDigitalSpiDataSet(self.handle, c_int(0), c_int(mosi_pin))
        self.inst.FDwfDigitalSpiDataSet(self.handle, c_int(1), c_int(miso_pin))
        self.inst.FDwfDigitalSpiClockSet(self.handle, c_int(sck_pin))
        self.inst.FDwfDigitalSpiWriteOne(self.handle, c_int(1), c_int(0),
                                         c_int(0))

    def spi_reset(self):
        self.logger.debug("spi_reset()")
        self.inst.FDwfDigitalSpiReset(self.handle)

    def spi_write(self, cs, data):
        self.logger.debug("spi_write(cs=%r, data=%r)", cs, data)
        write_data = (c_uint8 * len(data))(*data)
        self.inst.FDwfDigitalSpiSelect(self.handle, c_int(cs), c_int(0))
        self.inst.FDwfDigitalSpiWrite(self.handle, c_int(1), c_int(8),
                                      write_data, c_int(len(write_data)))
        self.inst.FDwfDigitalSpiSelect(self.handle, c_int(cs), c_int(1))

    def spi_read(self, cs, size=1):
        self.logger.debug("spi_read(cs=%r, size=%r)", cs, size)
        data = (c_uint8 * size)()
        size = c_int(size)
        self.inst.FDwfDigitalSpiSelect(self.handle, c_int(cs), c_int(0))
        self.inst.FDwfDigitalSpiRead(self.handle, c_int(1), c_int(8), data,
                                     size)
        self.inst.FDwfDigitalSpiSelect(self.handle, c_int(cs), c_int(1))
        self.logger.debug("ret=%r", list(data))
        return list(data)

    def spi_xfer(self, cs, data):
        self.logger.debug("spi_xfer(cs=%r, write_data=%r)", cs, data)
        size = len(data)
        write_data = (c_uint8 * size)(*data)
        read_data = (c_uint8 * size)()
        size = c_int(size)
        self.inst.FDwfDigitalSpiSelect(self.handle, c_int(cs), c_int(0))
        self.inst.FDwfDigitalSpiWriteRead(self.handle, c_int(1), c_int(8),
                                          write_data, size, read_data, size)
        self.inst.FDwfDigitalSpiSelect(self.handle, c_int(cs), c_int(1))
        self.logger.debug("ret=%r", list(read_data))
        return list(read_data)

    def analout_setup(self):
        self.inst.FDwfAnalogOutNodeEnableSet(self.handle, c_int(-1), c_int(0),
                                             c_bool(True))

    def analout_reset(self):
        self.inst.FDwfAnalogOutReset(self.handle, c_int(-1))

    def analout_set_dc(self, channel, level):
        chan = c_int(channel)
        self.inst.FDwfAnalogOutNodeFunctionSet(self.handle, chan,
                                               c_int(0), c_int(0))
        self.inst.FDwfAnalogOutNodeOffsetSet(self.handle, chan,
                                             c_int(0), c_double(level))
        self.inst.FDwfAnalogOutConfigure(self.handle, chan, c_bool(True))

    def analin_setup(self):
        chan = c_int(0)
        self.inst.FDwfAnalogInChannelEnableSet(self.handle, chan, c_bool(True))
        self.inst.FDwfAnalogInChannelOffsetSet(self.handle, chan, c_double(0))
        self.inst.FDwfAnalogInChannelRangeSet(self.handle, chan, c_double(10))
        self.inst.FDwfAnalogInConfigure(self.handle, c_bool(False),
                                        c_bool(False))

    def analin_reset(self):
        self.inst.FDwfAnalogInReset(self.handle)

    def analin_sample(self, channel):
        voltage = c_double()
        self.inst.FDwfAnalogInStatus(self.handle, False, None)
        self.inst.FDwfAnalogInStatusSample(self.handle, c_int(channel),
                                           byref(voltage))
        return float(voltage.value)


class DigilentAnalogDiscovery2():
    def __init__(self, *args, **kwargs):
        """Driver for Digilent Analog Discover 2"""
        self.logger = logging.getLogger(self.__class__.__name__)

        self.pins = self.get_pinout()
        self.mode = None
        self.default_spi_cs = kwargs.pop('default_spi_cs',
                                         self.pins['DUT_NSS'])
        self.default_i2c_addr = kwargs.pop('default_i2c_addr', 0x55)

        mosi_pin = kwargs.pop('mosi_pin', self.pins['DUT_MOSI'])
        miso_pin = kwargs.pop('miso_pin', self.pins['DUT_MISO'])
        sck_pin = kwargs.pop('sck_pin', self.pins['DUT_SCK'])
        spi_speed = kwargs.pop('spi_speed', 100000)
        clk_pol = kwargs.pop('clk_pol', 0)
        clk_pha = kwargs.pop('clk_pha', 0)
        self.spi_kwargs = {'mosi_pin': mosi_pin, 'miso_pin': miso_pin,
                           'sck_pin': sck_pin, 'speed': spi_speed,
                           'clk_pol': clk_pol, 'clk_pha': clk_pha}

        scl_pin = kwargs.pop('scl_pin', self.pins['DUT_SCL'])
        sda_pin = kwargs.pop('sda_pin', self.pins['DUT_SDA'])
        i2c_speed = kwargs.pop('i2c_speed', 100000)
        clk_str = kwargs.pop('clk_str', 1)
        self.i2c_kwargs = {'scl_pin': scl_pin, 'sda_pin': sda_pin,
                           'speed': i2c_speed, 'clk_str': clk_str}

        if "driver" in kwargs:
            self.driver = kwargs["driver"]
        else:
            self.driver = DigilentDriver(*args, **kwargs)

    @staticmethod
    def get_pinout():
        pins = {}
        pins["DUT_IC"] = 0
        pins["DEBUG0"] = 1
        pins["DEBUG1"] = 2
        pins["DEBUG2"] = 3
        pins["DUT_SCL"] = 4
        pins["DUT_SDA"] = 5
        pins["DUT_MOSI"] = 6
        pins["DUT_MISO"] = 7
        pins["DUT_SCK"] = 8
        pins["DUT_NSS"] = 9
        # DUT_ADC os connected to waveform channel 0 labeled W1
        pins["DUT_ADC"] = 0
        # DUT_ADC os connected to scope channel 0 labeled 1+
        pins["DUT_DAC"] = 0
        return pins

    def _spi_mode(self):
        if self.mode != 'spi':
            self.driver.pins_reset()
            self.driver.spi_setup(**self.spi_kwargs)
            self.mode = 'spi'

    def _i2c_mode(self):
        if self.mode != 'i2c':
            self.driver.pins_reset()
            self.driver.i2c_setup(**self.i2c_kwargs)
            try:
                self.driver.i2c_clear()
            except IOError as exc:
                self.logger.debug(exc)
            self.mode = 'i2c'

    def _pulse_mode(self):
        if self.mode != 'pulse':
            self.driver.pins_reset()
            self.driver.pins_reset()
            self.mode = 'pulse'

    def _analout_mode(self):
        if self.mode != 'analout':
            self.driver.pins_reset()
            self.driver.analout_setup()
            self.mode = 'analout'

    def _analin_mode(self):
        if self.mode != 'analin':
            self.driver.pins_reset()
            self.driver.analin_setup()
            self.mode = 'analin'

    def pulse(self, pin, time_l, time_h=None, count=1):
        self._pulse_mode()
        time_h = time_h or time_l
        if not isinstance(pin, int):
            pin = self.pins[pin]
        self.driver.pulse_pin(pin, time_l, time_h, count=count, run=True)

    def i2c_write_bytes(self, data, addr=None):
        self._i2c_mode()
        if not isinstance(data, list):
            data = [data]
        addr = addr or self.default_i2c_addr
        self.driver.i2c_write_bytes(addr, data)

    def i2c_write_regs(self, reg, data, addr=None, reg16=False):
        self._i2c_mode()
        if not isinstance(data, list):
            data = [data]
        addr = addr or self.default_i2c_addr
        self.driver.i2c_write_regs(addr, reg, data, reg16=reg16)

    def i2c_read_bytes(self, size=1, addr=None):
        self._i2c_mode()
        addr = addr or self.default_i2c_addr
        return self.driver.i2c_read_bytes(addr, size)

    def i2c_read_regs(self, reg, size=1, addr=None, reg16=False):
        self._i2c_mode()
        addr = addr or self.default_i2c_addr
        return self.driver.i2c_read_regs(addr, reg, size, reg16=reg16)

    def spi_write_bytes(self, data, cs=None):
        self._spi_mode()
        if not isinstance(data, list):
            data = [data]
        cs = cs or self.default_spi_cs
        self.driver.spi_write(cs, data)

    def spi_write_regs(self, reg, data, cs=None):
        self._spi_mode()
        cs = cs or self.default_spi_cs
        if not isinstance(data, list):
            data = [data]
        write_data = data.copy()
        write_data.insert(0, reg)
        self.driver.spi_write(cs, write_data)

    def spi_read_bytes(self, cs=None, size=1):
        self._spi_mode()
        cs = cs or self.default_spi_cs
        return self.driver.spi_read(cs, size)

    def spi_read_regs(self, reg, size=1, cs=None):
        self._spi_mode()
        cs = cs or self.default_spi_cs
        data = [reg | 0x80]
        data.extend([0]*size)
        return self.driver.spi_xfer(cs, data)[1:]

    def spi_xfer(self, data, cs=None):
        self._spi_mode()
        if not isinstance(data, list):
            data = [data]
        write_data = data.copy()
        cs = cs or self.default_spi_cs
        return self.driver.spi_xfer(cs, write_data)

    def anal_output_volts(self, volts, channel=None):
        self._analout_mode()
        channel = channel or self.pins['DUT_ADC']
        self.driver.analout_set_dc(channel, volts)

    def anal_sample(self, channel=None):
        self._analin_mode()
        channel = channel or self.pins['DUT_DAC']
        return self.driver.analin_sample(channel)


def main():
    """Main program example with PHiLIP"""
    # logging.basicConfig(level="DEBUG")
    dad2 = DigilentAnalogDiscovery2()
    print("scope 0 = {}V".format(dad2.anal_sample()))
    dad2.anal_output_volts(1.0)
    dad2.pulse("DUT_IC", 0.000001, count=128)
    dad2.pulse("DEBUG0", 0.0001, count=256)
    print("first available i2c byte:{}".format(dad2.i2c_read_bytes()))
    data = [42, 69, 69, 1]
    print("writing {} to i2c registers 5".format(data))
    dad2.i2c_write_regs(5, data)
    res = dad2.i2c_read_regs(2, 10)
    print("i2c reading 10 bytes starting a reg 2:{}".format(res))
    print("spi writing {} to reg 23".format(data))
    dad2.spi_write_regs(23, data)
    res = dad2.spi_read_regs(22, 14)
    print("spi reading 14 bytes starting a reg 22:{}".format(res))


if __name__ == '__main__':
    main()
