int pwmPin = 9;
int in1 = 2;
int in2 = 3;

void setup() {
  pinMode(pwmPin, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  digitalWrite(in1, HIGH);  // Motor direction
  digitalWrite(in2, LOW);
}

void loop() {
  analogWrite(pwmPin, 200);   // 0–255 speed
}
