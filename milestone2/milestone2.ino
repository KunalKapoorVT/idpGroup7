const int motorLForward = 10;
const int motorLBackward = 11;
const int motorRForward = 6;
const int motorRBackward = 5;

bool buttonState;
int highCount;
int lowCount;
int buttonSensorValue;

enum STATE {GOOD, ESTOP};

void setup() {
  pinMode(motorLForward, OUTPUT);
  pinMode(motorLBackward, OUTPUT);
  pinMode(motorRForward, OUTPUT);
  pinMode(motorRBackward, OUTPUT);
  Serial.begin(9600);
  Serial.setTimeout(500);
  buttonState = false;
  highCount = 0;
  lowCount = 0;
  STATE s = GOOD;
}

void loop() {
  String cmd = Serial.readString();
  
  button();

  if (lowCount > 10 && buttonState)
  {
    buttonState = GOOD;
    Serial.println(buttonState);
  }
  else if (highCount > 10 && !buttonState)
  {
    buttonState = ESTOP;
    Serial.println(buttonState);
  }

  switch (buttonState)
  {
    case GOOD:
      if (cmd == "Go")
      {
        startMotor();
        delay(500);
        stopMotor();
      }
      break;
    
    case ESTOP:
      break;

    default: break;
  }
}

void startMotor()
{
  digitalWrite(motorLForward,LOW);
  digitalWrite(motorLBackward,HIGH); 
  digitalWrite(motorRForward,HIGH);
  digitalWrite(motorRBackward,LOW);
}

void stopMotor()
{
   digitalWrite(motorLForward,LOW);
   digitalWrite(motorLBackward,LOW);
   digitalWrite(motorRForward,LOW);      
   digitalWrite(motorRBackward,LOW);
}

void button()
{
  buttonSensorValue = analogRead(A0); // read the input on A0
  //float voltage = sensorValue * (5.0 / 1023.0); // convert the value to voltage
  //Serial.println(voltage); // print the voltage to the serial monitor
  if(buttonSensorValue > 750){
    highCount++;
    lowCount = 0;
  }
  else if(buttonSensorValue < 750){
    lowCount++;
    highCount = 0;
  }
}