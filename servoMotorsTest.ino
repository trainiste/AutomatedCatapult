#include <Servo.h>

Servo gancio;
Servo inclinazione;
Servo molla;

void setup() {
  Serial.begin(9600);
  gancio.attach(2);
  inclinazione.attach(4);
  molla.attach(6);
  delay(1000);
}

void loop() {
gancio.write(100);
molla.write(100);
inclinazione.write(100);
delay(2000);
gancio.write(0);
molla.write(0);
inclinazione.write(0);
}
