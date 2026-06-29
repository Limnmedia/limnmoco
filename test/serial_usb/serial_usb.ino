// SPDX-License-Identifier: GPL-3.0-or-later


void setup() {
  SerialUSB.begin(115200);
}

void loop() {
  SerialUSB.println("test");
  delay(1000);
}

