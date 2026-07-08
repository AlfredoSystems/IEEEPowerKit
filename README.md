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

A factory-fresh board does not have the Arduino bootloader installed. Follow these steps to flash the IEEEPowerKit firmware using PlatformIO and dfu-util. You only need to do this once — after this, the board can be programmed over USB from Arduino IDE without these tools. Power Kits come with the bootloader installed already! These steps are for internal use only.

### Prerequisites (one-time setup)

1. Install [Python](https://www.python.org/downloads/) if you don't have it.
2. Install PlatformIO and the STM32 dfu-util tool:
   ```
   pip install platformio
   pio platform install ststm32
   pio pkg install --global --tool "tool-dfuutil"
   ```
3. Install the WinUSB driver for the STM32 bootloader using [Zadig](https://zadig.akeo.ie/):
   - Download and run Zadig.
   - Go to **Options → List All Devices**.
   - Select **STM32 BOOTLOADER** from the dropdown (VID `0483`, PID `DF11`).
   - Set the driver on the right to **WinUSB** and click **Replace Driver**.

### Flashing

1. Click this [link](https://github.com/AlfredoSystems/IEEEPowerKit/archive/refs/heads/main.zip) to download the latest `IEEEPowerKit2.bin`.
2. Put the board into DFU boot mode: unplug USB, hold the **BOOT** button, plug USB into your computer, then release **BOOT**. The yellow and green LEDs will glow dim when the board is in boot mode.
3. Open a terminal in the folder containing `IEEEPowerKit2.bin` and run:
   ```
   pio pkg exec --package "tool-dfuutil" -- dfu-util -d 0483:df11 -a 0 -s 0x08000000:leave -D "IEEEPowerKit2.bin"
   ```
4. The output will show `Download done. File downloaded successfully` followed by a reset. The board is now flashed and ready to use.
