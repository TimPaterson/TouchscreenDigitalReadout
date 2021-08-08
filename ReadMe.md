# Smart Touchscreen DRO for Milling Machines
__Coming Soon!__ A two-part article on this project will appear in upcoming
issues of [*Servo* magazine](https://www.servomagazine.com/).

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
the X or Y axes move.

Another helpful feature is the undo list for changes in the origin of any
axis. If you accidently zero out or set a value to the wrong axis, you can
easily recover the origin even if the axis has been moved.

The on-screen calculator allows you to include axis readings in 
calculations, and copy results back to an axis.

### The Project
The main component is the 10" LCD touchscreen, specified in the parts 
list. This repository includes all other aspects of the hardware and 
software design needed to build this project as shown. These elements 
include:

+ Custom [electronics design](https://github.com/TimPaterson/TouchscreenDigitalReadout/tree/master/Electronics),
with printed circuit board ready for fabrication.
+ Complete firmware for the microcontoller unit.
+ All screen images.
+ Enclosure design, using a 3D-printed bezel and machined aluminum back.
+ All of the customized tools needed to generate binary files.

The project Release includes all binary files as well as schematic, parts
list, etc. If you want to make one of these as-is, there is no reason to
figure out the software build process, or even clone the repository. The
contents of the Release (SmartDro.zip) is the same as the 
[Release folder](https://github.com/TimPaterson/TouchscreenDigitalReadout/tree/master/Release).

__More to come!__ But in the meantime, here are some critical links:
+ [Touchscreen at BuyDisplay.com](https://www.buydisplay.com/serial-spi-i2c-10-1-inch-tft-lcd-module-dislay-w-ra8876-optl-touch-panel).
Be sure to select options per the [parts list](https://github.com/TimPaterson/TouchscreenDigitalReadout/blob/master/Electronics/DroPartsList.pdf).
+ [Main PCB shared at OSH Park](https://oshpark.com/shared_projects/AQX5MSFg).
+ [Pinout PCB at OSH Park](https://oshpark.com/shared_projects/TLEyl9I6).
+ [Tool Library Manager](https://github.com/TimPaterson/CNC-Tool-Library-Manager)
for making tool libary files the Smart DRO can import.
