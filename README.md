# Arduino-EEG-Experiments Notebook
This code is the messiest I've ever written. It will be cleaned up

I ran across an article titled "Identifying Ketamine Responses in Treatment-Resistant Depression Using a Wearable Forehead EEG" (https://arxiv.org/abs/1805.11446), and as I do periodic infusions for depression, I was interested to see if I could measure my own response via a DIY EEG band. I had run across the Mikroe EEG Click (https://www.mikroe.com/eeg-click) board on SparkFun, and after doing some research, found it to be the easiest to implement: only 3 pins are required: +V, GND, and Analog.

I started with a Arduino Uno R3 and a simple sketch that just read the analog pin on loop. After more research, I realized that it was more complicated than that, and thus my dive into this began.

## Features
- Rechargable battery
- status LED
- Log to file via microSD
- RTC for timestamping files
- button to "mark" events
- FFT for visualization
- Serial output
- output to Bluetooth LE for real-time visualization
- simple serial/ble command interface
- can be attached as a Mass Storage device (hold button while plugging in to USB)

## Version History
The first version was an Arduino Uno, a protoshield, a TFT display shield, and a Mikroe EEG Click with what are really ECG leads and sticky electrodes. This worked fine, but the code kept crashing above 128 Hz, and it was bulky.

For the second version, I decided to use a smaller microcontroller, skip the display, and put it in a headband. I started with the Adafruit ItsyBitsy nRF52840 Express, but moved to the Feather nRF52840 Express so I could use the Adalogger Featherwing to capture the data.

## Build

### Hardware
- Adafruit Feather nRF52840 Express (https://www.adafruit.com/product/4062)
- Stacking headers for Feather (https://www.adafruit.com/product/2830)
- Adafruit Adalogger FeatherWing (https://www.adafruit.com/product/2922)
- Mikroe EEG Click (https://www.mikroe.com/eeg-click)
- 2x tiny breadboard (https://www.adafruit.com/product/65)
- Random wire sourced from extra CAT5
- Cup electrodes and leads (can be found cheaper elsewhere, but I didn't want to wait: https://www.amazon.com/gp/product/B012BTTXI2/)
- 1/8" TRS plug (only need one, but I got these to use in other projects: https://www.amazon.com/gp/product/B077XNTSF9/)
- 1x Tactile Switch Button (12mm square, 6mm tall) (something like this: https://www.adafruit.com/product/1119)
- Headband (this one in particular folds over perfectly to put the parts in: https://www.amazon.com/gp/product/B08S384T5Z/)
- Random tiny switch I had
- Lithium Ion Polymer Battery - 3.7v 500mAh (https://www.adafruit.com/product/1578)

## Assembly

### Electronics
After soldering the headers on the Adalogger and the stacking headers on the Feather, I marked the pin locations I needed on the breadboards. One breadboard is for the Feather/Adalogger, the other is for the Mikroe EEG Click.

For the Feather/Adalogger, I marked the locations for 3.3v, GND, and A0, which all need to be connected to the EEG Click. On the other breadboard, I marked the Vin, GND, and AOut pins.

In additon, I added jumpers from the 3.3v and D13 pins to unconnected points on the Feather breadboard for the tactile switch, to be used to "mark" events that happen during monitoring.

Once I had this, I measured out some wire to connect the Feather board and the EEG Click. My plan was to have the Feather board in the headband on one side of my head, and the EEG Click on the other, so I cut the wires to be about 2/3 the way around my head, for a bit extra.

Since I used leftover Cat5, I kept one pair twisted, and twisted in a third wire to create a 3 conductor "cable". After stripping the ends, I inserted the matching pair (orange and orange/white in my case) into the 3.3v and GND points on each breadboard, and the other wire in the A0 pin and Aout pins. To help hold the wires in place, I used the holes closest to the center of the breadboard, and then used a liberal amount of electrical tape to hold them in place, ensuring that the outer holes of the breadboard were still open to insert the boards.

I then mounted the tactile switch on the outer edge of the board, ensuring that the pins were in the same busses as the jumpers I placed earlier. It didn't stay very well, but once I placed the Feather on, the battery connector seemed to hold it adaquately.

Now that the breadboards were finished, I went ahead and placed the boards on their respective breadboards. For the battery, I cut the positive wire, and soldered in a simple mini switch I had to give it a power switch. The battery I purchased also slid nicely in the space between the Feather and the Adalogger.

### Headband

The entire circuit, from electrodes to EEG Click to Feather will start on the forehead, and travel to the right, around the back of the head, to the Feather on the left. There is a seam on the headband, and in order to not have to deal with it, I decided it would be placed slightly to the left of my forehead.

The headband is folded over twice, to give it a sort of pocket where the electronics will be housed. In order to make it more comfortable to wear, I decided to put the opening on the outside, and sew it up at the end.

The first step in putting it all together is to find the location for the electrodes. I mounted the electrodes about 1 - 1.5 cm above the bottom of the band. Horizontally, I measured from the seam, to the right, 5 cm for the first one, and then following the 10-20 positioning, and the circumfrence of my head, 33 mm between the next two.

On the other side of the seam, I placed the Feather board, and then ran the "cable" around the back, and placed the EEG Click board on the other side of the band. I wrapped the flaps of the headband over everything, and to check positioning, I carefully placed it on my head. I adjusted the position of the boards, and placed safety pins around them to hold them in place for the moment.

At this point, it was time to put together the electrodes and leads. In each spot that I marked for the electrodes, I cut a tiny hole, inserted the back of the electrode, and clipped in the lead. The connectors on these leads were not the kind I wanted, so I carefully measured, leaving some slack, to the Mikroe board, and cut the leads. I then stripped and soldered these leads to the 1/8" TRS plug, making sure that the lead to the middle electrode was connected to the sleeve.

The final step was to sew the headband closed, leaving openings at the front for adjusting the electrodes, and over the Feather to access the MicroUSB port and microSD card.

## Code
TODO

## Software
- EDFBrowser (https://www.teuniz.net/edfbrowser/)
- MNE Tools (https://mne.tools/stable/index.html)

## Analysis of data
TODO
