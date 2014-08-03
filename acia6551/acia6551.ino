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

#define WRITE false
#define READ  true

int dataBusPins[] = {D0, D1, D2, D3, D4, D5, D6, D7};
int numIrq = 0;

void irqHandler() {
  numIrq++;
}

void phi2(int state) {
  digitalWrite(PHI2, state);
  delayMicroseconds(1);
}

// Read data bus pin states and store in context.
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
    digitalWrite(RWB, HIGH);
    for (int i = 0; i < 8; i++) {
      pinMode(dataBusPins[i], INPUT);
    }
  } else {
    digitalWrite(RWB, LOW);
    for (int i = 0; i < 8; i++) {
      pinMode(dataBusPins[i], OUTPUT);
    }
  }
}

void writeCharToDataBus(char chr) {
  Serial.print("Write char:");
  for (int i = 0; i < 8; i++) {
    int bit = chr & (1 << i);
    if (bit > 0) {
      Serial.print(1);
    } else {
      Serial.print(0);
    }
    digitalWrite(dataBusPins[i], bit);
  }
  Serial.println("");
}

void readStatusRegister() {
  setDataBusReadWrite(READ);
  // wait 200ns after set to Read
  digitalWrite(RS1, LOW);
  digitalWrite(RS0, HIGH);
  delay(1); // 1ms
  readDataBus(&ctx);
}

void dumpStatusRegister() {
  // Read status register
  // RS1 - L
  // RS0 - H
  // RWB - H
  readStatusRegister();
  Serial.print("STATUS:");
  dumpDataBus(&ctx);
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
  
  setDataBusReadWrite(READ);
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
  
  // See Status Register immediately after a reset.
  dumpStatusRegister();
  
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
  setDataBusReadWrite(WRITE);
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
  setDataBusReadWrite(READ);
  
  phi2(HIGH);
  // READ DATA BUS
  readDataBus(&ctx);
  phi2(LOW);
  Serial.print("CONTROL:");
  dumpDataBus(&ctx);

  //attachInterrupt(0, irqHandler, FALLING);

  Serial.print("after setting control register:");
  dumpStatusRegister();
  
  // Write Command Register
  // RS1 - H
  // RS0 - L
  digitalWrite(RS1, HIGH);
  digitalWrite(RS0, LOW);
  setDataBusReadWrite(WRITE);
  digitalWrite(D7, LOW);
  digitalWrite(D6, LOW);
  digitalWrite(D5, LOW);  // Parity mode disabled.
  digitalWrite(D4, LOW);  // Receiver normal mode
  digitalWrite(D3, HIGH);  // /RTS = low, transmit interrupt disabled
  digitalWrite(D2, LOW);
  digitalWrite(D1, HIGH);  // IRQ disabled (receiver)
  digitalWrite(D0, HIGH);  // Data terminal ready (/DTR low)
  phi2(HIGH);
  phi2(LOW);
  Serial.println("Wrote Command Register");
  
  // Write Transmit Data Register
  // RS1 - L
  // RS0 - L
  // RWB - L (write)
  digitalWrite(RS1, LOW);
  digitalWrite(RS0, LOW);
  setDataBusReadWrite(WRITE);
  writeCharToDataBus('*');
  phi2(HIGH);
  phi2(LOW);
  
  // TxD is not transmitting anything.
  dumpStatusRegister();
  for (int i = 0; i < 500; i++) {
    readStatusRegister();
    if (ctx.dataBus[4] == 1) {
      // Register full
      Serial.println("==================FULL");
    }
    digitalWrite(RS1, LOW);
    digitalWrite(RS0, LOW);
    setDataBusReadWrite(WRITE);
    writeCharToDataBus('A');
    phi2(HIGH);
    phi2(LOW);
    dumpStatusRegister();
  }
}

void loop() {
    readStatusRegister();
    if (ctx.dataBus[4] == 1) {
      // Register full
      Serial.println("==================FULL");
    }
    digitalWrite(RS1, LOW);
    digitalWrite(RS0, LOW);
    setDataBusReadWrite(WRITE);
    writeCharToDataBus('A');
    phi2(HIGH);
    phi2(LOW);
    dumpStatusRegister();
}



