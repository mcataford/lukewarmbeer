// Constants
const int OFF = 0;
const int ON = 1;

// Inputs
const int L_UP = A5;
const int L_DOWN = A4;
const int R_UP = A3;
const int R_DOWN = A2;

const int L_BOTTOM = A1;
const int R_BOTTOM = A0;

const int BALL_RETURN = 2;
const int LEVEL_SENSOR = 3;
const int START = 7;

// Level setters
// can also use PORTD B00011011 to set these
const int LEVEL_SET_A = 0;
const int LEVEL_SET_B = 1;
const int LEVEL_SET_C = 3;
const int LEVEL_SET_D = 4;

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
  Setup
 */

void setup() {
  configureInputs();
  configureLeftMotor();
  configureRightMotor();
  configureDisplay();
}

void configureInputs() {
  pinMode(L_DOWN, INPUT_PULLUP);
  pinMode(L_UP, INPUT_PULLUP);
  pinMode(R_DOWN, INPUT_PULLUP);
  pinMode(R_UP, INPUT_PULLUP);
  pinMode(BALL_RETURN, INPUT_PULLUP);
  pinMode(LEVEL_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BALL_RETURN), ballReturn, FALLING);
  attachInterrupt(digitalPinToInterrupt(LEVEL_SENSOR), levelSensor, FALLING);
}

void ballReturn() {
}

void levelSensor() {
}

void configureLeftMotor() {
  pinMode(L_MOTOR_DIRECTION, OUTPUT);
  pinMode(L_MOTOR_STEP, OUTPUT);
  // timer 1, output A, pin 9
  // Section 15 in datasheet
  // fast PWM with top limit set by OCR1A
  // clock prescale starts 64
  TCCR1A = bit(COM1A0) | bit(WGM11) | bit(WGM10);
  TCCR1B = bit(WGM13) | bit(WGM12) | bit(CS11) | bit(CS10);
  OCR1A = 75;
}

void configureRightMotor() {
  pinMode(R_MOTOR_DIRECTION, OUTPUT);
  pinMode(R_MOTOR_STEP, OUTPUT);
  // timer 2, ouput A, pin 11
  // Section 17 in datasheet
  // fast PWM with top limit set by OCR2A, inverting
  // clock prescale starts at 64
  TCCR2A = bit(COM2A0) | bit(WGM21) | bit(WGM20);
  TCCR2B = bit(WGM22) | bit(CS22);
  OCR2A = 200;
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
    setRegister(MAX7219_REG_SHUTDOWN, OFF);   // turn off display
    setRegister(MAX7219_REG_DISPTEST, OFF);   // turn off test mode
    setRegister(MAX7219_REG_INTENSITY, 0x0D); // display intensity
}

/******************************************************************************
  Loop
 */

void loop() {
  controlMotors();
  /* setDisplay(); */
}

void testDisplay() {
  String score = "01234567";

  setRegister(MAX7219_REG_SHUTDOWN, OFF); // turn off
  setRegister(MAX7219_REG_SCANLIMIT, 7); // limit to 8 digits
  setRegister(MAX7219_REG_DECODE, 0b11111111); // decode all digits
  setRegister(1, score.charAt(0));
  setRegister(2, score.charAt(1));
  setRegister(3, score.charAt(2));
  setRegister(4, score.charAt(3));
  setRegister(5, score.charAt(4));
  setRegister(6, score.charAt(5));
  setRegister(7, score.charAt(6));
  setRegister(8, score.charAt(7));
  setRegister(MAX7219_REG_SHUTDOWN, ON); // turn on

  delay(100);
}

void setRegister(byte reg, byte value)
{
    digitalWrite(MAX7219_CS, LOW);
    shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, reg);
    shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, value);
    digitalWrite(MAX7219_CS, HIGH);
}

void controlMotors() {
    boolean l_down = !digitalRead(L_DOWN);
    boolean l_up = !digitalRead(L_UP);

    boolean r_down = !digitalRead(R_DOWN);
    boolean r_up = !digitalRead(R_UP);

    if (l_down) {
      digitalWrite(L_MOTOR_DIRECTION, HIGH);
      setLeftSpeed(32);
    } else if (l_up) {
      digitalWrite(L_MOTOR_DIRECTION, LOW);
      setLeftSpeed(16);
    } else {
      setLeftSpeed(0);
    }

    if (r_down) {
      digitalWrite(R_MOTOR_DIRECTION, HIGH);
      setRightSpeed(150);
    } else if (r_up) {
      digitalWrite(R_MOTOR_DIRECTION, LOW);
      setRightSpeed(220);
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
