// Constants
const int OFF = 0;
const int ON = 1;

// Inputs
const int L_DOWN = 9;
const int L_UP = 8;
const int R_DOWN = 1;
const int R_UP = 0;

// Motor outputs
const int L_MOTOR_DIRECTION = 4;
const int L_MOTOR_STEP = 10;
const int R_MOTOR_DIRECTION = 7;
const int R_MOTOR_STEP = 3;

// MAX7219 LED Driver pins
const int MAX7219_DIN = 6;
const int MAX7219_CLK = 13;
const int MAX7219_CS = 12;

// Register values. See MAX7219 Datasheet, Table 2, page 7
const int MAX7219_REG_DECODE = 0x09;
const int MAX7219_REG_INTENSITY = 0x0A;
const int MAX7219_REG_SCANLIMIT = 0x0B;
const int  MAX7219_REG_SHUTDOWN = 0x0C;
const int MAX7219_REG_DISPTEST = 0x0F;

/******************************************************************************
  Setup
 */

void setup() {
  pinMode(L_MOTOR_DIRECTION, OUTPUT);
  pinMode(L_MOTOR_STEP, OUTPUT);
  pinMode(R_MOTOR_DIRECTION, OUTPUT);
  pinMode(R_MOTOR_STEP, OUTPUT);

  pinMode(L_DOWN, INPUT_PULLUP);
  pinMode(L_UP, INPUT_PULLUP);
  pinMode(R_DOWN, INPUT_PULLUP);
  pinMode(R_UP, INPUT_PULLUP);

  configureLeftMotor();
  configureRightMotor();

  configureDisplay();
}

void configureLeftMotor() {
  // timer 1, output B
  // fast PWM with top limit set by OCR1A
  // clock prescale starts at max: 1024
  TCCR1A = _BV(COM1B1) | _BV(WGM11) | _BV(WGM10);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS12) | _BV(CS10);
  OCR1A = 255;
  OCR1B = 128;
}

void configureRightMotor() {
  // timer 2, ouput B
  // fast PWM with top limit set by OCR2A
  // clock prescale starts at max: 1024
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(WGM22) | _BV(CS22) | _BV(CS21) | _BV(CS20);
  OCR2A = 255;
  OCR2B = 128;
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

  /* setRegister(MAX7219_REG_DISPTEST, OFF); // disp test for debugging */

  delay(1000);

    /* int switch_state = digitalRead(SWITCH_PIN); */

    /* if (switch_state) { */
    /*     digitalWrite(LED_PIN, HIGH); */
    /* } else { */
    /*     digitalWrite(LED_PIN, LOW); */
    /* } */
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
      analogWrite(L_MOTOR_STEP, 124);
    } else if (l_up) {
      digitalWrite(L_MOTOR_DIRECTION, LOW);
      analogWrite(L_MOTOR_STEP, 124);
    } else {
      analogWrite(L_MOTOR_STEP, 0);
    }

    if (r_down) {
      digitalWrite(R_MOTOR_DIRECTION, HIGH);
      analogWrite(R_MOTOR_STEP, 124);
    } else if (r_up) {
      digitalWrite(R_MOTOR_DIRECTION, LOW);
      analogWrite(R_MOTOR_STEP, 124);
    } else {
      analogWrite(R_MOTOR_STEP, 0);
    }
}

void setLeftSpeed(int count) {
  // use timer 1, output B, pin 10
  OCR1A = count;
  OCR1B = count / 2;
}

void setRightSpeed(int count) {
  // use timer 2, output B, pin 3
  OCR2A = count;
  OCR2B = count / 2;
}
