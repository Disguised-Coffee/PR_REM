# Particle/PR_REM

This firmware project was created using [Particle Developer Tools](https://www.particle.io/developer-tools/) and is compatible with all [Particle Devices](https://www.particle.io/devices/).

Parts of this README.md file have been modified from its original template.

## Table of Contents
- [Introduction](#introduction)
- [Prerequisites To Use This Template](#prerequisites-to-use-this-repository)
- [Getting Started](#getting-started)


## Introduction

For an in-depth understanding of this project template, please refer to our [documentation](https://docs.particle.io/firmware/best-practices/firmware-template/).

## Prerequisites To Use This Repository

To use this software/firmware on a device, you'll need:

- A [Particle Device](https://www.particle.io/devices/).
- Windows/Mac/Linux for building the software and flashing it to a device.
- [Particle Development Tools](https://docs.particle.io/getting-started/developer-tools/developer-tools/) installed and set up on your computer.
- Optionally, **a nice cup of coffee and a soundtrack**.

## Getting Started

1. While not essential, we recommend running the [device setup process](https://setup.particle.io/) on your Particle device first. This ensures your device's firmware is up-to-date and you have a solid baseline to start from.

2. If you haven't already, open this project in Visual Studio Code (File -> Open Folder). Then [compile and flash](https://docs.particle.io/getting-started/developer-tools/workbench/#cloud-build-and-flash) your device. Ensure your device's USB port is connected to your computer.

3. Verify the device's operation by monitoring its logging output:
    - In Visual Studio Code with the Particle Plugin, open the [command palette](https://docs.particle.io/getting-started/developer-tools/workbench/#particle-commands) and choose "Particle: Serial Monitor".
    - Or, using the Particle CLI, execute:
    ```
    particle serial monitor --follow
    ```
4. As with I mentioned in `Arduino/PR_REM_Peripheral`, wire the whole project *(picture will be coming soon at some point of the project's conclusion)*