const int L_DOWN = 9;
const int L_UP = 8;
const int R_DOWN = 12;
const int R_UP = 10;

const int L_MOTOR_DIRECTION = 4;
const int L_MOTOR_STEP = 5;
const int R_MOTOR_DIRECTION = 7;
const int R_MOTOR_STEP = 6;

void setup() {
  pinMode(L_MOTOR_DIRECTION, OUTPUT);
  pinMode(L_MOTOR_STEP, OUTPUT);
  pinMode(R_MOTOR_DIRECTION, OUTPUT);
  pinMode(R_MOTOR_STEP, OUTPUT);

  pinMode(L_DOWN, INPUT_PULLUP);
  pinMode(L_UP, INPUT_PULLUP);
  pinMode(R_DOWN, INPUT_PULLUP);
  pinMode(R_UP, INPUT_PULLUP);
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

    delay(100);
}
