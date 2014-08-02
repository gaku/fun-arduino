#include "acia6551.h"

context ctx;

// Arduino Mega's digital pin#s are printed on the pcb.
#define RESB 4
#define PHI2 27
#define CS0  2
#define CS1B 3
#define RS1 14
#define RS0 13
#define RWB 28
#define D7  25
#define D6  24
#define D5  23
#define D4  22
#define D3  21
#define D2  20
#define D1  19
#define D0  18

// typedef struct context context;

int dataBusPins[] = {D0, D1, D2, D3, D4, D5, D6, D7};

void phi2(int state) {
  digitalWrite(PHI2, state);
  delayMicroseconds(1);
}

void readDataBus(context* ctx) { 
  for (int i = 0; i < 8; i++) {
    ctx->dataBus[i] = digitalRead(dataBusPins[i]);
  }
}

void dumpDataBus(context* ctx) {
  for (int i = 7; i >= 0; i--) {
    Serial.print(ctx->dataBus[i]);
  }
  Serial.println("");
}

void setup() {
  Serial.begin(57600);
  Serial.println("START");
  pinMode(RESB, OUTPUT);
  pinMode(PHI2, OUTPUT);
  pinMode(CS0, OUTPUT);
  pinMode(CS1B, OUTPUT);
  pinMode(RS1, OUTPUT);
  pinMode(RS0, OUTPUT);
  pinMode(RWB, OUTPUT);
  
  pinMode(D7, INPUT);
  pinMode(D6, INPUT);
  pinMode(D5, INPUT);
  pinMode(D4, INPUT);
  pinMode(D3, INPUT);
  pinMode(D2, INPUT);
  pinMode(D1, INPUT);
  pinMode(D0, INPUT);
  
  phi2(HIGH);

  // Reset 6551
  // RESB to LOW
  digitalWrite(RESB, LOW);
  // PHI2 clock H->L.  Minimum 175ns.
  phi2(LOW);
  phi2(HIGH);
  phi2(LOW);
  phi2(HIGH);
  phi2(LOW);
  digitalWrite(RESB, HIGH);

  // Select 6551
  digitalWrite(CS0, HIGH);
  digitalWrite(CS1B, LOW);
  
  // Read status register
  // RS1 - L
  // RS0 - H
  // RWB - H
  digitalWrite(RS1, LOW);
  digitalWrite(RS0, HIGH);
  digitalWrite(RWB, HIGH);
  phi2(HIGH);
  // READ DATA BUS
  readDataBus(&ctx);
  phi2(LOW);
  dumpDataBus(&ctx);

}

void loop() {
  phi2(HIGH);
  phi2(LOW);
}



