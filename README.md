# bootdev_pp1_encoder_menu
A menu for a small OLED screen that can be navigated with a `rotary encoder` and `confirm` and `back actions`.

### Language
Arduino leaning towards C.

### Hardware
- ESP32-S3 Feather [Product ID: 5477](https://www.adafruit.com/product/5477)
- Rotary encoder (I2C) [Product ID: 4991](https://www.adafruit.com/product/4991)
- 1.3" 128x64 OLED display (SSD1306, I2C) [Product ID: 938](https://www.adafruit.com/product/938)

### Usage
This repo is an example of how the code can be used. Main.cpp sets up a few screens. The enum indexing these screens is in MenuItem.h. Each screen displays a title and an array of menu items which can have a mixture of the following types:
- MENU_ITEM_TYPE_ACTION -> calls a function without return and parameters like void func().
- MENU_ITEM_TYPE_NAVIGATE -> links to another screen.
- MENU_ITEM_TYPE_VALUE -> displays and modifies a value of one of the following types
    - VALUE_TYPE_INT
    - VALUE_TYPE_TEMP
    - VALUE_TYPE_TIME
    - VALUE_TYPE_TEXT

Rendering happens on demand, when in the menu. A _dirty flag is set when anything has changed and the screen needs to be redrawn.

There is also a home screen that is not part of the menu structure and can be used to display live updating information. In my example, turning the encoder enters the menu. This leaves the click of the encoder to do a common action, like start/stop a process. One could also use the direction of the turn to enter two different menu structures (e.g. ccw -> detailed process setup, cw -> select from previously set up processes).

Short press on the encoder acts as confirm, long press as back. Long press triggers upon reaching the long press duration and does not wait for release.

### Gallery
<img src="https://github.com/cyberfly100/bootdev_pp1_encoder_menu/blob/main/.github/mainmenu.jpg?raw=true" alt="Main Menu" width="200"/>
<img src="https://github.com/cyberfly100/bootdev_pp1_encoder_menu/blob/main/.github/navigateitems.jpg?raw=true" alt="Navigate items" width="200"/>
<img src="https://github.com/cyberfly100/bootdev_pp1_encoder_menu/blob/main/.github/menuitems.jpg?raw=true" alt="Menu items" width="200"/>
<img src="ttps://github.com/cyberfly100/bootdev_pp1_encoder_menu/blob/main/.github/edittext.jpg?raw=true" alt="Edit text" width="200"/>

### Notes
I used PlatformIO within VS Code to flash the board.

Things to improve in the future:
- Time highlights do not fit perfectly and should probably adopt the text approch of dynamic string length checking.
- MenuNavigator and Display are tightly coupled via MenuNavigators getters. 
- add a generator that can convert an md file describing the menu structure into the screen array and menu items