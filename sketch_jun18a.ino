int pin = 52;

void setup() {
  DDRE = B11111111;
}

void loop() {
  while (1) {    
    PORTE = B0000000;
    PORTE = B0000000;
    PORTE = B0000000;
    PORTE = B1111111;
  }
}

