# Smart Touchscreen DRO for Milling Machines
__As seen in this article in [*Servo* magazine](https://www.timpaterson.com/articles/Servo/SmartDroP1_P2.pdf)!__

![Screen](https://raw.githubusercontent.com/TimPaterson/TouchscreenDigitalReadout/master/ReadmeImages/Screen.jpg)

![Machine](https://raw.githubusercontent.com/TimPaterson/TouchscreenDigitalReadout/master/ReadmeImages/Machine.jpg)
![Back view](https://raw.githubusercontent.com/TimPaterson/TouchscreenDigitalReadout/master/ReadmeImages/Back.jpg)

### Introduction
The Smart DRO is a touchscreen digital readout for milling machines. You 
can see in the photos an installation on a Grizzly mill/drill, replacing 
the original display head (shown on the mill table). The OEM position 
sensors for the X, Y, and Z axes directly connect to the back of the unit.
Position sensors like these are individually available commercially for 
mills that do not have them factory installed.

The Smart DRO can greatly simplify milling from dimensioned drawings. If 
you enter the diameter of the tool it can automatically adjust the
display reading by the tool radius. This allows you to cut at the specified 
dimension without having to add or subtract the radius offset. Note in the 
lower left corner of the main screen (the dark rectangle with a circle on 
each side): you can select which side you're cutting so it knows whether to
add or subtract the cutter radius.

You can enter the diameter of each tool as you use it, but the Smart DRO
can also keep your entire tool library (up to 500 tools) in non-volatile
memory. You can enter the list manually or import it from a flash drive
if you share it with other programs or devices. The Smart DRO will also
calculate feeds and speeds, and it displays the current feed rate when
any of the axes move.

Another helpful feature is the undo list for changes in the origin of any
axis. If you accidently zero out or set a value to the wrong axis, you can
easily recover the origin even if the axis has been moved.

The on-screen calculator allows you to include axis readings in 
calculations, and copy results back to an axis.

### Sensor Compatibility
It is industry standard that position sensors use a 9-pin "D" connector
(like a PC serial port). However, there is no standard for the pin
assignments for the connector! To solve this, the Smart DRO uses a pinout
adapter for each axis. A tiny printed circuit board with a single connector
routes signals to the correct pins. A set of 3 of these PCBs costs just 45 cents.
See the [*Servo* magazine article](https://www.timpaterson.com/articles/Servo/SmartDroP1_P2.pdf)
for more info.

### The Project
The main component is the 10" LCD touchscreen, specified in the parts 
list. This repository includes all other aspects of the hardware design 
needed to build this project as shown. These elements include:
- Custom [electronics design](https://github.com/TimPaterson/TouchscreenDigitalReadout/tree/master/Electronics),
with printed circuit board ready for fabrication.
- Enclosure design, using a 3D-printed bezel and machined aluminum back.

A [separate repository](https://github.com/TimPaterson/Touchscreen-DRO-Firmware) 
has the microcontroller firmware and graphics images.

Here are the critical links:
- The [*Servo* magazine article](https://www.timpaterson.com/articles/Servo/SmartDroP1_P2.pdf)
has complete instructions on how to build it.
- The [Touchscreen-DRO-Firmware](https://github.com/TimPaterson/Touchscreen-DRO-Firmware) repository
has firmware and everything else you need, bundled into a release.
- [Touchscreen at BuyDisplay.com](https://www.buydisplay.com/serial-spi-i2c-10-1-inch-tft-lcd-module-dislay-w-ra8876-optl-touch-panel).
Be sure to select options per the [parts list](https://github.com/TimPaterson/TouchscreenDigitalReadout/blob/master/Electronics/DroPartsList.pdf).
- [Main PCB shared at OSH Park](https://oshpark.com/shared_projects/AQX5MSFg). The
EAGLE CAD for the PCB is included is you want to use another PCB source.
- Pinout PCBs at OSH Park. Pinouts are specified by a 4-digit number where
the digits are the pin numbers on the 9-pin D-connector for GND, +5V, A, & B, respectively.
  - [2768 (Grizzly)](https://oshpark.com/shared_projects/TLEyl9I6)
  - [2134 (Jing/Jcgs)](https://oshpark.com/shared_projects/ilViDS0E)
  - [6724 (Acu-Rite)](https://oshpark.com/shared_projects/GIDBOB83)
- [Tool Library Manager](https://github.com/TimPaterson/CNC-Tool-Library-Manager)
for making tool libary files the Smart DRO can import.
