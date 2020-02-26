# Release Qualification Procedure for PHiLIP
The qualification must be done to make PHiLIP a valid reference.
This ensures things like i2c will reply with the proper response or fail when it should fail.
The qualification usually involves both automated tests and verification with qualified instruments such as oscilloscopes.
The following procedure must be performed before any release of PHiLIP.
All tests must be performed on both PHiLIP-N and PHiLIP-B unless specified.
The released firmware is available in the [releases](https://github.com/riot-appstore/PHiLIP/releases) section.

# Tools and setup
The following are all the tools and packages needed to run the tests

### Packages
- `sudo pip3 install philip_pal pytest`

### Tools
- Oscilloscope or logic analyzer with protocol decoder
- Device that can do master SPI and I2C
- Bluepill
- Nucleo-f103rb


## 1. Interface Tests
The interface tests check how philip uart protocol, memory map allocation, and the actual python interface for philip.
Since this test has regression for the memory maps it should be executed and updated first.
Remember to connect `DUT_RST` to `DEBUG0` pin and pullup the to 3.3V.

1. Connect PHiLIP to the computer
2. Run tests located in `IF/philip_pal` with `python3 setup.py test`
3. Evaluate any changes in regression testing
4. If everything looks good then accept the changes with `python3 setup.py test --addopts --regtest-reset`

## 2. Build Tests
PHiLIP must be able to build in all configurations with both Eclipse and Makefiles.
The CI performs the build tests on the Makefiles which are release versions of PHiLIP (O3 optimization), however, manual verification should be used for all configurations.

1. Connect PHiLIP-n
2. Run `./build_test-n.sh`
3. Evaluate test results to make sure everything passes
4. Run debug mode eclipse for the `NUCLEO-F103RB_DEBUG`
5. While running, run tests located in `IF/philip_pal/philip_pal` with `python3 setup.py test`
6. Evaluate test results to make sure everything passes
7. Connect PHiLIP-b with a nucleo header that has clear memory _(this ensures the tx line is not being held by the nucleo)_
8. Run `FLASH_PORT=<FLASH_PORT> ./build_test-b.sh` where `FLASH_PORT` is the name of the nucleo header, for example `NODE_F103RB` for the nucleo-f103rb
9. Evaluate test results to make sure everything passes
10. Run debug mode eclipse for the `BLUEPILL_DEBUG`
11. While running, run tests located in `IF/philip_pal/philip_pal` with `python3 setup.py test`
12. Evaluate test results to make sure everything passes

## 3. UART Peripheral Tests
Tests functional of the UART.  The pinout must be correctly set up and a usb to serial converter should be used.

PHiLIP |     USB to Serial Converter
--------|-----------------------------
DUT_RX  | TX
DUT_TX | RX
DUT_CTS | RTS
DUT_RTS | CTS

1. Connect pins as per the table to the PHiLIP-b
2. Run `pytest test_uart.py`
3. Connect pins as per the table to the PHiLIP-n
4. Run `pytest test_uart.py`

## 4. I2C Peripheral Tests
The i2c tests should be done with a scope or logic analyzer and another i2c master device. Connect the i2c master, probes and PHiLIP `DUT_SCL` and `DUT_SDA` pins.  Use strong pullup resistors to eliminate hardware faults.


Evaluate the following:
- 1/2/3/10 byte read
- 1/2/3/10 byte write
- write-repeated start-read
- 16 bit register read, different endian
- scan all addresses, only two should respond
- ticks and count at different speeds
- clock stretch 10ms
- no clock stretch fails
- data nack mode
- check status flags behave
- disable stops everything and re-enable doesn't cause timeouts

Not tested:
- general call
- 10 bit address

## 5. SPI Peripheral Tests
The spi tests should be done with a scope or logic analyzer and another spi master device. Connect the spi master, probes and PHiLIP `DUT_MOSI`, `DUT_MISO`, `DUT_SCK` and `DUT_NSS` pins.  _NOTE: The SCLK pin can sometimes wring depending on cable lengths, a resistor or capacitor or even a extra wire._

Evaluate the following:
- 16 bit modes and endianness
- different if types
- disable works
- different modes (4)
- counts and ticks (tick reading +- 12 us)
- register options

Not tested:
- status flags

## 6. Trace Tests
The trace tests should be verified with a scope or precision timing.  Connect to the `DEBUG0`, `DEBUG1` and `DEBUG2` pins.

Evaluate the following:
- Connect the each pin and toggle the following and verify
- Evaluate the timing at different tick div up to 16 (+-15%)
