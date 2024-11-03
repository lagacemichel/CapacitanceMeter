/*
  Compute capacitor value using NE555 timer sketch
  
  We use a NE555 timer integrated circuit in astable configuration to charge
  and discharge a capacitor through known resistor values and measure the time
  it takes for the circuit to go through a full cycle between the NE555
  internal thresholds of 1/3 Vcc and 2/3 Vcc. We use the resulting time to
  compute the capacitance value and return it to the PC connected to the
  Arduino board. It is associated with the Capacitance Meter Using the NE555
  Timer IC blog post on https://lagacemichel.com

  MIT License

  Copyright (c) 2024, Michel Lagace

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <LiquidCrystal.h>
const int NE555OutputPin = 2;
const float timerResistorValueInOhms = 69100.0;
const float dischargeResistorValueInOhms = 121900.0;
const float RCToFullCycleTimeRatio = 1.442295040888963;
const int waitTimeInMilliseconds = 500;
volatile bool cycleComplete = false;
volatile unsigned long previousRisingEdgeTimeInMicroseconds = 0;
volatile unsigned long curentRisingEdgeTimeInMicroseconds = 0;
LiquidCrystal lcd(13, 12, 11, 10, 9, 8);

void DisplayStringsOnLcd(const String& textRow1, const String& textRow2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(textRow1);
  lcd.setCursor(0, 1);
  lcd.print(textRow2);
}

void prepareLcd() {
  lcd.begin(16, 2);
  lcd.clear();
  DisplayStringsOnLcd("Measuring", "");
}

inline float calculateCapacitance(unsigned long timeForFullCycleInMicroseconds) {
  return timeForFullCycleInMicroseconds * RCToFullCycleTimeRatio / (timerResistorValueInOhms + (2.0 * dischargeResistorValueInOhms));
}

void onRisingEdge() {
  previousRisingEdgeTimeInMicroseconds = curentRisingEdgeTimeInMicroseconds;
  curentRisingEdgeTimeInMicroseconds = micros();
  cycleComplete = previousRisingEdgeTimeInMicroseconds > 0;
}

void addCapacitanceToString (float Capacitance, String& capacitanceStr) {
  if (Capacitance >= 1.0) {
    capacitanceStr += String(Capacitance, 2) + " uF";
  }
  else if (Capacitance >= 0.001) {
    capacitanceStr += String(Capacitance * 1000.0, 2) + " nF";
  }
  else {
    capacitanceStr += String(Capacitance * 1000000.0, 2) + " pF";
  }
}

void displayCapacitanceOnLcd(float Capacitance) {
  String capacitorStr = "Cx: ";
  addCapacitanceToString(Capacitance, capacitorStr);
  DisplayStringsOnLcd(capacitorStr,"");
}

void setup() {
  pinMode(NE555OutputPin, INPUT);
  attachInterrupt(digitalPinToInterrupt(NE555OutputPin), onRisingEdge, RISING);
  prepareLcd();
}

void loop() {
  if (cycleComplete) {
    unsigned long timeForFullCycleInMicroseconds = curentRisingEdgeTimeInMicroseconds - previousRisingEdgeTimeInMicroseconds;
    float capacitanceValue = calculateCapacitance(timeForFullCycleInMicroseconds);
    displayCapacitanceOnLcd(capacitanceValue);
    delay(waitTimeInMilliseconds);
    cycleComplete = false;
  }
}