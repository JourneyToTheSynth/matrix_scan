// Arduino Nano universal keybed mapper for 14-pin passive matrix with diodes
// Fill this with the EXACT 14 Nano pins you wired, in any order.
int pins[14] = {
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, A0, A1, A2
};

const int N = sizeof(pins)/sizeof(pins[0]);

void setup() {
  Serial.begin(115200);
  delay(300);
  Serial.println("Keybed auto-discover tool ready.");
  Serial.println("Amount N: " + N);
  Serial.println("Press a key and watch for 'DRV x -> SENSE y'.");
}

void allHighZWithPullupsExcept(int drvIdx) {
  for (int i = 0; i < N; i++) {
    if (i == drvIdx) continue;
    pinMode(pins[i], INPUT_PULLUP);  // sense lines
  }
}

void driveOneLow(int drvIdx) {
  pinMode(pins[drvIdx], OUTPUT);
  digitalWrite(pins[drvIdx], LOW);   // sink current
}

void loop() {
  for (int d = 0; d < N; d++) {
    // Configure one driver low, others input pullup
    driveOneLow(d);
    // Slight settle time helps cheap keybeds/scanners
    delayMicroseconds(100);
    allHighZWithPullupsExcept(d);

    // Scan all the sensing pins
    for (int s = 0; s < N; s++) {
      if (s == d) continue;
      int v = digitalRead(pins[s]);
      if (v == LOW) {
        // LOW here means current flowed from the sense line's pull-up
        // through a pressed switch+diode into the driven-low line.
        Serial.print("DRV ");
        Serial.print(pins[d]);
        Serial.print(" -> SENSE ");
        Serial.print(pins[s]);
        Serial.println("  (key CLOSED)");
      }
    }

    // Return driver to Hi-Z so we don't fight the next pass
    pinMode(pins[d], INPUT);
  }

  delay(20); // ~50 Hz overall scan
}
