PHiLIP PAL (PHiLIP Protocol Abstraction Layer)
================================

## Description

A set of python modules that abstract away and standardize shell based command for bare metal memory map access.
PHiLIP PAL is used to interface to a memory map specified from a csv or by the device.
It handles the parsing of offsets, sizes, etc. of the device.

To use this interface a memory must be provided, by default the package contains the PHiLIP memory map for all the versions.
If a development version of PHiLIP is used and a memory map has not been released, the memory map can be scanned from the device.

PHiLIP PAL provides a philip_shell which developers can use for manual tests and interacting with PHiLIP.
For automated scripts, a Phil() class is provided.

For the API check the docstring of [philip_shell.py](IF/philip_pal/philip_pal/philip_shell.py) or [philip_if.py](IF/philip_pal/philip_pal/philip_if.py)

### PHiLIP PAL Installation
To interface to PHiLIP first install the philip_pal for python3.

`sudo pip3 install philip_pal`

### Running PHiLIP PAL Shell
First connected PHiLIP to the computer so a serial port connection is available.

This script handles interfacing to the PHiLIP device.  It parses data and
exposes from the PhilipExtIf class to be run in a shell.

The pip installation installs a script that can be run with `philip_shell`

The purpose of this script is allow easy setup and manual usage of PHiLIP.

Usage
-----

```
usage: philip_shell.py  [-h]
                        [--loglevel {debug,info,warning,error,fatal,critical}]
                        [--port PORT]
                        [--filter-data]

optional arguments:
  -h, --help            show this help message and exit
  --loglevel {debug,info,warning,error,fatal,critical}
                        Python logger log level (default: warning)
  --port, -p
                        Specify the serial port
  --data_only, -do
                        Filters data from philip responses to only display
                        what is needed (default: False)
```

Upon starting the shell use the `help` command to see what functionality is available.

See the main page for more getting started and examples.
