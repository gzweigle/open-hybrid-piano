// Get an analog value from the Teensy ADC.
// Convert to a number.
// Display the result.

void setup() {
  pinMode(16, INPUT);  // Connect analog input to pin A2.
  Serial.begin(9600);  // Start the serial monitor so can see values.
}

void loop() {
  float adc_value;
  adc_value = (float) analogRead(16);  // Convert an analog value to a number.
  Serial.println(adc_value);

  // Delay 10 milliseconds.
  // Delay can be shorter but if make too small,
  // it is possible to overflow the Serial.println().
  delayMicroseconds(10000);
}
