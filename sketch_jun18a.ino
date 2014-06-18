int pin = 13;

void setup() {
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void loop() {
  digitalWrite(pin, LOW);
  digitalWrite(pin, HIGH);
}

