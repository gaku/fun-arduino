// Using Arduino MEGA's pin 3 (PORT E's 4th pin).
// 4th bit from MSB, not LSB.
//
int a[16];

void setup() {
  // Pin 3 for clock output
  DDRE = B00010000;
  
  pinMode(A0, INPUT);

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
    delay(100);
    a[0] = digitalRead(A0);
    a[1] = digitalRead(A1);
    for (i = 0; i < 2; i++) {
      Serial.print(a[i]);
    }
    Serial.println();    
  }
}

