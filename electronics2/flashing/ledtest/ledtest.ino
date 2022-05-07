// ledtest.ino - Test if all segment resistors are soldered correctly

void setup() {
  Serial.begin(115200);
  Serial.println("blinky");
  pinMode(6, OUTPUT); // SEG-A
  pinMode(7, OUTPUT); // SEG-B
  pinMode(A0, OUTPUT); // SEG-C
  pinMode(A1, OUTPUT); // SEG-D
  pinMode(A2, OUTPUT); // SEG-E
  pinMode(A3, OUTPUT); // SEG-F
  pinMode(A4, OUTPUT); // SEG-G
  pinMode(A5, OUTPUT); // SEG-P
}

void loop() {
  Serial.println("on");
  digitalWrite(6, HIGH);
  digitalWrite(7, HIGH);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  digitalWrite(A4, HIGH);
  digitalWrite(A5, HIGH);
  delay(50);

  Serial.println("off");
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  digitalWrite(A0, LOW);
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
  delay(50);
}
