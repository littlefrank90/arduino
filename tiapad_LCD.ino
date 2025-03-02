#include <LiquidCrystal_I2C.h>
#include <HID-Project.h>  // For keyboard and consumer controls

// Rotary encoder pin definitions
#define PIN_CLK 4  // Rotary encoder CLK pin
#define PIN_DT 5   // Rotary encoder DT pin
#define PIN_SW 6   // Rotary encoder button pin

// Button pins
#define BUTTON1 10  // First button pin
#define BUTTON2 14  // Second button pin
#define BUTTON3 15  // Third button pin
#define BUTTON4 16  // Fourth button pin

// LCD setup (0x27 is the typical I2C address, adjust if needed)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Set the LCD address to 0x27 for a 16 chars and 2 line display


bool inSubmenu = false;
volatile int lastStateCLK;
enum MenuItem { MEDIA,
                DISCORD,
                WEB,
                GAMES };
MenuItem currentMenu = MEDIA;
const unsigned long BACKLIGHT_TIMEOUT = 3000;  //backlight timer in ms
unsigned long lastBacklightOn = 0;
const unsigned long MESSAGE_DURATION = 2000;  //popup message timer in ms
unsigned long messageStartTime = 0;
bool popupActive = false;

//________________________________SETUP

void setup() {
  Serial.begin(9600);

  pinMode(PIN_CLK, INPUT);
  pinMode(PIN_DT, INPUT);
  pinMode(PIN_SW, INPUT_PULLUP);

  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  pinMode(BUTTON4, INPUT_PULLUP);

  lcd.init();

  Keyboard.begin();
  Consumer.begin();

  displayMenu();
  selectMenu(currentMenu);

  lastStateCLK = digitalRead(PIN_CLK);
}

//________________________________FUNCTIONS

void userActivityDetected() {
  lastBacklightOn = millis();
}

void backlightHandler() {
  if ((millis() - lastBacklightOn) >= BACKLIGHT_TIMEOUT) {
    lcd.noBacklight();
  } else {
    lcd.backlight();
  }
}

void showPopupMessage(String message) {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(message);
  Serial.println("Writing to LCD: " + message);
  messageStartTime = millis();
  popupActive = true;
}

void clearPopupMessage() {
  if ((millis() - messageStartTime) >= MESSAGE_DURATION) {
    lcd.setCursor(0, 1);
    lcd.print("                ");
    Serial.println("Clearing popup message.");
    popupActive = false;
  }
}

void displayMenu() {
  lcd.clear();
  // Menu 1
  lcd.setCursor(1, 0);
  lcd.print("Media");
  // Menu 2
  lcd.setCursor(9, 0);
  lcd.print("Discord");
  // Menu 3
  lcd.setCursor(1, 1);
  lcd.print("Web");
  // Menu 4
  lcd.setCursor(9, 1);
  lcd.print("Games");
}

void selectMenu(MenuItem currentMenu) {
  if (inSubmenu) { return; }
  // Menu positions on LCD
  const uint8_t menuPositions[4][2] = {
    { 0, 0 },  // Menu 1
    { 8, 0 },  // Menu 2
    { 0, 1 },  // Menu 3
    { 8, 1 }   // Menu 4
  };
  for (uint8_t i = 0; i < 4; i++) {
    lcd.setCursor(menuPositions[i][0], menuPositions[i][1]);
    if (i == currentMenu) {
      lcd.print(">");
    } else {
      lcd.print(" ");
    }
  }
}

void updateMenu() {
  int currentStateCLK = digitalRead(PIN_CLK);
  // Check if the rotary encoder has rotated
  if (currentStateCLK != lastStateCLK && currentStateCLK == LOW) {
    userActivityDetected();
    if (digitalRead(PIN_DT) != currentStateCLK) {
      // Clockwise rotation
      currentMenu = (MenuItem)((currentMenu + 1) % 4);
    } else {
      // Counterclockwise rotation
      currentMenu = (MenuItem)((currentMenu - 1 + 4) % 4);
    }

    // Update menu display
    selectMenu(currentMenu);
  }

  lastStateCLK = currentStateCLK;
}

void knobClickHandler() {
  static bool lastButtonState = HIGH;  // Last known state of button
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 1000;  // Debounce time in ms
  bool buttonState = digitalRead(PIN_SW);    // Read current button state

  if (buttonState == LOW && lastButtonState == HIGH) {
    if (millis() - lastDebounceTime > debounceDelay) {
      userActivityDetected();
      if (inSubmenu) {
        Serial.println("Exiting submenu...");
        inSubmenu = false;
        displayMenu();
        selectMenu(currentMenu);
        updateMenu();
      } else {
        Serial.print("Button clicked in menu: ");
        navigateSubmenu(currentMenu);
      }
      lastDebounceTime = millis();  // Update debounce timer
    }
  }
  lastButtonState = buttonState;
}

void buttonClickHandler() {
  if (inSubmenu) { return; }
  static bool lastButton1State = HIGH;
  static bool lastButton2State = HIGH;
  static bool lastButton3State = HIGH;
  static bool lastButton4State = HIGH;

  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 1000;  // Debounce time in ms

  bool button1State = digitalRead(BUTTON1);
  bool button2State = digitalRead(BUTTON2);
  bool button3State = digitalRead(BUTTON3);
  bool button4State = digitalRead(BUTTON4);

  if (button1State == LOW && lastButton1State == HIGH) {
    if (millis() - lastDebounceTime > debounceDelay) {
      userActivityDetected();
      lastDebounceTime = millis();  // Update debounce timer
      currentMenu = MEDIA;
      Serial.println("BUTTON1 pressed - MEDIA SELECTED");
      selectMenu(currentMenu);
      delay(200);
      navigateSubmenu(currentMenu);
    }
  }
  if (button2State == LOW && lastButton2State == HIGH) {
    if (millis() - lastDebounceTime > debounceDelay) {
      userActivityDetected();
      lastDebounceTime = millis();  // Update debounce timer
      currentMenu = DISCORD;
      Serial.println("BUTTON2 pressed - DISCORD SELECTED");
      selectMenu(currentMenu);
      delay(200);
      navigateSubmenu(currentMenu);
    }
  }
  if (button3State == LOW && lastButton3State == HIGH) {
    if (millis() - lastDebounceTime > debounceDelay) {
      userActivityDetected();
      lastDebounceTime = millis();  // Update debounce timer
      currentMenu = WEB;
      Serial.println("BUTTON3 pressed - WEB SELECTED");
      selectMenu(currentMenu);
      delay(200);
      navigateSubmenu(currentMenu);
    }
  }
  if (button4State == LOW && lastButton4State == HIGH) {
    if (millis() - lastDebounceTime > debounceDelay) {
      userActivityDetected();
      lastDebounceTime = millis();  // Update debounce timer
      currentMenu = GAMES;
      Serial.println("BUTTON4 pressed - GAMES SELECTED");
      selectMenu(currentMenu);
      delay(200);
      navigateSubmenu(currentMenu);
    }
  }
  lastButton1State = button1State;
  lastButton2State = button2State;
  lastButton3State = button3State;
  lastButton4State = button4State;
}

void navigateSubmenu(MenuItem currentMenu) {
  lcd.clear();
  inSubmenu = true;

  switch (currentMenu) {
    case MEDIA:
      Serial.println("MEDIA");
      lcd.setCursor(0, 0);
      lcd.print("[Media]");
      performActions(currentMenu);
      break;
    case DISCORD:
      Serial.println("DISCORD");
      lcd.setCursor(0, 0);
      lcd.print("[Discord]");
      performActions(currentMenu);
      break;
    case WEB:
      Serial.println("WEB");
      lcd.setCursor(0, 0);
      lcd.print("[Web]");
      performActions(currentMenu);
      break;
    case GAMES:
      Serial.println("GAMES");
      lcd.setCursor(0, 0);
      lcd.print("[Games]");
      performActions(currentMenu);
      break;
  }
}

void performActions(MenuItem currentMenu) {
  int currentStateCLK = digitalRead(PIN_CLK);
  static bool lastButton1State = HIGH;
  static bool lastButton2State = HIGH;
  static bool lastButton3State = HIGH;
  static bool lastButton4State = HIGH;
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 1000;  // Debounce time in ms
  bool button1State = digitalRead(BUTTON1);
  bool button2State = digitalRead(BUTTON2);
  bool button3State = digitalRead(BUTTON3);
  bool button4State = digitalRead(BUTTON4);


  //TO CHANGE KEYBINDINGS, EDIT THE BUTTON ASSIGNMENTS IN THE FOLLOWING SWITCH STATEMENT.
  switch (currentMenu) {
    case MEDIA:
      // === ROTARY ENCODER (unchanged) ===
      if (currentStateCLK != lastStateCLK && currentStateCLK == LOW) {
        if (digitalRead(PIN_DT) != currentStateCLK) {
          // Clockwise rotation
          Serial.println("vol +");
          showPopupMessage("vol +");
          Consumer.write(MEDIA_VOLUME_UP);
          userActivityDetected();
        } else {
          // Counterclockwise rotation
          Serial.println("vol -");
          showPopupMessage("vol -");
          Consumer.write(MEDIA_VOLUME_DOWN);
          userActivityDetected();
        }
      }
      lastStateCLK = currentStateCLK;

      // === BUTTON HANDLING ===
      //F1
      if (button1State == LOW && lastButton1State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Play/Pause");
          showPopupMessage("Play/Pause");
          Consumer.write(MEDIA_PLAY_PAUSE);
          userActivityDetected();
        }
      }
      lastButton1State = button1State;
      //F2
      if (button2State == LOW && lastButton2State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Previous track");
          showPopupMessage("Previous track");
          Consumer.write(MEDIA_PREV);
          userActivityDetected();
        }
      }
      lastButton2State = button2State;
      //F3
      if (button3State == LOW && lastButton3State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Next track");
          showPopupMessage("Next track");
          Consumer.write(MEDIA_NEXT);
          userActivityDetected();
        }
      }
      lastButton3State = button3State;
      //F4
      if (button4State == LOW && lastButton4State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Windows audio output");
          showPopupMessage("Windows audio output");
          Keyboard.press(KEY_LEFT_GUI);
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_V);
          delay(100);
          Keyboard.releaseAll();
          userActivityDetected();
        }
      }
      lastButton4State = button4State;
      break;

    case DISCORD:
      // === ROTARY ENCODER (unchanged) ===
      if (currentStateCLK != lastStateCLK && currentStateCLK == LOW) {
        if (digitalRead(PIN_DT) != currentStateCLK) {
          // Clockwise rotation
          Serial.println("vol +");
          showPopupMessage("vol +");
          Consumer.write(MEDIA_VOLUME_UP);
          userActivityDetected();
        } else {
          // Counterclockwise rotation
          Serial.println("vol -");
          showPopupMessage("vol -");
          Consumer.write(MEDIA_VOLUME_DOWN);
          userActivityDetected();
        }
      }
      lastStateCLK = currentStateCLK;

      // === BUTTON HANDLING ===

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
      //F2
      if (button2State == LOW && lastButton2State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Toggle deafen");
          showPopupMessage("Discord deafen");
          Keyboard.press(KEY_LEFT_ALT);
          Keyboard.press(KEY_INSERT);
          Keyboard.releaseAll();
          userActivityDetected();
        }
      }
      lastButton2State = button2State;
      //F3
      if (button3State == LOW && lastButton3State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Toggle streaming");
          showPopupMessage("Discord streaming");
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_INSERT);
          Keyboard.releaseAll();
          userActivityDetected();
        }
      }
      lastButton3State = button3State;
      //F4
      if (button4State == LOW && lastButton4State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Undefined button");
          showPopupMessage("Undefined button");
          // UNDEFINED!
          userActivityDetected();
        }
      }
      lastButton4State = button4State;
      break;

    case WEB:
      // === ROTARY ENCODER (unchanged) ===
      if (currentStateCLK != lastStateCLK && currentStateCLK == LOW) {
        if (digitalRead(PIN_DT) != currentStateCLK) {
          // Clockwise rotation
          Serial.println("scroll down");
          showPopupMessage("scroll down");
          Mouse.begin();
          Mouse.move(0, 0, -1);  //scroll down
          userActivityDetected();
        } else {
          // Counterclockwise rotation
          Serial.println("scroll up");
          showPopupMessage("scroll up");
          Mouse.begin();
          Mouse.move(0, 0, 1);  //scroll up
          userActivityDetected();
        }
      }
      lastStateCLK = currentStateCLK;

      // === BUTTON HANDLING ===
      //F1
      if (button1State == LOW && lastButton1State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("New TAB");
          showPopupMessage("New TAB");
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_T);
          Keyboard.releaseAll();
          userActivityDetected();
        }
      }
      lastButton1State = button1State;
      //F2
      if (button2State == LOW && lastButton2State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("<-- TAB");
          showPopupMessage("<-- TAB");
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_PAGE_UP);
          Keyboard.releaseAll();
          userActivityDetected();
        }
      }
      lastButton2State = button2State;
      //F3
      if (button3State == LOW && lastButton3State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("--> TAB");
          showPopupMessage("--> TAB");
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_PAGE_DOWN);
          Keyboard.releaseAll();
          userActivityDetected();
        }
      }
      lastButton3State = button3State;
      //F4
      if (button4State == LOW && lastButton4State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Close TAB");
          showPopupMessage("Close TAB");
          Keyboard.press(KEY_LEFT_CTRL);
          Keyboard.press(KEY_W);
          Keyboard.releaseAll();
          userActivityDetected();
        }
      }
      lastButton4State = button4State;
      break;

    case GAMES:
      // === ROTARY ENCODER (unchanged) ===
      if (currentStateCLK != lastStateCLK && currentStateCLK == LOW) {
        if (digitalRead(PIN_DT) != currentStateCLK) {
          // Clockwise rotation
          Serial.println("Undefined button");
          showPopupMessage("Undefined button");
          // UNDEFINED!
          userActivityDetected();
        } else {
          // Counterclockwise rotation
          Serial.println("Undefined button");
          showPopupMessage("Undefined button");
          // UNDEFINED!
          userActivityDetected();
        }
      }
      lastStateCLK = currentStateCLK;

      // === BUTTON HANDLING ===
      //F1
      if (button1State == LOW && lastButton1State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Undefined button");
          showPopupMessage("Undefined button");
          // UNDEFINED!
          userActivityDetected();
        }
      }
      lastButton1State = button1State;
      //F2
      if (button2State == LOW && lastButton2State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Undefined button");
          showPopupMessage("Undefined button");
          // UNDEFINED!
          userActivityDetected();
        }
      }
      lastButton2State = button2State;
      //F3
      if (button3State == LOW && lastButton3State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Undefined button");
          showPopupMessage("Undefined button");
          // UNDEFINED!
          userActivityDetected();
        }
      }
      lastButton3State = button3State;
      //F4
      if (button4State == LOW && lastButton4State == HIGH) {
        if (millis() - lastDebounceTime > debounceDelay) {
          Serial.println("Undefined button");
          showPopupMessage("Undefined button");
          // UNDEFINED!
          userActivityDetected();
        }
      }
      lastButton4State = button4State;
      break;
  }
}

//________________________________LOOP

void loop() {
  if (!inSubmenu) {
    updateMenu();
  } else {
    performActions(currentMenu);
  }
  knobClickHandler();
  buttonClickHandler();
  backlightHandler();
  if (popupActive && inSubmenu) { clearPopupMessage(); }
}
