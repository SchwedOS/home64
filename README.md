# Home64 – make your C64 Userport available as a smart home device

This software uses the network interface of Ultimate-64 computer or UltimateII+ cartridge for C64.
Via network the C64 Userport Pins PB0..PB7 can be accessed. The Port Pins are controlled low active. That means a low level on PB0..7 is assumed as active state for driven relays or power stages.
For controlling the user port as a smart home device you need the home64 program running on C64 and the sample server application for running a kind of gateway between your smart home and C64 on a Linux x64/Arm64 or Windows computer. “Server.pcc” can handle up to ten C64 instances running home64.
The sample server application parses http requests from your smart home system through C64 instances.
This gateway server software is necessary because currently Ultimate64/II+ does not support listener mode on the network interface and therefore cannot act directly as a home device (server).

# Software Package contains:
- home64.d64:	ready to use C64-d64 image and all source code for cc65 compiler
- server.pcc:	gateway software example implemented as a pico-c script
- picoc:		c-interpreter executables for Linux Arm64/X64 and Windows

For further information please read the "home64.docx" included in this package.
For compilation information please go into Makefile.
