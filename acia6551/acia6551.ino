#include "acia6551.h"

context ctx;

// Arduino Mega's digital pin#s are printed on the pcb.
#define RESB 4
#define PHI2 27
// USE pin 52 to free up pin 2
#define CS0  52
// Use pin 2 for IRQB(6551's pin 26)
#define IRQB 2
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
int numIrq = 0;

void irqHandler() {
  numIrq++;
}

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

void setDataBusReadWrite(boolean read) {
  if (read) {
    for (int i = 0; i < 8; i++) {
      pinMode(dataBusPins[i], INPUT);
    }
  } else {
    for (int i = 0; i < 8; i++) {
      pinMode(dataBusPins[i], OUTPUT);
    }
  }
}

void writeCharToDataBus(char chr) {
  for (int i = 0; i < 8; i++) {
    int bit = chr & (1 << i);
    Serial.print("bit");
    Serial.print(i);
    Serial.print(":");
    Serial.print(bit);
    digitalWrite(dataBusPins[i], bit);
  }
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
  pinMode(IRQB, INPUT);
  
  setDataBusReadWrite(true);
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
  
  // Read Control Register
  // RS1 - H
  // RS0 - H
  // RWB - H
  digitalWrite(RS1, HIGH);
  digitalWrite(RS0, HIGH);
  digitalWrite(RWB, HIGH);
  
  phi2(HIGH);
  // READ DATA BUS
  readDataBus(&ctx);
  phi2(LOW);
  dumpDataBus(&ctx);
  
  // Set control register
  // RWB - L (write)
  digitalWrite(RWB, LOW);
  setDataBusReadWrite(false);
  // Baud Rate 9600.
  digitalWrite(D0, LOW);
  digitalWrite(D1, HIGH);
  digitalWrite(D2, HIGH);
  digitalWrite(D3, HIGH);
  // Receiver Clock source - Baud rate
  digitalWrite(D4, HIGH);
  // Word length 8
  digitalWrite(D5, LOW);
  digitalWrite(D6, LOW);
  // Stop bit 1
  digitalWrite(D7, LOW);
  phi2(HIGH);
  phi2(LOW);
  
  // Read Control Register
  // RS1 - H
  // RS0 - H
  // RWB - H
  digitalWrite(RS1, HIGH);
  digitalWrite(RS0, HIGH);
  digitalWrite(RWB, HIGH);
  
  phi2(HIGH);
  // READ DATA BUS
  readDataBus(&ctx);
  phi2(LOW);
  dumpDataBus(&ctx);

  attachInterrupt(0, irqHandler, FALLING);

  // Write Transmit Data Register
  // RS1 - L
  // RS0 - L
  // RWB - L (write)
  digitalWrite(RS1, LOW);
  digitalWrite(RS0, LOW);
  digitalWrite(RWB, LOW);
  writeCharToDataBus('A');   // NOT WORKING
  phi2(HIGH);
  phi2(LOW);
  
  // TxD is not transmitting anything.
  // XTLO is actually almost always 5V and not resonating?
  
}

void loop() {
  phi2(HIGH);
  phi2(LOW);
  Serial.println(numIrq);
}



