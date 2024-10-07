# IEEEPowerKit

1) Click this [link](https://github.com/AlfredoSystems/IEEEPowerKit/archive/refs/heads/main.zip) to download the latest IEEEPowerKit.bin file, software for the IEEE Power Electronics Kit Board.
1) Click this [link](https://drive.google.com/file/d/1hDfX-p4nzeYVYNdIkguA9ulwUVvr4Olv/view?usp=drive_link) to download STM32CubeProgrammer.
1) install and run STM32CubeProgrammer.
1) There is a drop down in the top right with four options: ST-LINK, j-link, UART, USB. Select the USB option.
1) The provided USB cable in the kit has two sides, a USB-A and a USB-C side. Go ahead and plug the USB-C side into the Power Electronics Board.
1) IMPORTANT: Now you will put the board in “boot mode”. To do that, press and hold the boot button while the device has no power. THEN plug the USB-A side into your computer. You can now release the boot button. You will know if it worked if the yellow and green leds are on but dim.
1) Back in STM32CubeProgrammer, on the right you will see a panel called USB Configuration, hit the port refresh button. STM32CubeProgrammer will find your board at USB1.
1) Click the CONNECT button in the top right.
1) Once the device is connected, on the left go to the Erasing and Programming Tab.
1) Click the Browse button and select the IEEEPowerKit.bin file you just downloaded.
1) Click Start Programming. After just a second the file will be uploaded to the Power Electronics Board and it will be ready for use.
