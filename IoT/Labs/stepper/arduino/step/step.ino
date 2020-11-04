// Stepper
// Manferdelli

#ifndef byte
typedef uint8_t byte;
#endif
const int measurementDelay= 1000;

int out_ports[4] = {
  11, 10, 9, 8
};

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++)
    pinMode(out_ports[i], OUTPUT);
}

const int FW = 0;
const int BW = 1;

void move_one_step(int dir) {
  static byte out = 1;
  if (dir == FW) {
    if (out != 0x08)
      out = out << 1;
    else
      out = 1;
  } else {
    if (out != 1)
      out = out >> 1;
    else
      out = 0x08;
  }
  for (int i = 0; i < 4; i++) {
    digitalWrite(out_ports[i], (out & (1<<i)) ? HIGH:LOW);
  }
}

void move_steps(int steps, byte ms_delay) {
  if (steps > 0) {
    for (int i = 0; i < steps; i++) {
      move_one_step(FW);
      delay(ms_delay);
    }
  } else {
    for (int i = 0; i < -steps; i++) {
      move_one_step(BW);
      delay(ms_delay);
    }
  }
}

void loop() {
  move_steps(2048, 4); 
  delay(measurementDelay);
  move_steps(-2048, 4); 
  delay(measurementDelay);
}
