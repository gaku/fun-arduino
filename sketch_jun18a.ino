// Using Arduino MEGA's pin 3 (PORT E's 4th pin).
// 4th bit from MSB, not LSB.
//
void setup() {
  DDRE = B00010000;
}

void loop() {
  while (1) {    
    // roughly 2usec clock
    PORTE = B00000000;
    delayMicroseconds(1);
    PORTE = B00010000;
    delayMicroseconds(1);
  }
}

