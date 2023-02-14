int lowCount = 0;
int highCount = 0;
bool buttonState = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int test = analogRead(A5);
  if(test > 750){
    highCount++;
    lowCount = 0;
  }
  if(test < 750){
    lowCount++;
    highCount = 0;
  }
  if(lowCount > 10 && buttonState){
    buttonState = false;
    Serial.println(buttonState);
  }
  if(highCount > 10 && !buttonState){
    buttonState = true;
    Serial.println(buttonState);
  }
}
