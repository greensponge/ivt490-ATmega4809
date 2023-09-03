# Description
For this project I've soldered some wires from a `Arduino Uno WiFi Rev2` onto my exhaust air heat pump system. 
I've cobbled together this solution using guides from some Swedish pump forums and other similar projects here on Github. 

# Problem
I wanted to capture data from my IVT-490 pump and store it in a Postgres database, make some fancy graphs and generally monitor what's going on. Initially I thought about using a Raspberry PI for this project, 
however it is non-trivial to use this board because the TTL on a Raspberry is 3.3V, which means you need to protect the pins if you're planning on receiving a higher voltage. 

It's certainly doable, but it was too much of a hassle for me since Raspberrys are stupid expensive at this point in time, and if I'm buying a new board anyway then it might as well use the right TTL to begin with.
The IVT-490 TTL signals are 5V. Because of this I opted to use an Arduino board which typically has 5V TTL for its boards AFAIK. I didn't want to use network cables so I needed a board that had WiFi included,
which resulted in me ending up with a `Arduino Uno WiFi Rev2`. This board is perfect for doing this kind of loop and sending a payload somewhere.

# Getting started
This script should work out of the box as long as you replace the server `IP` to your endpoint as well as the arbitrary name `/flp` I've used for mine. I've added comments where you need to update. 
You need to make your own endpoint that saves this data somewhere. Take the request and parse it as json and insert it into your favorite database which contains a table that has all the columns that you want to capture.

To get it up and running:
1. Download Arduino IDE, unless you know what you're doing. It should be possible for example to use `CLion` as well.
2. Compile & Upload the script to a `ATmega4809` board.
3. Activate administrator mode on the `IVT-490` by holding the button on the right for ~10 seconds, navigating to "Installation and Operation", selecting and activating "Variable dumping to PC".
4. Solder the wires following the images that can be found here: https://www.varmepumpsforum.com/vpforum/index.php?topic=27306.15
It's `RxD` (receiving) and `TxD` (transmitting) and `GND` (ground) that you want to deal with. Maybe `VCC` (power out, 5V) could be used to power something, but it's not enough to power the `Arduino Uno WiFi Rev2` reliably.
I opted to use an external power supply to get the correct voltage for this board.

# Issues
If you have problems with the script or want to ask a question you can always create a new issue in this repository and I'll take a look.

# Inspiration
https://www.varmepumpsforum.com/vpforum/index.php?topic=27306.15
https://www.tsoft.se/wp/2015/03/08/overvakning-av-min-ivt-490-varmepump-med-raspberry-pi/
https://github.com/mskorge/ivt490-esp8266/blob/master/IVT-ArduinoCore/IVT-ArduinoCore.ino
