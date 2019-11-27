// Constants
const int OFF = 0;
const int ON = 1;
const int UP_THRESHOLD = 750;
const int DOWN_THRESHOLD = 300;
const bool UP = HIGH;
const bool DOWN = LOW;

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
  configureIO();
  configureLeftMotor();
  configureRightMotor();
  configureDisplay();
  initializeState();
}

void initializeState() {
  state.ballsLeft = 0;
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
  // set to start at 8 (level 1)
  digitalWrite(LEVEL_SET_A, LOW);
  digitalWrite(LEVEL_SET_B, LOW);
  digitalWrite(LEVEL_SET_C, LOW);
  digitalWrite(LEVEL_SET_D, HIGH);

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

  String topRow, bottomRow;

  switch (state.currentState) {

  case START_STATE:
    topRow = "1111";
    bottomRow = "1111";
    updateDisplay(topRow, bottomRow);
    if (!digitalRead(START)) {
      state.currentState = PLAY_STATE;
    }
    break;

  case PLAY_STATE:
    topRow = "2222";
    bottomRow = "2222";
    updateDisplay(topRow, bottomRow);
    controlMotors();
    break;

  case LEVEL_LOST_STATE:
    break;

  case LEVEL_WON_STATE:
    break;

  case GAME_LOST_STATE:
    break;

  case GAME_WON_STATE:
    break;

  case ENTER_SCORE_STATE:
    break;
  }

  /* if (didTriggerBallReturn) { */
  /*   topRow = "3333"; */
  /*   didTriggerBallReturn = false; */
  /* } */
  /* if (didTriggerLevelComplete) { */
  /*   bottomRow = "4444"; */
  /*   didTriggerLevelComplete = false; */
  /* } */

  /* if (!digitalRead(L_BOTTOM)) { */
  /*   topRow = "5555"; */
  /* } */
  /* if (!digitalRead(R_BOTTOM)) { */
  /*   bottomRow = "6666"; */
  /* } */
  /* if (!digitalRead(START)) { */
  /*   bottomRow = "7777"; */
  /* } */
}

void updateDisplay(String topRow, String bottomRow) {
  setRegister(MAX7219_REG_SHUTDOWN, OFF); // turn off
  setRegister(MAX7219_REG_SCANLIMIT, 7); // 8 digits
  setRegister(MAX7219_REG_DECODE, 0b11111111); // decode all digits
  // legal characters to decode: 0123456789-HELP

  setRegister(1, topRow.charAt(0));
  setRegister(2, topRow.charAt(1));
  setRegister(3, topRow.charAt(2));
  setRegister(4, topRow.charAt(3));

  setRegister(5, bottomRow.charAt(0));
  setRegister(6, bottomRow.charAt(1));
  setRegister(7, bottomRow.charAt(2));
  setRegister(8, bottomRow.charAt(3));

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
