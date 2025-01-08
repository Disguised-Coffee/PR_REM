# Project Room Environment Module

*because why not?*

Project Room Environment Module ***(PR_REM)*** is a Arduino-based project meant to deep dive into Particle's PaaS environment with a goofy focus. The Module tracks environmental data such as temperature, humidity, and light level for analysis through a 3rd party web client application that is also able to alter the light settings of the room via the module. 

## 🔧 Hardware/Sensors
> *Most hardware sourced from* **[Elegoo Arduino Super Starter Kit](https://www.elegoo.com/blogs/arduino-projects/elegoo-uno-project-super-starter-kit-tutorial)** 

- __Arduino Uno R3__ (Elegoo copy)
- [__Particle Argon__](https://docs.particle.io/argon/)
- Photoresistor
- 16x2 LCD Screen
- 10k Ohm Potentiometer
- Switch buttons
- DHT-11 Sensor with simplified board
- Light [type is TBD]

## 🚀 Software Stack

- 🔷 **Particle** ~ IoT PaaS that handles internet interactions such as sending data to 3rd party databases
- 📄 **Google Sheets** ~ Google's spreadsheet platform, used to place data (in other words, temporary database). Uses ___Google Apps Scripts___ for handling data posts.
- 🟩 **Arduino** ~ Popular, public domain microcontroller environment to prototype electronic designs easily

## 🔧 Running the Project

> Dedicated Instructables page will be made a few weeks after project conclusion
