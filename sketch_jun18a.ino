// Using Arduino MEGA's pin 3 (PORT E's 4th pin).
// 4th bit from MSB, not LSB.
//
// Reset Pin at Pin53
#define RST (53)
#define RW (51)

int a[16];
int Pin[16] = {A0, A1, A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15};
int DataPin[8] = {30,31,32,33,34,35,36,37};

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
  digitalWrite(DataPin[0], LOW);  // D0: Lowest bit?
  digitalWrite(DataPin[1], HIGH); // D1
  digitalWrite(DataPin[2], LOW);  // D2
  digitalWrite(DataPin[3], HIGH); // D3
  digitalWrite(DataPin[4], LOW);  // D4
  digitalWrite(DataPin[5], HIGH); // D5
  digitalWrite(DataPin[6], HIGH); // D6
  digitalWrite(DataPin[7], HIGH); // D7
  
  // Setup Reset pin
  digitalWrite(RST, HIGH);
  
  // See dump at serial terminal
  Serial.begin(57600);
  Serial.println("HELLO");
}

void print_hex() {
  int addr = 0;
  int i;
  for (i = 15; i >= 0; i--) {
    addr <<= 1;
    addr += a[i];
  }
  char buffer[5];
  
  Serial.print(" (");
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
    print_hex();
    print_rw();
    Serial.println();
    clock_counter++;    
  }
}

