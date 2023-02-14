const int motorLForward = 10;
const int motorLBackward = 11;
const int motorRForward = 6;
const int motorRBackward = 5;


void setup() {
  pinMode(motorLForward, OUTPUT);
  pinMode(motorLBackward, OUTPUT);
  pinMode(motorRForward, OUTPUT);
  pinMode(motorRBackward, OUTPUT);
}

void loop() {
    digitalWrite(motorLForward,HIGH);
    digitalWrite(motorLBackward,LOW);
    digitalWrite(motorRForward,HIGH);
    digitalWrite(motorRBackward,LOW);
    delay(500);
    digitalWrite(motorLForward,LOW);
    digitalWrite(motorLBackward,LOW);
    digitalWrite(motorRForward,LOW);
    digitalWrite(motorRBackward,LOW);
    delay(500);
}