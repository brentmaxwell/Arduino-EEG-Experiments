# Arduino-EEG-Experiments Notebook
This code is the messiest I've ever written. It will be cleaned up

I ran across an article titled "Identifying Ketamine Responses in Treatment-Resistant Depression Using a Wearable Forehead EEG" (https://arxiv.org/abs/1805.11446), and as I do periodic infusions for depression, I was interested to see if I could measure my own response via a DIY EEG band. I had run across the Mikroe EEG Click (https://www.mikroe.com/eeg-click) board on SparkFun, and after doing some research, found it to be the easiest to implement: only 3 pins are required: +V, GND, and Analog.

I started with a Arduino Uno R3 and a simple sketch that just read the analog pin on loop. After more research, I realized that it was more complicated than that, and thus my dive into this began.

## Features
- Rechargable battery
- Serial output
- Log to file via microSD
- output to Bluetooth LE for real-time visualization
- status LED
- RTC for timestamping files
- FFT for visualization
- button to "mark" events
- simple serial/ble command interface
- can be attached as a Mass Storage device (hold button while plugging in to USB)

## Parts that I used
- Adafruit ItsyBitsy nRF52840 Express (https://www.adafruit.com/product/4481)
- Adafruit LiIon/LiPoly Backpack Add-On for Pro Trinket/ItsyBitsy (https://www.adafruit.com/product/2124)
- Lithium Ion Polymer Battery - 3.7v 1200mAh (https://www.adafruit.com/product/258)
- Adafruit Adalogger FeatherWing (https://www.adafruit.com/product/2922)
- Mikroe EEG Click (https://www.mikroe.com/eeg-click)
- Cup electrodes and leads (can be found cheaper elsewhere, but I didn't want to wait: https://www.amazon.com/gp/product/B012BTTXI2/)
- 1/8" TRS plug (only need one, but I got these to use in other projects: https://www.amazon.com/gp/product/B077XNTSF9/)
- 3x tiny breadboard (https://www.adafruit.com/product/65)
- 1x Tactile Switch Button (12mm square, 6mm tall) (something like this: https://www.adafruit.com/product/1119)
- Random wire sourced from extra CAT5
- Headband (this one in particular folds over perfectly to put the parts in: https://www.amazon.com/gp/product/B08S384T5Z/)

## Circuit
![Schematic](/Schematic/EEG%20schematic.png)
Missing from the image is the connction to the Adafruit LiIon/LiPoly Backpack Add-On. This was soldered to the ItsyBitsy, connecting the BAT, GND, and 5V pins of the Backpack to the BAT, GND, and USB pins of the ItsyBitsy. Tip: If you want a power switch, solder it to the Backpack BEFORE soldering it to the ItsyBitsy; I learned the hard way.

## Code
TODO

## Software
- EDFBrowser (https://www.teuniz.net/edfbrowser/)
- MNE Tools (https://mne.tools/stable/index.html)

## Analysis of data
TODO
