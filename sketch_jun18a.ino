// Using Arduino MEGA's pin 3 (PORT E's 4th pin).
// 4th bit from MSB, not LSB.
//
int a[16];
int Pin[16] = {A0, A1, A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};

void setup() {
  // Pin 3 for clock output
  DDRE = B00010000;
  
  pinMode(A0, INPUT);

  // See dump at serial terminal
  Serial.begin(57600);
  Serial.println("HELLO");
}

void loop() {
  int i;
  while (1) {    
    // roughly 2usec clock
    PORTE = B00000000;
    delayMicroseconds(1);
    PORTE = B00010000;
    //delayMicroseconds(10000);
    delay(50);

    Serial.print("ADDRESS:");
    for (i = 15; i >= 0; i--) {
      a[i] = digitalRead(Pin[i]);
      Serial.print(a[i]);
    }
    Serial.println();    
  }
}

