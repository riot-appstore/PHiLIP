# Testing
This is intended to provide tools for embedded testing.


# Setting up the PHiLIP (Primitive Hardware In the Loop Integration Product)

## Tools
- SWD flasher
- Soldering iron and solder

## Materials
- [bluepill](https://hackaday.com/2017/03/30/the-2-32-bit-arduino-with-debugging/) or [nucelo-f103rb](https://www.digikey.com/products/en?keywords=nucleo-f103rb)

## Flashing bluepill
1. Solder pins on the bluepill and secure the USB connector of not already done.
2. Download [firmware](QUALIFICATION/FW/) from the QUALIFICATION/FW folder.
3. Flash using an [SWD or JTAG interface](https://satoshinm.github.io/blog/171212_stm32_blue_pill_arm_development_board_first_look_bare_metal_programming.html).  (I usually like using the Nucleo debugger/flasher since it is cheap, has USB to Serial and support drag and drop binary).
4. If custom implementation is desired open source firmware is available in the [PHiLIP](FW/) folder.

## Flashing nucleo-f103rb
1. Download PHiLIP-nucleo103rb* [firmware](QUALIFIED_FW) from the QUALIFIED_FW folder.
2. Drag and drop the .bin file to flash

# bluepill Pinout
<a href="RESOURCES/PHiLIP-BLUEPILL-PINOUT.png">
    <img src="RESOURCES/PHiLIP-BLUEPILL-PINOUT.png" alt="drawing" width="720px"/>
</a>

# nucleo-f103rb Pinout
<a href="RESOURCES/PHiLIP-NUCLEOF103RB-PINOUT.png">
    <img src="RESOURCES/PHiLIP-NUCLEOF103RB-PINOUT.png" alt="drawing" width="720px"/>
</a>

# PHiLIP Misc Info
- I2C slave address is 85 or 0x55
- Total registers are 256
- Upper registers can be read or written (152+)
- sys register is locked
- For changing configuration the registers can be set then the ex command must be used to execute changes
