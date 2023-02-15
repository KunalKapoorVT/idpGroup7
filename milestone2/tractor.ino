#include <SoftwareSerial.h>

const int motorLForward = 11;
const int motorLBackward = 10;
const int motorRForward = 6;
const int motorRBackward = 5;

const int BT_PIN_RXD = 7;
const int BT_PIN_TXD = 8;

int lowCount = 0;
int highCount = 0;

enum {MOVE, STOP} STATE = MOVE;

SoftwareSerial bluetooth (BT_PIN_RXD, BT_PIN_TXD);

void setup() {
  pinMode(motorLForward, OUTPUT);
  pinMode(motorLBackward, OUTPUT);
  pinMode(motorRForward, OUTPUT);
  pinMode(motorRBackward, OUTPUT);
  Serial.begin(9600);
  bluetooth.begin(9600);
}

void loop() {
  
  if(bluetooth.available()){
    String btString = bluetooth.readString();
    if(btString == "Go"){
      Serial.print("Going");
      STATE = MOVE;
      bluetooth.print("Go Recieved");
    }
    else if(btString == "Stop"){
      Serial.print("Stopped");
      STATE = STOP;
      bluetooth.print("Stop Recieved");
    }
  }
  
  bool buttonVal = analogRead(A5) > 750;
  updateButton(buttonVal);

  switch (STATE)
  {
    case MOVE:
      driveForward();
      break;
    case ESTOP:
      stopTractor();
      break;
    default: break;
  }
}

void stopTractor(){
    digitalWrite(motorLForward,LOW);
    digitalWrite(motorLBackward,LOW);
    digitalWrite(motorRForward,LOW);
    digitalWrite(motorRBackward,LOW);
}

void driveForward(){

    //TODO: get gyro data in order to keep straight here

    int motorLSpeed = 250; // 0-255
    int motorRSpeed = 250; // 0-255

    driveTractor(motorLSpeed, motorRSpeed);

}

void driveTractor(int LSpeed, int RSpeed){
  
    //convert wheel speed into forward and backward signal
    byte motorLForwardSpeed = (LSpeed > 0)? LSpeed : 0;
    byte motorRForwardSpeed = (RSpeed > 0)? RSpeed : 0;
    byte motorLBackwardSpeed = (LSpeed < 0)? -LSpeed : 0;
    byte motorRBackwardSpeed = (RSpeed < 0)? -RSpeed : 0;
    
    //set the wheels' speeds
    analogWrite(motorLForward,motorLForwardSpeed);
    analogWrite(motorLBackward,motorLBackwardSpeed);
    analogWrite(motorRForward,motorRForwardSpeed);
    analogWrite(motorRBackward,motorRBackwardSpeed);
}

void updateButton(bool buttonVal){
  if (buttonVal){
    highCount++;
    lowCount = 0;
    if(highCount > 5 && STATE == STOP){
      STATE = MOVE;
    }
  }
  if (!buttonVal){
    lowCount++;
    highCount = 0;
    if(lowCount > 5 && STATE == MOVE){
      STATE = STOP;
    }
  }
}
