# Piano Network Protocol (PNP) specification

## Introduction
The PNP is responsible for hammer event data, streaming hammer samples, and configuration.

Data is communicated via UDP Ethernet.

Continuously sending all hammer samples for 88 keys results in unnecessary network traffic. Therefore, only send samples for hammer samples above a threshold.

## Requirements
* Send hammer event packets from XPS to PPS.
* Send hammer samples packets from XPS to PVA.
* Send configuration from PPS or PVA to XPS.

## Configuration
* Downsample rate of hammer samples.  Stream samples at the ADC rate divided by the downsample rate.

## Details

### Common Header

* The first two bytes are common for all packets.
  | Symbol       | Bits | Description             |
  |--------------|------|-------------------------|
  |              |      |                         |
  | Reserved     | 8    | Reserved                |
  |              |      |                         |
  | Reserved     | 2    | Reserved                |
  | Board Number | 4    | There are 11 XPS boards|
  | Packet Type  | 2    | 11 = Hammer samples<br> 10 = Hammer event<br> 0X = Future |

### Hammer Event Packet

* The following 2 bytes repeat for input 7, then 6, then 5, then.... 0.
  | Symbol       | Bits | Description             |
  |--------------|------|-------------------------|
  | Reserved     | 6    | Reserved                |
  | Event Type   | 2    | 11 = Strike<br> 10 = Release<br> 01 = Future<br> 00 = No Event |
  | Velocity     | 8    | 0 = min, 255 = max |
* Total size is 2 bytes for header plus 2 * 8 for event data = 18 bytes.

### Hammer Samples Packet

* When a hammer value exceeds 25% of ADC maximum value, start sending samples beginning approximately 50 milliseconds prior.
* As long as the hammer value is above 25%, continue sending the samples.
* When the hammer value falls below 25%, continue sending hammer samples for another approximately 50 milliseconds.
* For hammer samples in the packet that do not meet the threshold, send zero.
* The following repeats for input 7, then 6, then 5, then.... 0.
  | Symbol       | Bits | Description             |
  |--------------|------|-------------------------|
  | Sample       | 24   | Hammer samples |
* Total size is 2 bytes for header plus 3 * 8 for samples = 26 bytes.