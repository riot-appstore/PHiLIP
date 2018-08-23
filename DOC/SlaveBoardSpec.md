# Purpose

The purpose of this specification is to identify an easily acquirable or reproducible, certified slave device (the BPT or bluepill tester) that can be used to automate regression testing of basic peripherals such as UART, I2C, SPI, etc.  The slave board shall be usable offline by developers and/or within a HiL (hardware in the loop) CI (continuous integration) framework.

# Abbreviations
**BPT** - Bluepill Tester  
**CI** - Continuous Integration   
**DUT** - Device Under Tests  
**HiL** - Hardware In The Loop  
**FW** - Firmware  
**SN** - Serial Number  
**RPi** - Raspberry Pi 3  


# Technical Requirements Of Slave Device Hardware

|                               Requirement                               	| Reason                                                   	| Implementation                                     	|
|:-----------------------------------------------------------------------:	|----------------------------------------------------------	|----------------------------------------------------	|
| 3.3V core voltage, 5V tolerant pins                                     	| Some boards that must be tested are either 3.3V or 5V    	| All pins DUT pins but ADC                          	|
| Reasonably low cost (2 to 15 USD)                                       	| Allows for scalability and makes it accessible to users 	| 2.36 USD on Ebay, [bluepill](http://wiki.stm32duino.com/index.php?title=Blue_Pill) with a STM32F103C8 MCU                                   	|
| GPIO output for reset of DUT                                            	| Prevent hanging and get DUT to known state               	| (PB12) DUT_RST                                     	|
| UART with modem support                                                 	| Test DUT UART features                                   	| (UART3) DUT_CTS, DUT_RTS, DUT_RX, DUT_TX           	|
| Frequency measurement (input capture)                                   	| Test if DUT clocks/timers/pwm is correct                 	| (T1C1) DUT_IC                                      	|
| SPI slave with settable modes, speed up to 24 MHz, optional chip select 	| Test DUT master SPI features                             	| (SPI1) DUT_NSS, DUT_SCK, DUT_MISO, DUT_MOSI        	|
| I2C slave with settable address, speed up to 3.4 MHz, settable pullups  	| Test DUT master I2C features                             	| (I2C1) DUT_SCL, DUT_SDA                            	|
| DAC                                                                     	| Test DUT ADC values and linearity                        	| (T4C3) DUT_DAC [1] [2]                           	|
| PWM/clock output                                                        	| Test DUT timer reading and input capture                 	| (T4C4) DUT_PWM [2]                                	|
| Power monitoring                                                        	| Test DUT low power modes                                 	| (ADC) PM_A_ADC, PM_V_ADC (3) [5]                     	|
| ADC                                                                     	| Test DUT DAC or signals                                  	| (ADC) DUT_ADC [5]                                      	|
| Flashing LED                                                            	| Show status of testing device                            	| (PC13) LED0                                        	|
| Computer interface                                                      	| To configure and get testing device information          	| (USB or UART1) USB_DM, USB_DP or IF_RX, IF_RX [4] 	|
| Serial number                                                           	| Allows for traceability                                  	| 12 byte UID                                        	|
| Firmware revision                                                       	| Coordinate with test interface                           	| Hash of the binary to keep unique to code          	|         	|


_[1] must have RC filter_  
_[2] may share pins with CAN bus_  
_[3] requires special hardware_  
_[4] UART1 contains ROM bootloader_  
_[5] only works up to 3.3V_


# Future Considerations
- CANBus support
- Master mode of periph support
- Multi master I2C
- Port to RIOT

# FAQs
### Why not use a tool (like buspirate) that does this already?
- Standard test tools from DIOLAN and Total Phase require licenses and are expensive
- Most tools don't achieve everything needed, that is why it would be good to customize firmware

### Why not just test against sensors?
- Sensors and other slave devices cannot test all configurable modes (try to find a SPI MODE 3 or 4 device)

### Why not use a Raspberry Pi?
- RPi does not support low level testing
- RPi's drivers are not well certified

### Why not use Arduino?
- Standard Arduino extensions don't support all configurable features well
- Arduino hardware doesn't support everything

### Why not implement in RIOT or any other OS?
- If the device is using RIOT then it shouldn't be tested on RIOT, it should be tested against an accepted reference
- The tester should be minimal to not introduce unnecessary overhead
- RIOT doesn't support many slave features

### Are the peripheral pins static or reassignable?
- Pins are static, this makes them simple to wire and simple to develop

### Why program for only one board and not make it usable on other boards?
- Lots of code takes advantage of hardware specific features
- Certification becomes simpler, only need to certify one boards
- Documentation and wiring become simpler
- The board should be easy to obtain so it is not necessary to depend on boards that are lying around

### What happens if the bluepill board become obsolete?
- This can happen to any board
- The bluepill board is very simple to recreate (mcu, crystals, button, leds, connectors)
- The nucleo-f103rb can also be used with a change in pinout if usb is not used

### Why program the BPT bare metal (or using STMCube)?
- STMCube provides a standard reference to start so boilerplate code doesn't need to be implemented, tested, and debugged
- Bare metal is used since the application features are generally simple and an RTOS is not needed
- If alterations are made in the future it is not necessary to update or bring in a new RTOS

### How dependant is the bpt on the library (STMCube and CMSIS)?
- ...Very, many features use the HAL of the STMCube, this allows for quicker development time
- Application code will try to remain modular allowing for porting
- Since the HAL is used for much of the development, porting to other boards that support it should be quick (ie STM based boards)

### What are the final thoughts
- The most efficient way to assist RIOT is to quickly deploy a usable system, so far this is the best/fastest/most reliable way to do so
- Additional support for slave peripherals can be added after knowledge is gained from the baremetal development

# Shell Communication Protocol
## Introduction
The BPT will have a register based protocol, see [memory map](bpt_mem_map.csv) for information on the registers.  All interfaces (UART, USB, I2C, SPI) will access the registers (with different access levels).

## Shell Interface Description
The shell interface can be accessed either through UART1 or the devices USB, both have the same protocol.

### Shell Commands
| Command 	| Arguments                                                  	| Response                                            	| Description                                                                                                          	|
|---------	|------------------------------------------------------------	|-----------------------------------------------------	|----------------------------------------------------------------------------------------------------------------------	|
| rr      	| [REGISTER_INDEX] [SIZE]                                    	| [errno], 0x[BYTE n][BYTE n-1]...[BYTE 1][BYTE 0]    	| Read the memory map register                                                                                         	|
| wr      	| [REGISTER_INDEX] [BYTE 0] [BYTE 1] ... [BYTE n-1] [BYTE n] 	| [errno]                                             	| Write to the memory map register (only fills the register, does not change configurations)                           	|
| ex      	|                                                            	| [errno]                                             	| Executes configuration changes (ie. if i2c slave address is changed it will reinitialize with the new slave address) 	|
| mcu_rst 	|                                                            	| [errno], Initialized, Build Date: [BUILD DATE TIME] 	| Execute a soft reset of the device                                                                                   	|

# Development Phases
## Phase A (Initial implementation with I2C)
- Basic UART IF Protocol
- I2C slave mode with configuration
- First prototype of code generators for memory map management
- First prototype of python IFs for the bpt
- First prototype of RIOT i2c_periph tests
- Basic support of FW and SN
- Basic support of flashing LED0
- Example test of how to use the interfaces

## Phase B (Add DUT_UART functionality)
- UART IF and USB IF function and are standardized
- DUT_UART implemented
- Code generator standardized and tested
- Standardized python IF to BPT, periph_i2c, periph_uart
- Basic qualification test for BPT firmware
- Usable UART test

## Phase C (Add remaining peripherals)
- TBD

# Target Tests to Run
Test should both test that certain conditions pass (eg. i2c_read_bytes actually reads the correct bytes) and the expected failures occur (eg. reading from a wrong i2c address should return the proper error code and not return a success)
## SPI Test
### Pass Cases
- write to dummy register
- read from dummy register
- ensure a change has occurred
- change to all 4 modes
- 8/16 bit frame
- speeds
- stress test
- send different dummy data
### Failure Cases
- incorrect mode settings
- incorrect pin configs
- unsupported speeds
- frame errors

## I2C
### Pass Cases
- check if slave present
- write register
- read register
- change slave address
- slave clock stretch
- slave data not ready
- speeds
- stress tests
- 10 bit and 7 bit addr
- 8/16 bit registers

### Failure Cases
- unterminated session
- wrong slave address
- incorrect pin configs
- unsupported speeds
- no pullup resistor
- double acquire

## UART
### Pass Cases
- basic comms
- modem support
- change baudrate
- parity
- stop bit
- stress rx
- echo data
- change data
- ack data

### Failure Cases
- wrong baudrate
- wrong configs
- incorrect pin configs

## ADC
### Pass Cases
- Linearity
- speeds

### Failure Cases
- incorrect pin configs
- unsupported speeds

## PWM
### Pass Cases
- change duty cycle
- change period
- min/max bounds
- disable/enable
- timing test

## DAC
### Pass Cases
- linearity

## Timers
### Pass Cases
- time output clock is correct
