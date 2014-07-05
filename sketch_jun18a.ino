// Using Arduino MEGA's pin 3 (PORT E's 4th pin).
// 4th bit from MSB, not LSB.
//
// Reset Pin at Pin53
#define RST (53)
#define RW (51)

int a[16];
int Pin[16] = {A0, A1, A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
int DataPin[8] = {30,31,32,33,34,35,36,37};

// Set Arduino pins corresponds to 6502 Data bus
void set_data_bus(int v) {
  int b;
  int i;

  for (i = 0; i < 8; i++) {
    b = v % 2;
    digitalWrite(DataPin[i], b);
    v >>= 1;
  }
}

void setup() {
  // Pin 3 for clock output
  DDRE = B00010000;
  
  int i;
  for (i = 15; i >= 0; i--) {
    pinMode(Pin[i], INPUT);
  }

  pinMode(RST, OUTPUT);
  pinMode(RW, INPUT);  // Observing R/W pin

  // Data Pins to be OUTPUT to start with
  for (i = 0; i < 8; i++) {
    pinMode(DataPin[i], OUTPUT);
  }
  // OUTPUT $EA
  set_data_bus(0xea);
  
  // Setup Reset pin
  digitalWrite(RST, HIGH);
  
  // See dump at serial terminal
  Serial.begin(57600);
  Serial.println("HELLO");
}

void print_addr(int addr) {
  char buffer[5];
  Serial.print("ADDR (");
  sprintf(buffer, "%04x", addr);
  Serial.print(buffer);
  Serial.print(")");  
}

void print_rw() {
  int rw = digitalRead(RW);
  Serial.print("[");
  if (rw == 1) {
    Serial.print("R");
  } else {
    Serial.print("W");
  }
  Serial.print("]");  
}

int read_address_bus() {
  int i;
  int addr = 0;
  for (i = 15; i >= 0; i--) {
    addr <<= 1;
    addr += digitalRead(Pin[i]);
  }

  return addr;
}

void set_data_bus_for_address(int addr) {
  int v = 0xea;
  if (addr == 0x0000) {
    v = 0x11;
  } else if (addr == 0x1000) {
    v = 0xA9; // LDA #$AA
  } else if (addr == 0x1001) {
    v = 0xAA; // LDA $00
  } else if (addr == 0x1002) {
    v = 0x85; // STA $00
  } else if (addr == 0x1003) {
    v = 0x00;
  } else if (addr == 0x1004) {
    v = 0x4C;
  } else if (addr == 0x1005) {
    v = 0x00;
  } else if (addr == 0x1006) {
    v = 0x10;
  } else if (addr == 0xfffc) {
    v = 0x00;
  } else if (addr == 0xfffd) {
    v = 0x10;
  }
  set_data_bus(v);
}


typedef void (*Callback) (void);

void cb_test() {
  Serial.println("Test Callback!");
}

int clock_counter = 0;
int callback_at = -1;
Callback cb = NULL;


void cb_reset_done() {
  Serial.println("Reset done");
  digitalWrite(RST, HIGH); 
  callback_at = -1;
  cb = NULL;
}

void cb_reset() {
  Serial.println("Reset");
  digitalWrite(RST, LOW); 
  callback_at = clock_counter + 5;
  cb = cb_reset_done;  
}

void loop() {
  int i;

  // set up test callback
  callback_at = 10;
  cb = cb_test;

  while (1) {
    // Read key input from serial
    if (Serial.available()) {
      int key = Serial.read();
      Serial.print("KEY:");
      Serial.println(key);
      if (key == 114) {
        // Reset
        Serial.println("RESET!");
        callback_at = clock_counter;
        cb = cb_reset;
      }
    }
    // check callback
    if (clock_counter == callback_at) {
      cb();
    }
    // Read current bus
    int addr = read_address_bus();
    print_addr(addr);
    Serial.print(" ");
    print_rw();
    set_data_bus_for_address(addr);


    // roughly 2usec clock
    PORTE = B00000000;
    delayMicroseconds(1);
    PORTE = B00010000;
    // 1 second break
    //delayMicroseconds(1000000);
    delay(100);
    
    Serial.print("CLOCK#");
    Serial.print(clock_counter);
    Serial.print(" ");
    Serial.print("ADDRESS:");
    for (i = 15; i >= 0; i--) {
      a[i] = digitalRead(Pin[i]);
      Serial.print(a[i]);      
    }
    Serial.println();
    clock_counter++;    
  }
}

