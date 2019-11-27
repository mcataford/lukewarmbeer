/******************************************************************************
  Seven segment decoding alphabet
 */

const unsigned char seven_seg_digits_decode_abcdefg[75]= {
/*  0     1     2     3     4     5     6     7     8     9     :     ;     */
    0x7E, 0x30, 0x6D, 0x79, 0x33, 0x5B, 0x5F, 0x70, 0x7F, 0x7B, 0x00, 0x00,
/*  <     =     >     ?     @     A     B     C     D     E     F     G     */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x00, 0x4E, 0x00, 0x4F, 0x47, 0x5E,
/*  H     I     J     K     L     M     N     O     P     Q     R     S     */
    0x37, 0x06, 0x3C, 0x00, 0x0E, 0x00, 0x00, 0x7E, 0x67, 0x00, 0x00, 0x5B,
/*  T     U     V     W     X     Y     Z     [     \     ]     ^     _     */
    0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/*  `     a     b     c     d     e     f     g     h     i     j     k     */
    0x00, 0x7D, 0x1F, 0x0D, 0x3D, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00,
/*  l     m     n     o     p     q     r     s     t     u     v     w     */
    0x00, 0x00, 0x15, 0x1D, 0x00, 0x73, 0x05, 0x00, 0x0F, 0x1C, 0x00, 0x00,
/*  x     y     z     */
    0x00, 0x3B, 0x00
};

/* Invalid letters are mapped to all segments off (0x00). */
unsigned char decode_7seg(unsigned char chr)
{ /* assuming implementation uses ASCII */
    if (chr > (unsigned char)'z')
        return 0x00;
    return seven_seg_digits_decode_abcdefg[chr - '0'];
}

/******************************************************************************
  Constants
 */

// Constants
const int OFF = 0;
const int ON = 1;
const int UP_THRESHOLD = 750;
const int DOWN_THRESHOLD = 300;
const bool UP = HIGH;
const bool DOWN = LOW;
const int SCREEN_SCROLL_DELAY = 350; // ms
const int SCREEN_BLINK_DELAY = 1000; // ms

// Inputs
const int L_JOYSTICK = A3;
const int R_JOYSTICK = A2;

const int L_BOTTOM = A1;
const int R_BOTTOM = A0;

const int BALL_RETURN = 2;
const int LEVEL_SENSOR = 3;
const int START = 7;

// Level setters
// can also use PORTD B00110011 to set these
const int LEVEL_SET_A = 0;
const int LEVEL_SET_B = 1;
const int LEVEL_SET_C = 4;
const int LEVEL_SET_D = 5;

// Motor outputs
const int L_MOTOR_DIRECTION = 8;
const int L_MOTOR_STEP = 9;
const int R_MOTOR_DIRECTION = 10;
const int R_MOTOR_STEP = 11;

// MAX7219 LED Driver pins
const int MAX7219_DIN = 6;
const int MAX7219_CLK = 13;
const int MAX7219_CS = 12;

// Register values. See MAX7219 Datasheet, Table 2, page 7
const int MAX7219_REG_DECODE = 0x09;
const int MAX7219_REG_INTENSITY = 0x0A;
const int MAX7219_REG_SCANLIMIT = 0x0B;
const int MAX7219_REG_SHUTDOWN = 0x0C;
const int MAX7219_REG_DISPTEST = 0x0F;


/******************************************************************************
  State and Variables
 */

enum CurrentState {
                    START_STATE,
                    PLAY_STATE,
                    LEVEL_LOST_STATE,
                    LEVEL_WON_STATE,
                    GAME_LOST_STATE,
                    GAME_WON_STATE,
                    ENTER_SCORE_STATE,
};

volatile bool didTriggerBallReturn = false;
volatile bool didTriggerLevelComplete = false;

struct GameState {
  unsigned long lastMillis;
  int topRowScrollPosition;
  int bottomRowScrollPosition;
  bool blinkState;
  int ballsLeft;
  int currentLevel;
  int score;
  int bonus;
  CurrentState currentState;
};

struct GameState state;

/******************************************************************************
  Setup
 */

void setup() {
  Serial.begin(9600);
  configureIO();
  configureLeftMotor();
  configureRightMotor();
  configureDisplay();
  initializeState();
}

void initializeState() {
  state.lastMillis = millis();
  state.topRowScrollPosition = 0;
  state.bottomRowScrollPosition = 0;
  state.blinkState = true;
  state.ballsLeft = 3;
  state.currentLevel = 1;
  state.score = 0;
  state.bonus = 0;
  state.currentState = START_STATE;
}

void configureIO() {
  pinMode(LEVEL_SET_A, OUTPUT);
  pinMode(LEVEL_SET_B, OUTPUT);
  pinMode(LEVEL_SET_C, OUTPUT);
  pinMode(LEVEL_SET_D, OUTPUT);
  digitalWrite(LEVEL_SET_A, LOW);
  digitalWrite(LEVEL_SET_B, LOW);
  digitalWrite(LEVEL_SET_C, LOW);
  digitalWrite(LEVEL_SET_D, LOW);

  pinMode(L_JOYSTICK, INPUT);
  pinMode(R_JOYSTICK, INPUT);
  pinMode(L_BOTTOM, INPUT_PULLUP);
  pinMode(R_BOTTOM, INPUT_PULLUP);
  pinMode(BALL_RETURN, INPUT_PULLUP);
  pinMode(LEVEL_SENSOR, INPUT);
  pinMode(START, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BALL_RETURN), ballReturn, FALLING);
  attachInterrupt(digitalPinToInterrupt(LEVEL_SENSOR), levelSensor, RISING);
}

void configureLeftMotor() {
  pinMode(L_MOTOR_DIRECTION, OUTPUT);
  pinMode(L_MOTOR_STEP, OUTPUT);
  // timer 1, output A, pin 9
  // Section 15 in datasheet
  // fast PWM with top limit set by OCR1A
  // clock prescale starts 8
  TCCR1A = bit(COM1A0) | bit(WGM11) | bit(WGM10);
  TCCR1B = bit(WGM13) | bit(WGM12) | bit(CS11);
  OCR1A = 75;

  setLeftSpeed(0);
}

void configureRightMotor() {
  pinMode(R_MOTOR_DIRECTION, OUTPUT);
  pinMode(R_MOTOR_STEP, OUTPUT);
  // timer 2, ouput A, pin 11
  // Section 17 in datasheet
  // fast PWM with top limit set by OCR2A, inverting
  // clock prescale starts at 8
  TCCR2A = bit(COM2A0) | bit(WGM21) | bit(WGM20);
  TCCR2B = bit(WGM22) | bit(CS21);
  OCR2A = 200;

   setRightSpeed(0);
}

void configureDisplay() {
  pinMode(MAX7219_DIN, OUTPUT);
  pinMode(MAX7219_CS, OUTPUT);
  pinMode(MAX7219_CLK, OUTPUT);
  digitalWrite(MAX7219_CS, HIGH);
  resetDisplay();
}

void resetDisplay()
{
    setRegister(MAX7219_REG_SHUTDOWN, ON);   // turn off display
    setRegister(MAX7219_REG_DISPTEST, OFF);   // turn off test mode
    setRegister(MAX7219_REG_INTENSITY, 0x07); // display intensity
}


/******************************************************************************
  Interrupts
 */

void ballReturn() {
  didTriggerBallReturn = true;
}

void levelSensor() {
  didTriggerLevelComplete = true;
}


/******************************************************************************
  Loop
 */

void loop() {

  char topDisplay[5] = "zzzz";
  char bottomDisplay[5] = "zzzz";
  unsigned long currentMillis = millis();

  if(state.currentState == START_STATE) {
    char topRow[] =    "PrESS     ";
    char bottomRow[] = "     StArt";

    for (int i = 0; i < 4; i++) {
      topDisplay[i] = topRow[(state.topRowScrollPosition + i) % strlen(topRow)];
      bottomDisplay[i] = bottomRow[(state.bottomRowScrollPosition + i) % strlen(bottomRow)];
    }

    if (currentMillis - state.lastMillis > SCREEN_SCROLL_DELAY) {
      state.topRowScrollPosition = (state.topRowScrollPosition + 1) % strlen(topRow);
      state.bottomRowScrollPosition = (state.bottomRowScrollPosition + 1) % strlen(bottomRow);
      state.lastMillis = currentMillis;

      updateDisplay(topDisplay, bottomDisplay);
    }

    if (!digitalRead(START)) {
      state.score = 0;
      state.bonus = 999;
      state.currentLevel = 1;
      state.ballsLeft = 3;
      state.currentState = PLAY_STATE;
      didTriggerLevelComplete = false;
      didTriggerBallReturn = false;
    }
  }

  else if (state.currentState == PLAY_STATE) {
    /* setLevel(state.currentLevel); */

    strcpy(bottomDisplay, "zzzz");
    String(state.score).toCharArray(bottomDisplay, 4);

    String(state.bonus).toCharArray(topDisplay, 4);
    for (int i = 4; i > 0; i--){
      topDisplay[i] = topDisplay[i-1];
    }
    if (currentMillis - state.lastMillis > SCREEN_BLINK_DELAY) {
      state.blinkState = !state.blinkState;
      state.lastMillis = currentMillis;
    }
    if (state.blinkState) {
      topDisplay[0] = state.ballsLeft+'0';
    } else {
      topDisplay[0] = 'z';
    }

    updateDisplay(topDisplay, bottomDisplay);
    controlMotors();

    if (didTriggerBallReturn) {
      if (didTriggerLevelComplete) {
        Serial.println("WIN");
        state.currentLevel += 1;
        state.currentState = LEVEL_WON_STATE;
      } else {
        Serial.println("LOSE");
        state.ballsLeft -= 1;
        state.currentState = LEVEL_LOST_STATE;
      }
      didTriggerLevelComplete = false;
      didTriggerBallReturn = false;
    }
  }

  else if (state.currentState == LEVEL_WON_STATE) {
    strcpy(topDisplay, "Good");
    strcpy(bottomDisplay, "Good");
    updateDisplay(topDisplay, bottomDisplay);
    if (!digitalRead(START)) {
      state.currentState = PLAY_STATE;
    }
  }

  else if (state.currentState == LEVEL_LOST_STATE) {
    strcpy(topDisplay, "LOSE");
    strcpy(bottomDisplay, "LOSE");
    updateDisplay(topDisplay, bottomDisplay);
    if (!digitalRead(START)) {
      state.currentState = PLAY_STATE;
    }
  }
}

void setLevel(int level) {
  PORTD = 0b00110011 & ((level & 0b00001100) << 2) & (level & 0b00000011);
}

void updateDisplay(char topRow[5], char bottomRow[5]) {
  setRegister(MAX7219_REG_SHUTDOWN, OFF); // turn off
  setRegister(MAX7219_REG_SCANLIMIT, 7); // 8 digits
  setRegister(MAX7219_REG_DECODE, 0b00000000); // don't decode digits

  setRegister(1, decode_7seg(topRow[0]));
  setRegister(2, decode_7seg(topRow[1]));
  setRegister(3, decode_7seg(topRow[2]));
  setRegister(4, decode_7seg(topRow[3]));

  setRegister(5, decode_7seg(bottomRow[0]));
  setRegister(6, decode_7seg(bottomRow[1]));
  setRegister(7, decode_7seg(bottomRow[2]));
  setRegister(8, decode_7seg(bottomRow[3]));

  setRegister(MAX7219_REG_SHUTDOWN, ON); // turn on
}

void setRegister(byte reg, byte value)
{
    digitalWrite(MAX7219_CS, LOW);
    shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, reg);
    shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, value);
    digitalWrite(MAX7219_CS, HIGH);
}

void controlMotors() {
    int l_joystick = analogRead(L_JOYSTICK);
    int r_joystick = analogRead(R_JOYSTICK);

    if (l_joystick > UP_THRESHOLD) {
      digitalWrite(L_MOTOR_DIRECTION, UP);
      setLeftSpeed(128);
    } else if (l_joystick > DOWN_THRESHOLD) {
      digitalWrite(L_MOTOR_DIRECTION, DOWN);
      setLeftSpeed(128);
    } else {
      setLeftSpeed(0);
    }

    if (r_joystick > UP_THRESHOLD) {
      digitalWrite(R_MOTOR_DIRECTION, UP);
      setRightSpeed(128);
    } else if (r_joystick > DOWN_THRESHOLD) {
      digitalWrite(R_MOTOR_DIRECTION, DOWN);
      setRightSpeed(128);
    } else {
      setRightSpeed(0);
    }
}

void setLeftSpeed(int count) {
  // set the max count value and the count at which the output toggles
  // max count is 256
  OCR1A = count;
  if (count == 0) {
    bitClear(TCCR1A, COM1A0);
    return;
  } else if (!bitRead(TCCR1A, COM1A0)) {
    bitSet(TCCR1A, COM1A0);
  }
}

void setRightSpeed(int count) {
  // set the max count value and the count at which the output toggles
  // max count is 256
  OCR2A = count;
  if (count == 0) {
    bitClear(TCCR2A, COM2A0);
    return;
  } else if (!bitRead(TCCR2A, COM2A0)) {
    bitSet(TCCR2A, COM2A0);
  }
}
