/*
 * Pin mapping:
 * A0: Joystick A
 * A1: Joystick B
 * 
 */

int DIRECTION_THRESHOLD = 512;
int TOLERANCE = 10;
int ANALOG_READ_MAX = 1023;

int a0 = A0;

int joystick_A = 0;

void setup() {
  // For easy debugging.
  Serial.begin(9600);
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

    int direction_A = get_direction(joystick_A);

    Serial.print("Reading: ");
    Serial.println(joystick_A);
    Serial.print("Direction: ");
    Serial.println(direction_A);

    delay(500);
}
