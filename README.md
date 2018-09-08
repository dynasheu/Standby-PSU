# Standby-PSU
Small sandby PSU for low power applications.


## standby-psu-test
This sketch is for debugging and testing on arduino. Uses normal IRremote.h library.


## standby-psu-master
This sketch is for use with ATTiny.
Uses tiny_IRremote.h and SendOnlySoftwareSerial.h libraries.

I/O:
- button
- led
- relay
- ir
- serial out

PCB and components coming soon

## standby-psu-slave
Comming soon.

This sketch is for use with ATTiny.
Uses ReceiveOnlySoftwareSerial.h library.

I/O:
- button
- led
- relay
- mute relay
- serial in

Same PCB as master. 