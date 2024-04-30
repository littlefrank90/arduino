#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>

#define ENCODER_CLK1 A0 // Pins for the first encoder
#define ENCODER_DT1 A1
#define ENCODER_SW1 A2

#define ENCODER_CLK2 16 // Pins for the second encoder
#define ENCODER_DT2 14
#define ENCODER_SW2 15

//NOTE: both encoders must have VCC connected (they can be connected to the same vcc, up to 400mA of power draw, should be plenty for 2 encorders)

ClickEncoder *encoder1; // encoder1 variable
ClickEncoder *encoder2; // encoder2 variable
int16_t last1, last2, value1, value2; // variables for current and last rotation value for both encoders

void timerIsr() {
  encoder1->service();
  encoder2->service();
}

void setup() {
  Serial.begin(9600); // Opens the serial connection used for communication with the PC. 
  Consumer.begin(); // Initializes the media keyboard
  encoder1 = new ClickEncoder(ENCODER_DT1, ENCODER_CLK1, ENCODER_SW1); // Initializes the first rotary encoder with the mentioned pins
  encoder2 = new ClickEncoder(ENCODER_DT2, ENCODER_CLK2, ENCODER_SW2); // Initializes the second rotary encoder with the mentioned pins

  Timer1.initialize(1000); // Initializes the timer, which the rotary encoders use to detect rotation
  Timer1.attachInterrupt(timerIsr); 
  last1 = last2 = -1;
} 

void loop() {  
  value1 += encoder1->getValue();
  value2 += encoder2->getValue();

  // Handle the first encoder
  if (value1 != last1) {
    if(last1 < value1)
      Consumer.write(MEDIA_VOLUME_UP); //CHANGE ME: CLOCKWISE ENCODER1 KEYBIND
    else
      Consumer.write(MEDIA_VOLUME_DOWN); //CHANGE ME: COUNTER-CLOCKWISE ENCODER1 KEYBIND
    last1 = value1;
    Serial.print("Encoder 1 Value: ");
    Serial.println(value1);
  }

  // Handle the second encoder
  if (value2 != last2) {
    if(last2 < value2)
      Consumer.write(MEDIA_VOLUME_UP); //CHANGE ME: CLOCKWISE ENCODER1 KEYBIND
    else
      Consumer.write(MEDIA_VOLUME_DOWN); //CHANGE ME: COUNTER-CLOCKWISE ENCODER1 KEYBIND
    last2 = value2;
    Serial.print("Encoder 2 Value: ");
    Serial.println(value2);
  }

  // ENCODER1 CLICK
  ClickEncoder::Button b1 = encoder1->getButton(); // Asking the button for it's current state
  if (b1 != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
    switch (b1) {
      case ClickEncoder::Clicked: // Button was clicked once
        Consumer.write(MEDIA_PLAY_PAUSE); //CHANGE ME: ENCODER1 BUTTON CLICK KEYBIND
      break;      
      
      case ClickEncoder::DoubleClicked: // Button was double clicked
         Consumer.write(MEDIA_NEXT); //CHANGE ME: ENCODER1 BUTTON DOUBLE-CLICK KEYBIND
      break;      
    }
  }

  // ENCODER2 CLICK
  ClickEncoder::Button b2 = encoder2->getButton(); // Asking the button for it's current state
  if (b2 != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
    switch (b2) {
      case ClickEncoder::Clicked: // Button was clicked once
        Consumer.write(MEDIA_PLAY_PAUSE); //CHANGE ME: ENCODER2 BUTTON CLICK KEYBIND
      break;      
      
      case ClickEncoder::DoubleClicked: // Button was double clicked
         Consumer.write(MEDIA_NEXT); //CHANGE ME: ENCODER2 BUTTON DOUBLE-CLICK KEYBIND
      break;      
    }
  }

  delay(10);
}
