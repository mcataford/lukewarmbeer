/*
 * Pin mapping:
 * A0: Joystick A
 * A1: Joystick B
 * 3: Step A
 * 4: Direction A
 * 
 */

int joystick_A_UP = 13;
int joystick_A_DOWN = 12;
int joystick_B_UP = 11;
int joystick_B_DOWN = 10;

int motor_A_direction_pin = 4;
int motor_A_step_pin = 3;

int motor_B_direction_pin = 2;
int motor_B_step_pin = 5;

void setup() {
  Serial.begin(9600);
  
  pinMode(motor_A_direction_pin, OUTPUT);
  pinMode(motor_A_step_pin, OUTPUT);

  pinMode(motor_B_direction_pin, OUTPUT);
  pinMode(motor_B_step_pin, OUTPUT);
  
  pinMode(joystick_A_UP, INPUT);
  pinMode(joystick_A_DOWN, INPUT);
  pinMode(joystick_B_UP, INPUT);
  pinMode(joystick_B_DOWN, INPUT);
  
  // This sets up the PWM to drive the step pin.
  analogWrite(motor_A_step_pin, 128);
}

void setMotor(int motorID, int direction) {
  int target_direction_pin;
  int target_step_pin;

  if (motorID == 1) {
    target_direction_pin = motor_A_direction_pin;
    target_step_pin = motor_A_step_pin;
  } else if (motorID == 2) {
    target_direction_pin = motor_B_direction_pin;
    target_step_pin = motor_B_step_pin;
  }
  
  if (direction == 0) {
    analogWrite(target_step_pin, 0);
  } else {
    analogWrite(target_step_pin, 128);
  }

  if (direction > 0) {
    digitalWrite(target_direction_pin, HIGH);
  } else {
    digitalWrite(target_direction_pin, LOW);
  }
}

void loop() {
    boolean A_up = digitalRead(joystick_A_UP);
    boolean A_down = digitalRead(joystick_A_DOWN);
    boolean B_up = digitalRead(joystick_B_UP);
    boolean B_down = digitalRead(joystick_B_DOWN);
        
    if (A_up && !A_down) {
      setMotor(1, 1);
    } else if (A_down && !A_up) {
      setMotor(1, -1);
    } else {
      setMotor(1, 0);
    }
    
    if (B_up && !B_down) {
      setMotor(2, 1);
    } else if (B_down && !B_up) {
      setMotor(2, -1);
    } else {
      setMotor(2, 0);
    }
 
    delay(100);
}
