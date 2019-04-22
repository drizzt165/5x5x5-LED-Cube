# Description:
This project was based off various similar projects that can be found all over the internet, but with some additions and changes I wanted to make. The process started during winter break after the Fall 2018 semester and finished a few months later as I worked on it whenever I wasn't bogged down by course work. 

The project features 125 individually addressable LED's soldered together with tinned copper wire. Aesthetically, this wire is pleasing but it does pose the risk of easily shorting and causing some LED's explode; however, this is a rare occurrence and won't happen unless it is being heavily tinkered with during operation. I have been also attempting to setup up an app to be able to control the animations via Bluetooth but have not been able to get to working quite to my standard yet.

### Final Construction of the Cube
<p align="center">
  <img alt = "Smile" src="https://github.com/drizzt165/5x5x5-LED-Cube/blob/master/images/FinalConstruction.jpg">
</p>


# Challenges:
Many challenges arose thorugh the process of creating this project. Most notably in the software aspect of the project. The physical portion had some of it's own challenges; however, it was essentially just a big soldering practice. The PL2983 individually adressable LED's used in the project have very strict timing requirements. After measure with an oscilloscope, it was found that each signal for each LED is about 42 micro seconds. The more common individually adressable WS2812 LED's have a delay of about 30 micro seconds per LED. When I originally programmed the arduino using the FastLED library, I was using the timeing for the latter of the two. That 12 microsecond difference ended up being a headache of mine for a couple weeks as I did troubleshooting to figure out why my precious LED's were suddenly flickering and not acting as I wanted.

### Top LED's were acting abnormal here
<p align="center">
  <img alt = "Smile" src="https://github.com/drizzt165/5x5x5-LED-Cube/blob/master/images/FinalConstruction.jpg">
</p>
