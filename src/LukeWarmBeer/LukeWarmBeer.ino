int LED_PIN = 13;
int SWITCH_PIN = 8;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);
}

void loop() {
    int switch_state = digitalRead(SWITCH_PIN);

    if (switch_state) {
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }
}
