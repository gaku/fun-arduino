// Arduino Mega's digital pin#s are printed on the pcb.
#define RESB 4
#define PHI2 27
#define CS0  2
#define CS1B 3


void phi2(int state) {
  digitalWrite(PHI2, state);
  delayMicroseconds(1);
}

void setup() {
  pinMode(RESB, OUTPUT);
  pinMode(PHI2, OUTPUT);
  pinMode(CS0, OUTPUT);
  pinMode(CS1B, OUTPUT);
  
  phi2(HIGH);

  // Reset 6551
  // RESB to LOW
  digitalWrite(RESB, LOW);
  // PHI2 clock H->L.  Minimum 175ns.
  phi2(LOW);
  phi2(HIGH);
  phi2(LOW);
  
  // Select 6551
  digitalWrite(CS0, HIGH);
  digitalWrite(CS1B, LOW);
  
  // Read status register
  
}

void loop() {
  digitalWrite(PHI2, HIGH);
  delay(1000);
  digitalWrite(PHI2, LOW);
  delay(1000);
}


