#!/bin/sh
#
# script file to start network
#
# Usage: init.sh {gw | ap} {all | bridge | wan}
#




sysconf init $*
telnet &
flash set HW_WLAN0_REG_DOMAIN 14

flash set HW_WLAN1_REG_DOMAIN 14
UDPServer &

