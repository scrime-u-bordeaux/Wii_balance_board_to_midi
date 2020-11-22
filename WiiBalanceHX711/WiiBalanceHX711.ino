#include <HX711_ADC.h>
#include <Bounce.h>
Bounce b1 = Bounce(2, 50);
//HX711 constructor:
HX711_ADC lc1(23, 22); // loadCell(hx711 dout, hx711 sck) => pins!!!
HX711_ADC lc2(21, 20);
HX711_ADC lc3(19, 18);
HX711_ADC lc4(17, 16);
float data1, data2, data3, data4;
long t;
bool issue = 0;

void setup() {
  Serial.begin(57600); delay(10);
  Serial.println();
  Serial.println("Starting...");
  pinMode(2, INPUT_PULLUP);
  pinMode(3, OUTPUT);
  digitalWrite(3, 0);
  float calibrationValue; // calibration value
  calibrationValue = 696.0; // uncomment this if you want to set this value in the sketch
  lc1.begin();
  lc2.begin();
  lc3.begin();
  lc4.begin();
  long stabilizingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilizing time
  boolean _tare = true; //set this to false if you don't want tare to be performed in the next step
  byte lc1_rdy = 0;
  byte lc2_rdy = 0;
  byte lc3_rdy = 0;
  byte lc4_rdy = 0;
  while ((lc1_rdy + lc2_rdy + lc3_rdy + lc4_rdy) < 4) { //run startup, stabilization and tare, both modules simultaniously
    if (!lc1_rdy) lc1_rdy = lc1.startMultiple(stabilizingtime, _tare);
    if (!lc2_rdy) lc2_rdy = lc2.startMultiple(stabilizingtime, _tare);
    if (!lc3_rdy) lc3_rdy = lc3.startMultiple(stabilizingtime, _tare);
    if (!lc4_rdy) lc4_rdy = lc4.startMultiple(stabilizingtime, _tare);

  }
  //  lc1.start(stabilizingtime, _tare);
  //  lc2.start(stabilizingtime, _tare);
  //  lc3.start(stabilizingtime, _tare);
  //  lc4.start(stabilizingtime, _tare);
  if (lc1.getTareTimeoutFlag()) {
    Serial.println("Timeout, check lc1");
    issue = 1;
  }
  if (lc2.getTareTimeoutFlag()) {
    Serial.println("Timeout, check lc2");
    issue = 1;
  }
  if (lc3.getTareTimeoutFlag()) {
    Serial.println("Timeout, check lc3");
    issue = 1;
  }
  if (lc4.getTareTimeoutFlag()) {
    Serial.println("Timeout, check lc4");
    issue = 1;
  }
  //  else {
  //    LoadCell.setCalFactor(calibrationValue); // set calibration factor (float)
  //    Serial.println("Startup is complete");
  //  }
  //  while (!LoadCell.update());
  //  Serial.print("Calibration value: ");
  //  Serial.println(LoadCell.getCalFactor());
  //  Serial.print("HX711 measured conversion time ms: ");
  //  Serial.println(LoadCell.getConversionTime());
  //  Serial.print("HX711 measured sampling rate HZ: ");
  //  Serial.println(LoadCell.getSPS());
  //  Serial.print("HX711 measured settlingtime ms: ");
  //  Serial.println(LoadCell.getSettlingTime());
  //  Serial.println("Note that the settling time may increase significantly if you use delay() in your sketch!");
  //  if (LoadCell.getSPS() < 7) {
  //    Serial.println("!!Sampling rate is lower than specification, check MCU>HX711 wiring and pin designations");
  //  }
  //  else if (LoadCell.getSPS() > 100) {
  //    Serial.println("!!Sampling rate is higher than specification, check MCU>HX711 wiring and pin designations");
  //  }
  lc1.setCalFactor(500.0); // user set calibration value (float)
  lc2.setCalFactor(500.0);
  lc3.setCalFactor(500.0);
  lc4.setCalFactor(500.0);
  lc1.setSamplesInUse(1);
  lc2.setSamplesInUse(1);
  lc3.setSamplesInUse(1);
  lc4.setSamplesInUse(1);
  if (!issue)  digitalWrite(3, 1);
}

void loop() {
  b1.update();
  if ( b1.fallingEdge() ) { //////////////////// setting ++
    lc1.tareNoDelay();
    lc2.tareNoDelay();
    lc3.tareNoDelay();
    lc4.tareNoDelay();
    digitalWrite(3, 0);
    delay(1000);
    digitalWrite(3, 1);
  }
  static boolean newDataReady = 0;
  const int serialPrintInterval = 50; //increase value to slow down serial print activity
  if (millis() > t + serialPrintInterval) {
    int Xglobal = 4096+(data1+data4)-(data2+data3);
    int Yglobal =4096+(data1 + data2) - (data3 + data4);
    int total = (data1+data2+data3+data4);
    Serial.print(Xglobal); Serial.print("\t");
    Serial.print(Yglobal); Serial.print("\t");
    Serial.print(total); Serial.print("\t");
    usbMIDI.sendPitchBend(Xglobal, 1);
    usbMIDI.sendPitchBend(Yglobal, 2);
    usbMIDI.sendPitchBend(total, 3);
//    Serial.print(data1); Serial.print("\t");
//    Serial.print(data2); Serial.print("\t");
//    Serial.print(data3); Serial.print("\t");
//    Serial.print(data4);
Serial.println();
    t = millis();
  }
  // check for new data/start next conversion:
  if (lc1.update()) { // newDataReady = true;
    data1 = -1 * lc1.getData();
    // Serial.print("Load_cell output val: ");
    //Serial.println(i);
  }
  if (lc2.update()) { // newDataReady = true;
    data2 = -1 * lc2.getData();
  }
  if (lc3.update()) { // newDataReady = true;
    data3 = -1 * lc3.getData();
  }
  if (lc4.update()) { // newDataReady = true;
    data4 = -1 * lc4.getData();
  }
  //  // get smoothed value from the dataset:
  //  if (newDataReady) {
  //    if (millis() > t + serialPrintInterval) {
  //      float i = LoadCell.getData();
  //      Serial.print("Load_cell output val: ");
  //      Serial.println(i);
  //      newDataReady = 0;
  //      t = millis();
  //    }
  //  }

  // receive command from serial terminal, send 't' to initiate tare operation:
  if (Serial.available() > 0) {
    float i;
    char inByte = Serial.read();
    if (inByte == 't') {
      lc1.tareNoDelay();
      lc2.tareNoDelay();
      lc3.tareNoDelay();
      lc4.tareNoDelay();
    }
  }

  // check if last tare operation is complete:
  if (lc1.getTareStatus() == true) {
    Serial.println("Tare complete");
  }

}
