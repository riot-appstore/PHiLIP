PHiLIP (Primitive Hardware In the Loop Integration Product)
===========================================================
[![Build Status](https://travis-ci.org/riot-appstore/PHiLIP.svg?branch=master)](https://travis-ci.org/riot-appstore/PHiLIP)


## Contents
<a name="c_desc"></a>
- [ Description ](#desc)
<a name="c_setup"></a>
- [ Setup of PHiLIP Environment ](#setup)
<a name="c_gs_dev"></a>
- [ Getting Started as a Developer ](#gs_dev)
<a name="c_gs_ci"></a>
- [ Getting Started with CI Scripts ](#gs_ci)
<a name="c_arch"></a>
- [ PHiLIP Architecture ](#arch)
<a name="c_qual"></a>
- [Qualification of PHiLIP](#qual)
<a name="c_map"></a>
- [PHiLIP Memory Map](#map)
<a name="c_proto"></a>
- [PHiLIP Serial Protocol](#proto)
<a name="c_inter"></a>
- [PHiLIP Python Interface](#inter)
<a name="c_pinb"></a>
- [PHiLIP-b Pinout](#pinb)
<a name="c_pinn"></a>
- [PHiLIP-n Pinout](#pinn)
- [PHiLIP Firmware Documentation](https://mrkevinweiss.github.io/philip/doxygen/index.html)

<a name="desc"></a>
## [Description](#c_desc)

PHiLIP is qualified open-source firmware for nucleo-f103rb or bluepill boards used for testing peripherals of other embedded devices.
PHiLIP is a low-cost solution to allow detailed, corner case peripheral testing for both developers and CI systems.
PHiLIP is aimed at getting salient information that would be gathered from an oscilloscope or logic analyzer as well as injecting specific peripheral behaviors.
PHiLIP is designed for testing peripheral APIs for embedded operating systems and hardware abstraction layers but was built with an architecture that allows for _easy_ extensions to other applications such as product qualification or simulation.
PHiLIP can be used with a raw serial connection but also comes with a python interface that simplifies writing test scripts as well as a shell for developers to run manual tests.

<a name="setup"></a>
## [Setup of PHiLIP Environment](#c_setup)
The setup will explain how to flash PHiLIP and install the python interface.
To setup the PHiLIP environment some hardware will be required, either a nucleo-f103rb or a bluepill with a usb to uart converter.

#### 1. Connect PHiLIP pins
For the nucleo-f103rb only the usb connection is required.
The bluepill will require a uart connection to `A9 - IF_TX` to the uart RX pin and the `A10 - IF_RX` to the uart TX pin.
See the [PHiLIP-b pinout](RESOURCES/PHiLIP-BLUEPILL-PINOUT.png) for more information.
The uart is needed for the basic interface but can also be used for ROM UART flashing.
To flash the bluepill with SWD, connect the swd pins and reset `R - NRST` pin.

#### 2. Flash PHiLIP to the device
The qualified firmware for PHiLIP is stored in the [PHiLIP repo](QUALIFICATION/FW).
The correct firmware is needed for the given board, either the [PHiLIP_BLUEPILL](QUALIFICATION/FW/PHiLIP-BLUEPILL-CURRENT.bin) or the [PHiLIP_NUCLEO-F103RB](QUALIFICATION/FW/PHiLIP-NUCLEO-F103RB-CURRENT.bin).
Previous versions can also be browsed or installed but the _CURRENT_ version is recommended.
To flash the firmware on the nucleo-f103rb, drag and drop the .bin file to the nucleo device.
There are many ways to flash the bluepill, either by connecting a swd connector or with the [ROM UART bootloader](https://medium.com/@paramaggarwal/programming-an-stm32f103-board-using-usb-port-blue-pill-953cec0dbc86).


_HINT: If flashing a nucleo-f103rb use the following command_

`wget -P /media/${USER}/NODE_F103RB/ https://github.com/riot-appstore/PHiLIP/raw/master/QUALIFICATION/FW/PHiLIP-NUCLEO-F103RB-CURRENT.bin
`

#### 3. Install the Python Interface (philip_pal)
The philip_pal is only python3 so use the following command to install:

`sudo pip3 install philip_pal`

<a name="gs_dev"></a>
## [Getting Started as a Developer](#c_gs_dev)
_First follow the [setup](#setup)_

To use PHiLIP as a developer, an interface shell `philip_shell` is provided with the [philip_pal package](IF/philip_pal).
The philip_shell has a connect wizard, command history, and auto-completion.
If in doubt, try pressing tab a few times.

The following is an example of PHiLIP running and evaluating a toggling pin.
This should provide a way to get started using PHiLIP.
Run the philip shell with `philip_shell` _(use the -h to view additional args)_

1. Check available commands.
```
PHiLIP: help
Documented commands (type help <topic>):
========================================
data_filter      exit         info_record_type  read_reg     show_pinout      
dut_reset        get_version  philip_reset      read_struct  write_and_execute
execute_changes  help         print_map         read_trace   write_reg
```

2. Check the description of the memory map.
This should help explain what each register is responsible for.
```
info_record_type description
```

3. Check the pinout of the philip.
```
show_pinout
```

4. Connect the `DUT_RST` to the `DEBUG0` pin.
5. Check what the `gpio[0].mode.io_type` register does.
```
print_map gpio[0].mode.io_type
```
6. Enable the `DEBUG0` or gpio0 pin to interrupt mode so traces can be collected.
```
write_and_execute gpio[0].mode.io_type 3
```
7. Use the `DUT_RST` pin to toggle events on the `DEBUG0` pin.
```
dut_reset
```

8. Verify the events are logged in a human-readable way.
```
read_trace
```

9. Read the basic tick registers of the trace.
```
read_reg trace.tick
```

10. Only read the first two elements of the array.
```
read_reg trace.tick 0 2
```

11. Now read the whole trace structure.
```
read_struct trace
```

12. Toggle the data filter off to see very verbose details of the tick traces
```
data_filter
read_struct trace
```

13. Now reset philip back to the default state.
```
data_filter on
philip_reset
```

<a name="gs_ci"></a>
## [Getting Started with CI Scripts](#c_gs_ci)
_First follow the [setup](#setup)_

To use the python interface in a CI, a Phil class is provided.
Create a python script for the CI to run and import Phil to use.
Refer to the [philip_if.py](/IF/philip_pal/philip_pal/philip_if.py) for more information.

Example of python test script tests the trace function, connect the `DUT_RST` to the `DEBUG0` pin.

```python
from philip_pal import Phil

phil = Phil()
print("interface version: {}".format(phil.if_version))

# Reset philip to a clean state
assert phil.reset_mcu()['result'] == phil.RESULT_SUCCESS

# Setup DEBUG0 pin to log trace events with interrupt
for result in phil.write_and_execute('gpio[0].mode.io_type', 3):
    # Check each result for success
    assert result['result'] == phil.RESULT_SUCCESS

# Toggle the DUT_RST pin for the default period
phil.dut_reset()

# Toggle the DUT_RST pin for 1 second
phil.dut_reset(1.0)

trace = phil.read_trace()

# Assert the second toggle was in fact about 1 second
elapse_time = trace[3]['time'] - trace[2]['time']
assert elapse_time > 0.9 and elapse_time < 1.1

print("Trace Results")
print(trace)
```
<a name="arch"></a>
## [PHiLIP Architecture](#c_arch)
#### PHiLIP Infrastructure
PHiLIP uses a combination of tools in order to work.
The [memory_map_manager](https://github.com/riot-appstore/memory_map_manager) or MMM help maintain and coordinate the memory map used for the firmware, documentation, and interface.
The [philip_pal](IF/philip_pal) wraps around the basic serial protocol so functionality can be implemented and handled with a higher level language and in a non-constrained environment.
The initial version used [STM32Cube](https://www.st.com/en/development-tools/stm32cubemx.html), however, the generator is being deprecated in this project due to lack of portability.


<a href="RESOURCES/PHiLIP_infrastructure.png">
    <img src="RESOURCES/PHiLIP_infrastructure.png" alt="drawing" width="1024px"/>
</a>

#### PHiLIP Firmware Design
PHiLIP firmware is designed to easily add peripheral functionality.
It separates out the peripherals from the communication and application logic and the memory map.

<a href="RESOURCES/PHiLIP_firmware_arch.png">
    <img src="RESOURCES/PHiLIP_firmware_arch.png" alt="drawing" width="1024px"/>
</a>

<a name="qual"></a>
## [Qualification of PHiLIP](#c_qual)
The qualification must be done to make PHiLIP a valid reference.
When changes are made PHiLIP should undergo the [qualification procedure](QUALIFICATION/TESTS). This ensures things like i2c will reply with the proper response or fail when it should fail.
The qualification is also a way to track the comparabilities of PHiLIP.
The qualification usually involves both automated tests and verification with qualified instruments such as oscilloscopes.

The released firmware is available in the [QUALIFIED FIRMWARE](QUALIFICATION/FW) section.

<a name="map"></a>
## [PHiLIP Memory Map](#c_map)
The memory map is a way to access a large amount of information in a compact way.
With the [memory_map_manager](https://github.com/riot-appstore/memory_map_manager), firmware, documentation, and interfaces are all coordinated with a single config file.
This makes changes easy to manage, for example, if a field for reporting if an i2c fails to ACK on data is needed, that field can be added to the config file.
After updating the map it can be accessed from the interface to the firmware.
The current versioned memory maps is available [here](IF/philip_pal/philip_pal/mem_map).

_Example of memory map_

| name                            |     description                                                                                                                                                                                               |
|---------------------------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| user_reg                        |     Writable registers for user testing - Starts at 0 and increases by 1 each register by default                                                                                                             |
| sys.sn                          |     Unique ID of the device                                                                                                                                                                                   |
| sys.fw_rev                      |     Firmware revision                                                                                                                                                                                         |
| sys.if_rev                      |     Interface revision - This corelates to the version of the memory map                                                                                                                                      |
| sys.tick                        |     Tick in ms - Updates with the sys tick register every few ms                                                                                                                                              |
| sys.build_time.second           |     The build time seconds                                                                                                                                                                                    |
| sys.build_time.minute           |     The build time minutes                                                                                                                                                                                    |
| sys.build_time.hour             |     The build time hours                                                                                                                                                                                      |
| sys.build_time.day_of_month     |     The build time day of month                                                                                                                                                                               |
| sys.build_time.day_of_week      |     Not used                                                                                                                                                                                                  |
| sys.build_time.month            |     The build time month                                                                                                                                                                                      |
| sys.build_time.year             |     The build time year (20XX)                                                                                                                                                                                |
| sys.build_time.res              |     Reserved bytes                                                                                                                                                                                            |
| sys.device_num                  |     The philip device designator - A constant number to identify philip firmware                                                                                                                              |
| sys.sys_clk                     |     The frequency of the system clock in Hz                                                                                                                                                                   |
| sys.status.update               |     1:register configuration requires exceution for changes - 0:nothing to update                                                                                                                             |
| sys.status.board                |     1:board is a bluepill - 0:board is a nucleo-f103rb                                                                                                                                                        |
| sys.mode.init                   |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| sys.mode.dut_rst                |     1:put DUT in reset mode - 0:run DUT                                                                                                                                                                       |
| i2c.mode.init                   |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| i2c.mode.disable                |     0:periph is enabled - 1:periph is disabled                                                                                                                                                                |
| i2c.mode.addr_10_bit            |     0:i2c address is 7 bit mode - 1:10 i2c address is 10 bit mode                                                                                                                                             |
| i2c.mode.general_call           |     0:disable general call - 1:enable general call                                                                                                                                                            |
| i2c.mode.no_clk_stretch         |     0:slave can clock stretch - 1:disables clock stretch                                                                                                                                                      |
| i2c.mode.reg_16_bit             |     0:8 bit register access - 1:16 bit register access mode                                                                                                                                                   |
| i2c.mode.reg_16_big_endian      |     0:little endian if 16 bit register access - 1:big endian if 16 bit register access                                                                                                                        |
| i2c.mode.nack_data              |     0:all data will ACK - 1:all data will NACK                                                                                                                                                                |
| i2c.status.ovr                  |     Overrun/Underrun: Request for new byte when not ready                                                                                                                                                     |
| i2c.status.af                   |     Acknowledge failure                                                                                                                                                                                       |
| i2c.status.berr                 |     Bus error:  Non-valid position during a byte transfer                                                                                                                                                     |
| i2c.status.gencall              |     General call address received                                                                                                                                                                             |
| i2c.status.busy                 |     i2c bus is BUSY                                                                                                                                                                                           |
| i2c.status.rsr                  |     Repeated start detected                                                                                                                                                                                   |
| i2c.clk_stretch_delay           |     Clock stretch the first byte in us                                                                                                                                                                        |
| i2c.slave_addr_1                |     Primary slave address                                                                                                                                                                                     |
| i2c.slave_addr_2                |     Secondary slave address                                                                                                                                                                                   |
| i2c.state                       |     Current state of i2c frame - 0:initialized - 1:reading data - 2-write address recieved - 3-1st reg byte recieved - 4-writing data - 5-NACK - 6-stopped                                                    |
| i2c.reg_index                   |     current index of i2c pointer                                                                                                                                                                              |
| i2c.start_reg_index             |     start index of i2c pointer                                                                                                                                                                                |
| i2c.r_count                     |     Last read frame byte count - only in reg if_type 0                                                                                                                                                        |
| i2c.w_count                     |     Last write frame byte count - only in reg if_type 0                                                                                                                                                       |
| i2c.r_ticks                     |     Ticks for read byte                                                                                                                                                                                       |
| i2c.w_ticks                     |     Ticks for write byte                                                                                                                                                                                      |
| i2c.s_ticks                     |     Ticks for start and address                                                                                                                                                                               |
| i2c.f_r_ticks                   |     Ticks for full read frame after the address is acked                                                                                                                                                      |
| i2c.f_w_ticks                   |     Ticks for full write frame                                                                                                                                                                                |
| spi.mode.init                   |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| spi.mode.disable                |     0:periph is enabled - 1:periph is disabled                                                                                                                                                                |
| spi.mode.cpha                   |     0:CK to 0 when idle - 1:CK to 1 when idle                                                                                                                                                                 |
| spi.mode.cpol                   |     0:the first clock transition is the first data capture edge - 1:the second clock transition is the first data capture edge                                                                                |
| spi.mode.if_type                |     Sets spi modes since slave cannot responds immediately - 0:access registers with spi - 1:preloads reg address to 0 for high speed tests - 2:echos SPI bytes - 3:always output user reg 0 (use for timing) |
| spi.mode.reg_16_bit             |     0:8 bit register access - 1:16 bit register access mode                                                                                                                                                   |
| spi.mode.reg_16_big_endian      |     0:little endian for 16 bit mode - 1:big endian for 16 bit mode                                                                                                                                            |
| spi.status.bsy                  |     Busy flag                                                                                                                                                                                                 |
| spi.status.ovr                  |     Overrun flag                                                                                                                                                                                              |
| spi.status.modf                 |     Mode fault                                                                                                                                                                                                |
| spi.status.udr                  |     Underrun flag                                                                                                                                                                                             |
| spi.status.clk                  |     0:sclk line low - 1:sclk line high                                                                                                                                                                        |
| spi.status.start_clk            |     SCLK reading at start of frame - 0:sclk line low - 1:sclk line high                                                                                                                                       |
| spi.status.end_clk              |     SCLK reading at end of frame - 0:sclk line low - 1:sclk line high                                                                                                                                         |
| spi.status.index_err            |     Register index error                                                                                                                                                                                      |
| spi.state                       |     Current state of the spi bus - 0:initialized - 1:NSS pin just lowered - 2:writing to reg - 3:reading reg - 4:transfering data - 5:NSS up and finished                                                     |
| spi.reg_index                   |     Current index of reg pointer                                                                                                                                                                              |
| spi.start_reg_index             |     Start index of reg pointer                                                                                                                                                                                |
| spi.r_count                     |     Last read frame byte count                                                                                                                                                                                |
| spi.w_count                     |     Last write frame byte count                                                                                                                                                                               |
| spi.transfer_count              |     The amount of bytes in the last transfer                                                                                                                                                                  |
| spi.frame_ticks                 |     Ticks per frame                                                                                                                                                                                           |
| spi.byte_ticks                  |     Ticks per byte                                                                                                                                                                                            |
| spi.prev_ticks                  |     Holder for previous byte ticks                                                                                                                                                                            |
| uart.mode.init                  |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| uart.mode.disable               |     0:periph is enabled - 1:periph is disabled                                                                                                                                                                |
| uart.mode.if_type               |     0:echos - 1:echos and adds one - 2:reads application registers - 3:constantly transmits                                                                                                                   |
| uart.mode.stop_bits             |     0:1 stop bit - 1:2 stop bits                                                                                                                                                                              |
| uart.mode.parity                |     0:no parity - 1:even parity - 2:odd parity                                                                                                                                                                |
| uart.mode.rts                   |     RTS pin state                                                                                                                                                                                             |
| uart.mode.data_bits             |     0:8 data bits - 1:7 data bits                                                                                                                                                                             |
| uart.baud                       |     Baudrate                                                                                                                                                                                                  |
| uart.mask_msb                   |     Masks the data coming in if 7 bit mode                                                                                                                                                                    |
| uart.rx_count                   |     Number of received bytes                                                                                                                                                                                  |
| uart.tx_count                   |     Number of transmitted bytes                                                                                                                                                                               |
| uart.status.cts                 |     CTS pin state                                                                                                                                                                                             |
| uart.status.pe                  |     Parity error                                                                                                                                                                                              |
| uart.status.fe                  |     Framing error                                                                                                                                                                                             |
| uart.status.nf                  |     Noise detected flag                                                                                                                                                                                       |
| uart.status.ore                 |     Overrun error                                                                                                                                                                                             |
| rtc.mode.init                   |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| rtc.mode.disable                |     0:periph is enabled - 1:periph is disabled                                                                                                                                                                |
| rtc.second                      |     Seconds of rtc                                                                                                                                                                                            |
| rtc.minute                      |     Minutes to set of rtc                                                                                                                                                                                     |
| rtc.hour                        |     Hours to set of rtc                                                                                                                                                                                       |
| rtc.day                         |     Days to set of rtc                                                                                                                                                                                        |
| rtc.set_second                  |     Seconds to set of rtc                                                                                                                                                                                     |
| rtc.set_minute                  |     Minutes to set of rtc                                                                                                                                                                                     |
| rtc.set_hour                    |     Hours to set of rtc                                                                                                                                                                                       |
| rtc.set_day                     |     Days to set of rtc                                                                                                                                                                                        |
| adc.mode.init                   |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| adc.mode.enable                 |     0:periph is disabled - 1:periph is enabled                                                                                                                                                                |
| adc.mode.fast_sample            |     0:slow sample rate - 1:fastest sample rate                                                                                                                                                                |
| adc.num_of_samples              |     Number of sample in the sum                                                                                                                                                                               |
| adc.counter                     |     Sum counter increases when available                                                                                                                                                                      |
| adc.index                       |     Sample index increases when new sample read                                                                                                                                                               |
| adc.sample                      |     Current 12 bit sample value                                                                                                                                                                               |
| adc.sum                         |     Sum of the last num_of_samples                                                                                                                                                                            |
| adc.current_sum                 |     Current collection of the sums                                                                                                                                                                            |
| pwm.mode.init                   |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| pwm.mode.disable                |     0:periph is enabled - 1:periph is disabled                                                                                                                                                                |
| pwm.duty_cycle                  |     The calculated duty cycle in percent/100                                                                                                                                                                  |
| pwm.period                      |     The calculated period in us                                                                                                                                                                               |
| pwm.h_ticks                     |     Settable high time in sys clock ticks                                                                                                                                                                     |
| pwm.l_ticks                     |     Settable low time in sys clock ticks                                                                                                                                                                      |
| dac.mode.init                   |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| dac.mode.disable                |     0:periph is enabled - 1:periph is disabled                                                                                                                                                                |
| dac.status                      |     Unimplemented status for padding                                                                                                                                                                          |
| dac.level                       |     The percent/100 of output level                                                                                                                                                                           |
| gpio[0].mode.init               |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| gpio[0].mode.io_type            |     0:high impedance input - 1:push pull output - 2:open drain output - 3:interrupts and saves event                                                                                                          |
| gpio[0].mode.level              |     If output sets gpio level - 0:low - 1:high                                                                                                                                                                |
| gpio[0].mode.pull               |     pull of the resistor - 0:none - 1:pullup - 2:pulldown                                                                                                                                                     |
| gpio[0].mode.tick_div           |     for trace tick divisor - max should be 16 for interface                                                                                                                                                   |
| gpio[0].status.level            |     The io level of the pin 0=low 1=high                                                                                                                                                                      |
| gpio[1].mode.init               |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| gpio[1].mode.io_type            |     0:high impedance input - 1:push pull output - 2:open drain output - 3:interrupts and saves event                                                                                                          |
| gpio[1].mode.level              |     If output sets gpio level - 0:low - 1:high                                                                                                                                                                |
| gpio[1].mode.pull               |     pull of the resistor - 0:none - 1:pullup - 2:pulldown                                                                                                                                                     |
| gpio[1].mode.tick_div           |     for trace tick divisor - max should be 16 for interface                                                                                                                                                   |
| gpio[1].status.level            |     The io level of the pin - 0:low - 1:high                                                                                                                                                                  |
| gpio[2].mode.init               |     0:periph will initialize on execute - 1:periph initialized                                                                                                                                                |
| gpio[2].mode.io_type            |     0:high impedance input - 1:push pull output - 2:open drain output - 3:interrupts and saves event                                                                                                          |
| gpio[2].mode.level              |     If output sets gpio level - 0:low - 1:high                                                                                                                                                                |
| gpio[2].mode.pull               |     pull of the resistor - 0:none - 1:pullup - 2:pulldown                                                                                                                                                     |
| gpio[2].mode.tick_div           |     for trace tick divisor - max should be 16 for interface                                                                                                                                                   |
| gpio[2].status                  |     The status of the GPIO                                                                                                                                                                                    |
| gpio[2].status.level            |     The io level of the pin - 0:low - 1:high                                                                                                                                                                  |
| trace.index                     |     Index of the current trace                                                                                                                                                                                |
| trace.tick_div                  |     The tick divisor of the event - max should be 16 for interface                                                                                                                                            |
| trace.source                    |     The event source of the event - 0:no source selected - 1:DEBUG0 pin - 2:DEBUG1 pin - 3:DEBUG2 pin                                                                                                         |
| trace.value                     |     The value of the event - 0:falling edge interrupt - 1:rising edge interrupt                                                                                                                               |
| trace.tick                      |     The tick when the event occured                                                                                                                                                                           |


<a name="proto"></a>
## [PHiLIP Serial Protocol](#c_proto)
The communication medium for PHiLIP is a serial connection.It provides basic instructions that allows for extensibility from other interfaces or basic communication.
Replies are given in standard json format with a `"result"` that corresponds to a `errno` code and `"data"` if data is present.
For example reading bytes from a register would respond with `{"data":[0,1,2,3,4,5,6,7,8,9], "result":0}`

Name           | Command                        | Description                                          | Example             | Example Description
---------------|--------------------------------|------------------------------------------------------|---------------------|------------------------------------------------------------------------------
READ_REG_CMD   | `rr <index> <size_of_bytes>`   | Read application registers                           | `rr 0 10`           | Reads 10 bytes starting at register 0
WRITE_REG_CMD  | `wr <index> [data0 ... datan]` | Write application registers                          | `wr 10 99 88 77`    | Writes 99, 88 and 77 starting at register 10
EXECUTE_CMD    | `ex`                           | Execute and commit changes in the registers          |                     |
RESET_CMD      | `mcu_rst`                      | Provide a software reset to PHiLIP                   |                     |
VERSION_CMD    | `-v`                           | Prints the version of the interface                  |                     |
HELP_CMD       | `help`                         | Prints a help menu                                   |                     |
MEMORY_MAP_CMD | `mm <record_index>`            | Gives properties of the memory map for a given index | `mm 0`              | Gives the properties of the first entry of the memory map
MM_SIZE_CMD    | `mm_size`                      | Gives the amount of records in the memory map        |                     |
READ_KEY_CMD   | `r <reg_name> [array_index]`   | Reads values given a record name                     | `r user_reg 2`      | Reads the 3rd value of the user_reg
WRITE_KEY_CMD  | `w <reg_name> <data>`          | Writes a value to a record name                      | `w i2c.mode.init 0` | Writes 0 to the i2c mode bit causing it to reinitialize the next `ex` command



<a name="inter"></a>
## [PHiLIP Python Interface](#c_inter)
PHiLIP has a python interface called [philip_pal](IF/philip_pal) which is available with `pip3 install philip_pal`.
It provides a philip_shell which developers can use for manual tests and interacting with PHiLIP.
For automated scripts, a Phil() class is provided.

For the API check the docstring of [philip_shell.py](IF/philip_pal/philip_pal/philip_shell.py) or [philip_if.py](IF/philip_pal/philip_pal/philip_if.py)

<a name="pinb"></a>
## [PHiLIP-b Pinout](#c_pinb)
Pinout for the PHiLIP on the [bluepill](https://hackaday.com/2017/03/30/the-2-32-bit-arduino-with-debugging/)

<a href="RESOURCES/PHiLIP-BLUEPILL-PINOUT.png">
    <img src="RESOURCES/PHiLIP-BLUEPILL-PINOUT.png" alt="drawing" width="1024px"/>
</a>

<a name="pinn"></a>
## [PHiLIP-n Pinout](#c_pinn)
Pinout for the PHiLIP on the [nucleo-f103rb](https://www.digikey.com/products/en?keywords=nucleo-f103rb)

<a href="RESOURCES/PHiLIP-NUCLEOF103RB-PINOUT.png">
    <img src="RESOURCES/PHiLIP-NUCLEOF103RB-PINOUT.png" alt="drawing" width="1024px"/>
</a>
