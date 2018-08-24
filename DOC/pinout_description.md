# Pin Description

PIN NAME  | DESCRIPTION                                                                                            | IMPLEMENTED
----------|--------------------------------------------------------------------------------------------------------|----------------------
DUT_RST   | Connects to the reset pin of the DUT, can put  it in a reset                                           | <ul><li>[x]</li></ul>
USER_BTN  | User button if a test requires manual interaction, the pin can also be automated but the RPi in the CI | <ul><li>[ ]</li></ul>
TEST_FAIL | Goes high if a low level test failed                                                                   | <ul><li>[ ]</li></ul>
TEST_WARN | Goes high if a low level test has a warning                                                            | <ul><li>[ ]</li></ul>
TEST_PASS | Goes high if a low level test passed                                                                   | <ul><li>[ ]</li></ul>
DEBUG0    | A GPIO debug pin                                                                                       | <ul><li>[ ]</li></ul>
DEBUG1    | A GPIO debug pin                                                                                       | <ul><li>[ ]</li></ul>
DEBUG2    | A GPIO debug pin                                                                                       | <ul><li>[ ]</li></ul>
LED0      | Heartbeat connection to bluepill LED                                                                   | <ul><li>[x]</li></ul>
PM_V_ADC  | Samples the voltage of the DUT (only when connected in external power mode)                            | <ul><li>[ ]</li></ul>
PM_HI_ADC | Course, mA range of current measurement (only when connected in external power mode)                   | <ul><li>[ ]</li></ul>
PM_LO_ADC | Fine, uA range of current measurement  (only when connected in external power mode)                    | <ul><li>[ ]</li></ul>
DUT_IC    | Input capture pin, this is used for timing measurements from the DUT                                   | <ul><li>[ ]</li></ul>
DUT_PWM   | Feeds a pwm signal to the DUT to confirm DUT receives correct timing                                   | <ul><li>[ ]</li></ul>
DUT_ADC   | Used to measure any analog out signals of the DUT                                                      | <ul><li>[ ]</li></ul>
DUT_RX    | The UART receive pin, connect to the DUT's TX pin                                                      | <ul><li>[x]</li></ul>
DUT_TX    | The UART receive pin, connect to the DUT's RX pin                                                      | <ul><li>[x]</li></ul>
DUT_RTS   | The UART Ready To Send pin, connect to the DUT's RTS pin                                               | <ul><li>[x]</li></ul>
DUT_CTS   | The UART Clear To Send pin, connect to the DUT's CTS pin                                               | <ul><li>[x]</li></ul>
DUT_NSS   | SPI Chip Select                                                                                        | <ul><li>[ ]</li></ul>
DUT_SCK   | SPI Clock                                                                                              | <ul><li>[ ]</li></ul>
DUT_MISO  | SPI Master In Slave Out                                                                                | <ul><li>[ ]</li></ul>
DUT_MOSI  | SPI Master Out Slave In                                                                                | <ul><li>[ ]</li></ul>
DUT_SCL   | I2C Clock                                                                                              | <ul><li>[x]</li></ul>
DUT_SDA   | I2C Data                                                                                               | <ul><li>[x]</li></ul>
