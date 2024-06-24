#include <ClickEncoder.h>
#include <TimerOne.h>
#include <HID-Project.h>

// Pins for the first encoder
#define ENCODER_CLK1 A0 
#define ENCODER_DT1 A1
#define ENCODER_SW1 A2

// Pins for the second encoder
#define ENCODER_CLK2 16 
#define ENCODER_DT2 14
#define ENCODER_SW2 15

//NOTE: both encoders must have VCC connected (they can be connected to the same vcc, up to 400mA of power draw, should be plenty for 2 encorders)

//_________________________________________________VARIABLES
ClickEncoder *encoder1; // encoder1 variable
ClickEncoder *encoder2; // encoder2 variable
int16_t last1, last2, value1, value2; // variables for current and last rotation value for both encoders

//LAYER VARIABLES - to activate a layer, double-click one of the encoders. If both left and right layers are active, layer_both is active, also known as layer 3
bool layer_default = true; //layer 0
bool layer_sx = false;     //layer 1
bool layer_dx = false;     //layer 2
bool layer_both = false;   //layer 3
//_________________________________________________VARIABLES END

void timerIsr() {
  encoder1->service();
  encoder2->service();
}

void checkLayer3(){
    if (layer_sx && layer_dx)
      layer_both = true;
    else
      layer_both = false;
}



void setup() {
  Serial.begin(9600); // Opens the serial connection used for communication with the PC. 
  Consumer.begin(); // Initializes the media keyboard
  encoder1 = new ClickEncoder(ENCODER_DT2, ENCODER_CLK2, ENCODER_SW2, 4); // Initializes the first rotary encoder with the mentioned pins. note that the last number is the number of steps per notch
  encoder2 = new ClickEncoder(ENCODER_DT1, ENCODER_CLK1, ENCODER_SW1, 4); // Initializes the second rotary encoder with the mentioned pins. note that the last number is the number of steps per notch

  Timer1.initialize(1000); // Initializes the timer, which the rotary encoders use to detect rotation
  Timer1.attachInterrupt(timerIsr); 
  last1 = last2 = -1;

  //led definition
  pinMode(8, OUTPUT);
  pinMode(4, OUTPUT);

}

//_________________________________________________LOOP
void loop() {  
  value1 += encoder1->getValue();
  value2 += encoder2->getValue();

  //led control
  if (layer_dx)
    digitalWrite(8, HIGH); 
  else 
    digitalWrite(8, LOW);

if (layer_sx)
    digitalWrite(4, HIGH); 
  else 
    digitalWrite(4, LOW);

  checkLayer3(); // Check necessary for layer 3 (layer_both) activation

                                                                                                          //_________________________LAYER_SELECTOR___________________________
  if (layer_both){                                                                                        //_________________________LAYER_BOTH_____LAYER_3___________________
    Serial.println("layer_both");
  //ENCODER1 ROTATION
    if (value1 != last1) {
      if(last1 < value1){
        Keyboard.press(KEY_W); //CHANGE ME: CLOCKWISE ENCODER1 KEYBIND
        delay(100); //this long delay is just so that the encoder doesn't have to be turned as much to move
        Keyboard.releaseAll();
      }
      else{
        Keyboard.press(KEY_S); //CHANGE ME: COUNTER-CLOCKWISE ENCODER1 KEYBIND
        delay(100); //this long delay is just so that the encoder doesn't have to be turned as much to move
        Keyboard.releaseAll();
      }

      last1 = value1;
      Serial.print("Encoder 1 Value: ");
      Serial.println(value1);
    }
    //ENCODER1 BUTTON
    ClickEncoder::Button b1 = encoder1->getButton();
    if (b1 != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
      switch (b1) {
        case ClickEncoder::Clicked:{
          Keyboard.press(KEY_Q); //CHANGE ME: ENCODER1 BUTTON CLICK KEYBIND
          Keyboard.releaseAll();
        }

        break;      
        
        case ClickEncoder::DoubleClicked:
          layer_sx = !layer_sx; //LAYER TOGGLE
        break;      
      }
    }

    //ENCODER2 ROTATION
    if (value2 != last2) {
      if(last2 < value2){
        Keyboard.press(KEY_D); //CHANGE ME: CLOCKWISE ENCODER2 KEYBIND
        delay(100); //this long delay is just so that the encoder doesn't have to be turned as much to move
        Keyboard.releaseAll();
        }
      else{
        Keyboard.press(KEY_A); //CHANGE ME: COUNTER-CLOCKWISE ENCODER2 KEYBIND
        delay(100); //this long delay is just so that the encoder doesn't have to be turned as much to move
        Keyboard.releaseAll();
      }
      last2 = value2;
      Serial.print("Encoder 2 Value: ");
      Serial.println(value2);
    }
    //ENCODER2 BUTTON
    ClickEncoder::Button b2 = encoder2->getButton();
    if (b2 != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
      switch (b2) {
        case ClickEncoder::Clicked:{
          Keyboard.press(KEY_Q); //CHANGE ME: ENCODER2 BUTTON CLICK KEYBIND
          Keyboard.releaseAll();
          break;
        }
        case ClickEncoder::DoubleClicked:
          layer_dx = !layer_dx; //LAYER TOGGLE
        break;      
      }
    }
    delay(10);
  } else if (layer_sx) {                                                                                  //_________________________LAYER_SX_____LAYER_1_____________________
    Serial.println("layer_sx");
  //ENCODER1 ROTATION
    if (value1 != last1) {
      if(last1 < value1){
        Consumer.write(MEDIA_VOLUME_UP); //CHANGE ME: CLOCKWISE ENCODER1 KEYBIND
      }
      else{
        Consumer.write(MEDIA_VOLUME_DOWN); //CHANGE ME: COUNTER-CLOCKWISE ENCODER1 KEYBIND
      }

      last1 = value1;
      Serial.print("Encoder 1 Value: ");
      Serial.println(value1);
    }
    //ENCODER1 BUTTON
    ClickEncoder::Button b1 = encoder1->getButton();
    if (b1 != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
      switch (b1) {
        case ClickEncoder::Clicked:{
          Consumer.write(MEDIA_PLAY_PAUSE); //CHANGE ME: ENCODER1 BUTTON CLICK KEYBIND
        }

        break;      
        
        case ClickEncoder::DoubleClicked:
          layer_sx = !layer_sx; //LAYER TOGGLE
        break;      
      }
    }

    //ENCODER2 ROTATION
    if (value2 != last2) {
      if(last2 < value2){
        Keyboard.press(KEY_LEFT_CTRL); //key combo! 
        Keyboard.press(KEYPAD_ADD);//CHANGE ME: CLOCKWISE ENCODER2 KEYBIND
        delay(10);
        Keyboard.releaseAll();
      }
      else{
        Keyboard.press(KEY_LEFT_CTRL); //key combo! 
        Keyboard.press(KEYPAD_SUBTRACT);//CHANGE ME: COUNTER-CLOCKWISE ENCODER2 KEYBIND
        delay(10);
        Keyboard.releaseAll();
      }
      last2 = value2;
      Serial.print("Encoder 2 Value: ");
      Serial.println(value2);
    }
    //ENCODER2 BUTTON
    ClickEncoder::Button b2 = encoder2->getButton();
    if (b2 != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
      switch (b2) {
        case ClickEncoder::Clicked:{
          Keyboard.press(KEY_LEFT_CTRL); //key combo! 
          Keyboard.press(KEYPAD_0);//CHANGE ME: COUNTER-CLOCKWISE ENCODER2 KEYBIND
          delay(10);
          Keyboard.releaseAll();
          break; 
        }
        case ClickEncoder::DoubleClicked:
          layer_dx = !layer_dx; //LAYER TOGGLE
        break;      
      }
    }
    delay(10);
  } else if (layer_dx) {                                                                                  //_________________________LAYER_DX_____LAYER_2_____________________
    Serial.println("layer_dx");
  //ENCODER1 ROTATION
    if (value1 != last1) {
      if(last1 < value1){
        Keyboard.press(KEY_RIGHT_ARROW); //CHANGE ME: CLOCKWISE ENCODER1 KEYBIND
        Keyboard.releaseAll();
      }
      else{
        Keyboard.press(KEY_LEFT_ARROW); //CHANGE ME: COUNTER-CLOCKWISE ENCODER1 KEYBIND
        Keyboard.releaseAll();
      }

      last1 = value1;
      Serial.print("Encoder 1 Value: ");
      Serial.println(value1);
    }
    //ENCODER1 BUTTON
    ClickEncoder::Button b1 = encoder1->getButton();
    if (b1 != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
      switch (b1) {
        case ClickEncoder::Clicked:{
          Keyboard.press(KEY_HOME); //CHANGE ME: ENCODER1 BUTTON CLICK KEYBIND
          Keyboard.releaseAll();
        }

        break;      
        
        case ClickEncoder::DoubleClicked:
          layer_sx = !layer_sx; //LAYER TOGGLE
        break;      
      }
    }

    //ENCODER2 ROTATION
    if (value2 != last2) {
      if(last2 < value2){
        Keyboard.press(KEY_DOWN_ARROW); //CHANGE ME: CLOCKWISE ENCODER1 KEYBIND
        Keyboard.releaseAll();
      }
      else {
        Keyboard.press(KEY_UP_ARROW); //CHANGE ME: COUNTER-CLOCKWISE ENCODER2 KEYBIND
        Keyboard.releaseAll();
      }
      last2 = value2;
      Serial.print("Encoder 2 Value: ");
      Serial.println(value2);
    }
    //ENCODER2 BUTTON
    ClickEncoder::Button b2 = encoder2->getButton();
    if (b2 != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
      switch (b2) {
        case ClickEncoder::Clicked:{
        Keyboard.press(KEY_SPACE); //CHANGE ME: ENCODER2 BUTTON CLICK KEYBIND
        Keyboard.releaseAll();
        break;      
        }

        
        case ClickEncoder::DoubleClicked:
          layer_dx = !layer_dx; //LAYER TOGGLE
        break;      
      }
    }
    delay(10);
  } else if (layer_default) {                                                                             //_________________________LAYER_DEFAULT_____LAYER_0________________
    Serial.println("layer_default");
  //ENCODER1 ROTATION
    if (value1 != last1) {
      if(last1 < value1){
        Mouse.move(0, 0, -1); //CHANGE ME: CLOCKWISE ENCODER1 KEYBIND
        Keyboard.releaseAll();
      }
      else{
        Mouse.move(0, 0, 1); //CHANGE ME: COUNTER-CLOCKWISE ENCODER1 KEYBIND
        Keyboard.releaseAll();
      }

      last1 = value1;
      Serial.print("Encoder 1 Value: ");
      Serial.println(value1);
    }
    //ENCODER1 BUTTON
    ClickEncoder::Button b1 = encoder1->getButton();
    if (b1 != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
      switch (b1) {
        case ClickEncoder::Clicked:{
          Keyboard.press(KEY_F5); //CHANGE ME: ENCODER1 BUTTON CLICK KEYBIND
          Keyboard.releaseAll();
        }

        break;      
        
        case ClickEncoder::DoubleClicked:
          layer_sx = !layer_sx; //LAYER TOGGLE
        break;      
      }
    }

    //ENCODER2 ROTATION
    if (value2 != last2) {
      if(last2 < value2){
        Keyboard.press(KEY_TAB); //CHANGE ME: CLOCKWISE ENCODER2 
        Keyboard.releaseAll();
      }
      else{
        Keyboard.press(KEY_LEFT_SHIFT); //key combo!
        Keyboard.press(KEY_TAB); //CHANGE ME: COUNTER-CLOCKWISE ENCODER2 KEYBIND
        delay(10);
        Keyboard.releaseAll();
      }
      last2 = value2;
      Serial.print("Encoder 2 Value: ");
      Serial.println(value2);
    }
    //ENCODER2 BUTTON
    ClickEncoder::Button b2 = encoder2->getButton();
    if (b2 != ClickEncoder::Open) { // If the button is unpressed, we'll skip to the end of this if block
      switch (b2) {
        case ClickEncoder::Clicked:
          Keyboard.press(KEY_ENTER); //CHANGE ME: ENCODER2 BUTTON CLICK KEYBIND
          Keyboard.releaseAll();
        break;      
        
        case ClickEncoder::DoubleClicked:
          layer_dx = !layer_dx; //LAYER TOGGLE
        break;      
      }
    }
    delay(10);
  }
                                                                                                          //_________________________LAYER_SELECTOR_END_______________________



}
//__________________________________________________LOOP END
