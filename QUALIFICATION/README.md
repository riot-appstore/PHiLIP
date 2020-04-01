# Release Qualification Procedure for PHiLIP
The qualification must be done to make PHiLIP a valid reference.
This ensures things like i2c will reply with the proper response or fail when it should fail.
The qualification usually involves both automated tests and verification with qualified instruments such as oscilloscopes.
The following procedure must be performed before any release of PHiLIP.
All tests must be performed on both PHiLIP-N and PHiLIP-B unless specified.
The released firmware is available in the [releases](https://github.com/riot-appstore/PHiLIP/releases) section.

# Tools and setup
The following are all the tools and packages needed to run the tests.

### Packages
- `sudo pip3 install philip_pal pytest`
- [Digilent Waveforms](https://reference.digilentinc.com/reference/software/waveforms/waveforms-3/start)

### Tools
- Digilent Analog Discovery 2
- FTDI USB to UART converter with flow control
- Bluepill
- Nucleo-f103rb


## Static Tests
Even though any merges should have passed the static tests, the static tests should be run.

Go to the base directory and run `./scripts/static_tests.py`.
Everything should pass without any errors.

## Interface Tests
The interface tests check how philip uart interface protocol, memory map allocation, and the actual python interface for philip.
Since this test has regression for the memory maps it should be executed and updated first.
This can be done on any PHiLIP board as it only confirms the host is up to date

1. Go to the `IF/philip_pal` directory
1. Connect PHiLIP to the computer
1. Flash the binary to the PHiLIP
1. Run tests with `./setup.py test`
1. Evaluate any changes in regression testing
1. If everything looks good then accept the changes with `./setup.py test --addopts --regtest-reset`

## Build Tests
PHiLIP must be able to build in all configurations with both Eclipse and Makefiles.
The CI performs the build tests on the Makefiles which are release versions of PHiLIP (O3 optimization).

1. Connect PHiLIP-n
1. Run `./build_test-n.sh`
1. Evaluate test results to make sure everything passes
1. Run debug mode eclipse for the `NUCLEO-F103RB_DEBUG`
1. While running, run tests located in `IF/philip_pal/` with `./setup.py test`
1. Evaluate test results to make sure everything passes
1. Connect PHiLIP-b with a nucleo header that has clear memory _(this ensures the tx line is not being held by the nucleo)_
1. Run `FLASH_PORT=<FLASH_PORT> ./build_test-b.sh` where `FLASH_PORT` is the name of the nucleo header, for example `NODE_F103RB` for the nucleo-f103rb
1. Evaluate test results to make sure everything passes
1. Run debug mode eclipse for the `BLUEPILL_DEBUG`
1. While running, run tests located in `IF/philip_pal/` with `./setup.py test`
1. Evaluate test results to make sure everything passes

##  UART Peripheral Tests
This test should be run for both the PHiLIP_b and PHiLIP_n.

1. Connect pins shown when running `./test_uart.py`
1. Run `pytest test_uart.py`


## I2C Peripheral Tests
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

To run automated tests:
1. Connect pins shown when running `./test_i2c.py`
1. Run `pytest test_i2c.py`


## SPI Peripheral Tests

The spi tests should be done with a scope or logic analyzer and another spi master device.
Connect the spi master, probes and PHiLIP `DUT_MOSI`, `DUT_MISO`, `DUT_SCK` and `DUT_NSS` pins.

Evaluate the following:
- 16 bit modes and endianness
- different if types
- disable works
- different modes (4)
- counts and ticks (tick reading +- 12 us)
- register options

or

In the qualification folder run: `pytest test_spi.py` with Digilent device connected.

Not tested:
- status flags


## Timing Tests

Evaluate the following:
- Connect the each pin and toggle the following and verify

| Module | Mode | Min Holdoff Time (ns) | Jitter (ns) |Events At Max Speed | Min Pause Time (us) |
|-|-|-| -|-|-|
| `tmr`  |  Both Edges | 1000  | 200  |  128 | 100
| `tmr`  |  Rising | 200  | 28  |  128 | 100
| `tmr`  |  Falling | 200  | 28  |  128 | 100
| `gpio`  | Both Edges |  10000 | 600  | n/a  |  n/a |

To run automated tests:
1. Connect pins shown when running `./test_timing.py`
1. Run `pytest test_timing.py`

## ADC Tests

Evaluating the following:
- Measure ADC samples at different voltages
- Measure ADC samples after PHiLIP resets

To run automated tests:
1. Connect pins shown when running `./test_adc.py`
1. Run `pytest test_adc.py`

## DAC Tests

Evaluating the following:
- Verify output accuracy
- Verify jitter

To run automated tests:
1. Connect pins shown when running `./test_dac.py`
1. Run `pytest test_dac.py`
