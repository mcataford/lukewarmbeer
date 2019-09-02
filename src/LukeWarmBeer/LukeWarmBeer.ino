/*
 * Pin mapping:
 * A0: Joystick A
 * A1: Joystick B
 * 3: Step A
 * 4: Direction A
 * 
 */

int DIRECTION_THRESHOLD = 512;
int TOLERANCE = 10;
int ANALOG_READ_MAX = 1023;

int joystick_A_pin = A0;
int joystick_B_pin = A1;
int motor_A_direction_pin = 4;
int motor_A_step_pin = 3;

int joystick_A = 0;
int joystick_B = 0;

void setup() {
  // For easy debugging.
  Serial.begin(9600);

  pinMode(motor_A_direction_pin, OUTPUT);
  pinMode(motor_A_step_pin, OUTPUT);

  // This sets up the PWM to drive the step pin.
  analogWrite(motor_A_step_pin, 128);
}

int get_direction(int joystick_reading) {
  // If the value is invalid, ignore.
  if (joystick_reading < 0 || joystick_reading > ANALOG_READ_MAX)
    return 0;
  
  int low_tol = DIRECTION_THRESHOLD - TOLERANCE;
  int high_tol = DIRECTION_THRESHOLD + TOLERANCE;

  if (joystick_reading < low_tol)
    return -1;
  else if (joystick_reading > high_tol) {
    return 1;  
  } else {
    return 0;  
  }
}

void loop() {
    joystick_A = analogRead(A0);
    joystick_B = analogRead(A1);
    
    int direction_A = get_direction(joystick_A);
    int direction_B = get_direction(joystick_B);

    // Setting the direction pin based on joystick_A value.
    if (direction_A > 0) {
      digitalWrite(motor_A_direction_pin, HIGH);
    }
    else if (direction_A < 0) {
      digitalWrite(motor_A_direction_pin, LOW);
    }
 
    delay(100);
}
