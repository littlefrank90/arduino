# TIAPAD LCD
## Introduction
TIAPAD LCD is a completely open source macropad featuring a 16x2 LCD, a rotary encoder knob for controls and menu navigation and 4 buttons.

[Printables link](https://github.com/littlefrank90/arduino/tree/main/tiapad_LCD#tiapad-lcd)

![photo1](https://raw.githubusercontent.com/littlefrank90/arduino/refs/heads/main/tiapad_LCD/photo1.jpg)

## What you need
The part list is rather short and self-explanatory (source: AliExpress):
* [4 mechanical switches](https://it.aliexpress.com/item/1005002378701948.html?spm=a2g0o.order_list.order_list_main.27.112e3696PeCfgK&gatewayAdapt=glo2ita)
* [Arduino pro micro/Leonardo](https://it.aliexpress.com/item/32846843498.html?spm=a2g0o.order_list.order_list_main.134.112e3696PeCfgK&gatewayAdapt=glo2ita)
* [LCD 1602](https://it.aliexpress.com/item/1005006964073869.html?spm=a2g0o.order_list.order_list_main.21.112e3696PeCfgK&gatewayAdapt=glo2ita)
* [USB C cable](https://it.aliexpress.com/item/1005007322156430.html?spm=a2g0o.order_list.order_list_main.10.112e3696PeCfgK&gatewayAdapt=glo2ita)
* [Rotary encoder](https://it.aliexpress.com/item/1005005559054521.html?spm=a2g0o.order_list.order_list_main.279.45863696TzvKPF&gatewayAdapt=glo2ita)
* [M3 L3 OD 4.5 brass inserts](https://it.aliexpress.com/item/1005006071488810.html?spm=a2g0o.productlist.main.1.354939c2ZqYKdq&algo_pvid=17390cb9-c79d-4282-9f6d-2deae5a9d825&algo_exp_id=17390cb9-c79d-4282-9f6d-2deae5a9d825-0&pdp_ext_f=%7B%22order%22%3A%224967%22%2C%22eval%22%3A%221%22%7D&pdp_npi=4%40dis%21EUR%212.95%212.54%21%21%2121.79%2118.74%21%40%2112000035595774900%21sea%21IT%214619657392%21X&curPageLogUid=kkTOv7SxkufB&utparam-url=scene%3Asearch%7Cquery_from%3A)

## Printing instructions
DOWNLOAD STL FILES HERE:
* [Main shell](tiapad_LCD.stl) - Print this laying down on its bottom and use supports for best results.
* [Lid](tiapad_LCD_lid.stl)
* [Rotary encoder knob](rotary_encoder_knob.stl) - Print on its head, with the hole on the top.
* [Keycaps - F1-F4 keys](keycaps_tiapad_lcd_f1_f4.stl) - print 2
* [Keycaps - F2-F3 keys](keycaps_tiapad_lcd_f2_f3.stl) - print 2

## Assembly instructions
- Start by soldering what will be the positives of your switches to a piece of wire, possibly with a dupont connector on the opposite side (the one that goes to the arduino). Do the same for all the GND.
It will not be easy to solder wires on the switches' pins once when they are inside the main shell, so it's better to do it in advance.
- Now pass the wires through the 4 holes on the top of the shells and click the switches in place.
- Use the soldering iron to push the inserts in the 4 slots behind the LCD window in the main shell.
- Put dupont (female-to-female) connectors on the rotary encoder knobs and place the knob in the hole to the left of the LCD, use the flat nut to secure it in place, connectors should be facing the top of the shell.
- Do the same for the LCD connectors and secure the LCD in place with 4 regular PC screws (M3x5 you would use on motherboards). It will be a tight fit, it should slide right below the rotary encoder PCB.
- Connect all the GND wires together and connect them to the arduino GND
- Do the same with the 2 VCC wires
- For the rest of the connections follow the schematics below
- Once you are done putting everything together you can use a hot glue gun and put a generous drop near the USB-c slot, there's a rectangular notch that should help align the arduino pcb.
- later, when you're sure you're done with the wiring, you can close the lid just by pushing it in place.
- push the 3d printed keycaps on the switches
- behind the LCD there should be a contrast screw. Remember to turn this and adjust your contrast once you uploaded the code to the arduino.

It should look something like this:
![photo3](https://raw.githubusercontent.com/littlefrank90/arduino/refs/heads/main/tiapad_LCD/photo3.jpg)

## Schematics
![schematics](https://github.com/littlefrank90/arduino/blob/main/tiapad_LCD/schematics.png?raw=true)

## Code
[DOWNLOAD HERE](tiapad_LCD.ino)

The arduino will need to be flashed using Arduino IDE (the board should appear as an Arduino Leonardo).

You will need to import the two libraries:
* [LiquidCrystal_I2C](https://github.com/johnrickman/LiquidCrystal_I2C)
* [HID-Project](https://github.com/NicoHood/HID) --> for media keyboard functionality, aka Consumer

To do so, from Arduino IDE simply do Tools --> Manage Libraries --> Search the two names above --> install.
At this point you're ready to import the code by downloading the [tiapad_LCD.ino](tiapad_LCD.ino) file.
Compile it and send it to the arduino.

### Customizing keybinds
You can customize the keybinds by changing the big "switch case" inside the performActions function definition.
For **example**, the following extract from tiapad_LCD.ino, lines 368-379, will define the behaviour of BUTTON F1 (aka BUTTON1) when in Discord mode.
I have set it so that it matches the key combination I use to mute the microphone: RIGHT CTRL + INS.

```C++
      //F1
      if (button1State == LOW && lastButton1State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Toggle mic mute");
          showPopupMessage("Discord mic mute");
          Keyboard.press(KEY_RIGHT_CTRL);
          Keyboard.press(KEY_INSERT);
          Keyboard.releaseAll();
          userActivityDetected();
        }
      }
      lastButton1State = button1State;
```
**Note: Button F4 in Discord mode and all keybinds in Games mode have been purposely left unassigned to leave some space to customizations.**
