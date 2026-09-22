/*
 * ============================================================
 * MACROPAD - FIRMWARE FINALE
 * Arduino Pro Micro / Leonardo - ATmega32U4
 *
 * LIBRERIA RICHIESTA:
 *   HID-Project (di NicoHood)
 *   Installala da: Sketch -> Includi libreria -> Gestione librerie
 *   -> cerca "HID-Project" -> installa quella di NicoHood.
 *   Sostituisce Keyboard.h in modo compatibile e aggiunge il
 *   supporto ai tasti multimediali (Consumer Control).
 *
 * CABLAGGIO
 *
 * Rotary encoder su modulo:
 *   CLK -> D3
 *   DT  -> D2
 *   SW  -> RXI / D0
 *   +   -> VCC / 5V
 *   GND -> GND fisico
 *
 * Pulsanti:
 *   comune di tutti i pulsanti -> TX0 / D1
 *   pulsante 1 -> D6
 *   pulsante 2 -> D7
 *   pulsante 3 -> D8
 *   pulsante 4 -> D15
 *   pulsante 5 -> D16
 *
 * ATTENZIONE:
 * TX0/D1 viene usato come massa software per i pulsanti:
 * OUTPUT LOW. Non usarlo per la seriale UART mentre il macropad
 * e' collegato in questo modo.
 *
 * FUNZIONALITA' ATTUALI
 *   Pulsante 1 -> Previous track
 *   Pulsante 2 -> DISCORD MUTE
 *   Pulsante 3 -> DISCORD DEAFEN
 *   Pulsante 4 -> Next track
 *   Pulsante 5 -> DISCORD STREAMING
 *   Encoder orario     -> Volume su
 *   Encoder antiorario -> Volume giu'
 *   Click encoder      -> Play / Pausa
 *
 * Per cambiare le funzioni modifica esclusivamente la sezione
 * "CONFIGURAZIONE FUNZIONI" piu' sotto.
 * ============================================================
 */

#include <HID-Project.h>
#include <HID-Settings.h>

// ============================================================
// PIN - non modificare se il cablaggio resta quello indicato
// ============================================================

const uint8_t ENC_CLK = 3;
const uint8_t ENC_DT  = 2;
const uint8_t ENC_SW  = 0;  // RXI / D0

const uint8_t BUTTON_COMMON = 1;  // TX0 / D1
const uint8_t BUTTON_PINS[] = {6, 7, 8, 15, 16};
const uint8_t BUTTON_COUNT = sizeof(BUTTON_PINS) / sizeof(BUTTON_PINS[0]);

// ============================================================
// CONFIGURAZIONE FUNZIONI
// Modifica questa sezione per assegnare altri tasti.
//
// Ogni pulsante puo' essere configurato come:
//   - combinazione di tastiera (modificatore + tasto)
//   - tasto multimediale (Consumer Control)
//
// Per aggiungere/rimuovere pulsanti dalla combo di tastiera usa
// KeyboardKeycode (es. KEY_LEFT_CTRL, KEY_RIGHT_CTRL, KEY_LEFT_ALT,
// KEY_INSERT, KEY_ESC, ...).
//
// Per i tasti multimediali usa le costanti Consumer di HID-Project,
// es: MEDIA_VOLUME_UP, MEDIA_VOLUME_DOWN, MEDIA_PLAY_PAUSE,
// MEDIA_NEXT, MEDIA_PREVIOUS, MEDIA_STOP, MEDIA_MUTE.
// ============================================================

enum ButtonActionType {
  ACTION_KEY_COMBO,
  ACTION_MEDIA_KEY
};

// HID-Project non definisce un valore "nessun tasto": lo definiamo noi.
const KeyboardKeycode NO_MODIFIER = (KeyboardKeycode)0;

struct ButtonAction {
  ButtonActionType type;
  KeyboardKeycode modifier;   // usato solo se type == ACTION_KEY_COMBO (KEY_NONE se non serve)
  KeyboardKeycode key;        // usato solo se type == ACTION_KEY_COMBO
  ConsumerKeycode mediaKey;   // usato solo se type == ACTION_MEDIA_KEY
};

const ButtonAction BUTTON_ACTIONS[] = {
  { ACTION_MEDIA_KEY, NO_MODIFIER,    NO_MODIFIER, MEDIA_PREVIOUS },   // D6 - pulsante 1: Previous track
  { ACTION_KEY_COMBO, KEY_RIGHT_CTRL,   KEY_INSERT, MEDIA_VOLUME_UP },   // D7 - pulsante 2: DISCORD MUTE
  { ACTION_KEY_COMBO, KEY_LEFT_ALT,  KEY_INSERT, MEDIA_VOLUME_UP },   // D8 - pulsante 3: DISCORD DEAFEN
  { ACTION_MEDIA_KEY, NO_MODIFIER,    NO_MODIFIER, MEDIA_NEXT },       // D15 - pulsante 4: Next track
  { ACTION_KEY_COMBO, KEY_LEFT_CTRL,    NO_MODIFIER, MEDIA_PREVIOUS }    // D16 - pulsante 5: DISCORD STREAMING
};

// Encoder: rotazione -> volume, click -> play/pausa
const ConsumerKeycode ENCODER_CW_MEDIA_KEY  = MEDIA_VOLUME_UP;
const ConsumerKeycode ENCODER_CCW_MEDIA_KEY = MEDIA_VOLUME_DOWN;
const ConsumerKeycode ENCODER_CLICK_MEDIA_KEY = MEDIA_PLAY_PAUSE;

// Se il verso risulta invertito, imposta true.
const bool REVERSE_ENCODER = false;

// ============================================================
// PARAMETRI
// ============================================================

const unsigned long BUTTON_DEBOUNCE_MS = 25;
const unsigned long ENCODER_SW_DEBOUNCE_MS = 30;

// Il modulo encoder normalmente ha uno scatto per evento.
// Se invia due eventi per scatto, usare 2 anziche' 1.
const int8_t ENCODER_EVENTS_PER_STEP = 1;

// ============================================================
// STATO PULSANTI
// ============================================================

bool buttonStable[BUTTON_COUNT];
bool buttonLastReading[BUTTON_COUNT];
unsigned long buttonDebounceAt[BUTTON_COUNT];

bool encoderSwStable = HIGH;
bool encoderSwLastReading = HIGH;
unsigned long encoderSwDebounceAt = 0;

// ============================================================
// STATO ENCODER
// ============================================================

uint8_t encoderState;
int8_t encoderDelta = 0;

// Tabella standard di transizione quadratura.
// Valori positivi/negativi rappresentano i due versi.
const int8_t ENCODER_TRANSITIONS[16] = {
   0, -1,  1,  0,
   1,  0,  0, -1,
  -1,  0,  0,  1,
   0,  1, -1,  0
};

// ============================================================
// SETUP
// ============================================================

void setup() {
  // Encoder su modulo: i pin sono gia' collegati al circuito del modulo,
  // ma le pull-up interne rendono la lettura piu' robusta.
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);

  // Comune pulsanti usato come massa software.
  pinMode(BUTTON_COMMON, OUTPUT);
  digitalWrite(BUTTON_COMMON, LOW);

  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    buttonStable[i] = digitalRead(BUTTON_PINS[i]);
    buttonLastReading[i] = buttonStable[i];
    buttonDebounceAt[i] = 0;
  }

  encoderSwStable = digitalRead(ENC_SW);
  encoderSwLastReading = encoderSwStable;

  encoderState = readEncoderState();

  Keyboard.begin();
  Consumer.begin();
}

// ============================================================
// LOOP
// ============================================================

void loop() {
  readEncoderRotation();
  readEncoderSwitch();
  readButtons();
}

// ============================================================
// ENCODER
// ============================================================

uint8_t readEncoderState() {
  // Bit 1 = CLK, bit 0 = DT.
  return (digitalRead(ENC_CLK) << 1) | digitalRead(ENC_DT);
}

void readEncoderRotation() {
  uint8_t newState = readEncoderState();

  if (newState == encoderState) {
    return;
  }

  uint8_t index = (encoderState << 2) | newState;
  encoderDelta += ENCODER_TRANSITIONS[index];
  encoderState = newState;

  if (encoderDelta >= ENCODER_EVENTS_PER_STEP) {
    encoderDelta = 0;

    if (REVERSE_ENCODER) {
      sendEncoderMediaKey(ENCODER_CCW_MEDIA_KEY);
    } else {
      sendEncoderMediaKey(ENCODER_CW_MEDIA_KEY);
    }
  }
  else if (encoderDelta <= -ENCODER_EVENTS_PER_STEP) {
    encoderDelta = 0;

    if (REVERSE_ENCODER) {
      sendEncoderMediaKey(ENCODER_CW_MEDIA_KEY);
    } else {
      sendEncoderMediaKey(ENCODER_CCW_MEDIA_KEY);
    }
  }
}

void sendEncoderMediaKey(ConsumerKeycode key) {
  Consumer.write(key);
}

void readEncoderSwitch() {
  bool reading = digitalRead(ENC_SW);

  if (reading != encoderSwLastReading) {
    encoderSwDebounceAt = millis();
  }

  if (millis() - encoderSwDebounceAt >= ENCODER_SW_DEBOUNCE_MS) {
    if (reading != encoderSwStable) {
      encoderSwStable = reading;

      if (encoderSwStable == LOW) {
        sendEncoderClick();
      }
    }
  }

  encoderSwLastReading = reading;
}

// ============================================================
// PULSANTI
// ============================================================

void readButtons() {
  for (uint8_t i = 0; i < BUTTON_COUNT; i++) {
    bool reading = digitalRead(BUTTON_PINS[i]);

    if (reading != buttonLastReading[i]) {
      buttonDebounceAt[i] = millis();
    }

    if (millis() - buttonDebounceAt[i] >= BUTTON_DEBOUNCE_MS) {
      if (reading != buttonStable[i]) {
        buttonStable[i] = reading;

        // Evento solo alla pressione, non al rilascio.
        if (buttonStable[i] == LOW) {
          sendButtonAction(i);
        }
      }
    }

    buttonLastReading[i] = reading;
  }
}

void sendButtonAction(uint8_t index) {
  if (index >= BUTTON_COUNT) {
    return;
  }

  const ButtonAction &action = BUTTON_ACTIONS[index];

  if (action.type == ACTION_KEY_COMBO) {
    if (action.modifier != NO_MODIFIER) {
      Keyboard.press(action.modifier);
    }
    Keyboard.press(action.key);
    delay(10);
    Keyboard.releaseAll();
  } else if (action.type == ACTION_MEDIA_KEY) {
    Consumer.write(action.mediaKey);
  }
}

// ============================================================
// FUNZIONI DA MODIFICARE PER LE MACRO
// ============================================================

void sendEncoderClick() {
  // Funzione separata per rendere facile sostituire il comportamento.
  // Esempio combinazione di tastiera al click:
  // Keyboard.press(KEY_LEFT_CTRL);
  // Keyboard.press('m');
  // delay(10);
  // Keyboard.releaseAll();

  Consumer.write(ENCODER_CLICK_MEDIA_KEY);
}
