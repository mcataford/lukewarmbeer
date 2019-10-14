const int L_DOWN = 9;
const int L_UP = 8;
const int R_DOWN = 12;
const int R_UP = 13;

const int L_MOTOR_DIRECTION = 4;
const int L_MOTOR_STEP = 10;
const int R_MOTOR_DIRECTION = 7;
const int R_MOTOR_STEP = 3;

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

void setLeftSpeed(int count) {
  // use timer 1, output B, pin 10
  OCR1A = count;
  OCR1B = count / 2;
}

void setRightSpeed(int speed) {
  // use timer 2, output B, pin 3
  OCR2A = count;
  OCR2B = count / 2;
}

void loop() {
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
