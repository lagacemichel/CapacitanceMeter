
/*
  Compute capacitor value using NE555 timer sketch
  
  We use an NE555 timer chip in monostable configuration to charge a capacitor
  through a known resistor value and measure the time it takes for the capacitor's
  charge to reach the NE555 internal threshold of 2/3 Vcc. We use the time to
  compute the capacitance value and return it to the PC connected to the Arduino
  board. It is associated with the Capacitance Meter Using the NE555 Timer blog
  post on https://lagacemichel.com

  MIT License

  Copyright (c) 2023, Michel Lagace

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

const int NE555ResetPin = 4;
const int NE555OutputPin = 3;
const int NE555TriggerPin = 2;
float timerResistorValue = 100900.0;
float RCToThresholdTimeRatio = 1.10363832;
int codeExecutionTimeOverhead = 16;
const int capacitorDischargeWaitTime = 1000;

void resetCapacitanceMeter() {
  Serial.println("Reseting Capacitance Meter");
  pinMode(NE555ResetPin, OUTPUT);
  digitalWrite(NE555ResetPin, LOW);
  digitalWrite(NE555ResetPin, HIGH);
  delay(capacitorDischargeWaitTime);
}

void triggerTimerAndWaitForThreshold() {
  digitalWrite(NE555TriggerPin, LOW);
  digitalWrite(NE555TriggerPin, HIGH);
  while(digitalRead(NE555OutputPin) == HIGH) {
  }
}

void returnCapacitanceToPc(float capacitance) {
  Serial.print("Capacitance: ");
  Serial.print(capacitance, 4);
  Serial.println(" uF");
}

void setup() {
  Serial.begin(9600);
  pinMode(NE555OutputPin, INPUT);
  pinMode(NE555TriggerPin, OUTPUT);
  digitalWrite(NE555TriggerPin, HIGH);
  resetCapacitanceMeter();
  Serial.println("Measuring Capacitance");
}

void loop() {
  long int initialTimerValue = micros();
  triggerTimerAndWaitForThreshold();
  long int timeToReachThreshold = micros() - initialTimerValue - codeExecutionTimeOverhead;
  float capacitance = timeToReachThreshold/RCToThresholdTimeRatio/timerResistorValue;
  returnCapacitanceToPc(capacitance);
  delay(capacitorDischargeWaitTime);
}
