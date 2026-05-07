# IEEEPowerKit

## Section 1: Developing Arduino Code for Your Board

Distributed Power Kits already have the Arduino DFU bootloader installed, so STM32CubeProgrammer is not needed. You can upload your own Arduino sketches directly over USB.

### Prerequisites

1. Download and install [Arduino IDE](https://www.arduino.cc/en/software).
2. In Arduino IDE, open **File > Preferences** and add the following URL to the "Additional boards manager URLs" field:
   `https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json`
3. Open **Tools > Board > Boards Manager**, search for `STM32`, and install the **STM32 MCU based boards** package by STMicroelectronics.
4. Set your board: **Tools > Board > STM32 MCU based boards > Generic STM32G4 series**.
5. Set the upload method: **Tools > Upload Method > STM32CubeProgrammer (DFU)**.

### Uploading a Sketch

1. Write or open your sketch in Arduino IDE.
2. Put the board into bootloader mode: hold the **BOOT** button, then press and release the **RESET** button, then release **BOOT**. The yellow and green LEDs will glow dim when the board is in bootloader mode.
3. Click **Upload** in Arduino IDE. The sketch will be compiled and flashed over USB.
4. After upload completes, press **RESET** once to run your new code.

---

## Section 2: Flashing a Brand New Board (Factory Fresh)

A factory-fresh board does not have the Arduino bootloader installed. Follow these steps to flash the IEEEPowerKit firmware using STM32CubeProgrammer. You only need to do this once — after this, the board can be programmed over USB from Arduino IDE without STM32CubeProgrammer. Power Kits come with the bootloader installed already! These steps are for internal use only.

1. Click this [link](https://github.com/AlfredoSystems/IEEEPowerKit/archive/refs/heads/main.zip) to download the latest IEEEPowerKit.bin file, software for the IEEE Power Electronics Kit Board.
1. Click this [link](https://drive.google.com/file/d/1hDfX-p4nzeYVYNdIkguA9ulwUVvr4Olv/view?usp=drive_link) to download STM32CubeProgrammer.
1. Install and run STM32CubeProgrammer.
1. There is a drop down in the top right with four options: ST-LINK, j-link, UART, USB. Select the USB option.
1. The provided USB cable in the kit has two sides, a USB-A and a USB-C side. Go ahead and plug the USB-C side into the Power Electronics Board.
1. IMPORTANT: Now you will put the board in "boot mode". To do that, press and hold the boot button while the device has no power. THEN plug the USB-A side into your computer. You can now release the boot button. You will know if it worked if the yellow and green leds are on but dim.
1. Back in STM32CubeProgrammer, on the right you will see a panel called USB Configuration, hit the port refresh button. STM32CubeProgrammer will find your board at USB1.
1. Click the CONNECT button in the top right.
1. Once the device is connected, on the left go to the Erasing and Programming Tab.
1. Click the Browse button and select the IEEEPowerKit.bin file you just downloaded.
1. Click Start Programming. After just a second the file will be uploaded to the Power Electronics Board and it will be ready for use.
