#include <SoftwareSerial.h>
#include <Wire.h>
#include <MPU6050_light.h>

const double pi = 3.14159265358979323846264;

const int motorLForward = 11;
const int motorLBackward = 10;
const int motorRForward = 6;
const int motorRBackward = 5;

const int buttonIn = A3;

const int BT_PIN_RXD = 7;
const int BT_PIN_TXD = 8;

unsigned long gyroTimer = 0;
double posX = 0.0;
double posY = 0.0;
double velX = 0.0;
double velY = 0.0;
double gyroTotalAccX;
double gyroTotalAccY;
unsigned long gyroPrintTimer = 0;

double lineAngleDeg = 0;
double lineX = 0;

int lowCount = 0;
int highCount = 0;

bool buttonState = true;
bool buttonControl = false;

SoftwareSerial bluetooth (BT_PIN_RXD, BT_PIN_TXD);

MPU6050 gyroscope(Wire);

void setup() {
  pinMode(motorLForward, OUTPUT);
  pinMode(motorLBackward, OUTPUT);
  pinMode(motorRForward, OUTPUT);
  pinMode(motorRBackward, OUTPUT);
  
  Serial.begin(9600);
  Wire.begin();
  bluetooth.begin(9600);
  
  byte gyroStatus = gyroscope.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(gyroStatus);

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  gyroscope.calcOffsets(true,true); // gyro and accelero
  Serial.println("Done!\n");
}

bool moving;

void loop() {

  gyroscope.update();

  updateGyroPos();

  gyroTest();
  
  if(bluetooth.available()){
    String btString = bluetooth.readString();
    if(btString == "Go"){
      Serial.print("Going");
      moving = true;
      bluetooth.print("Go Recieved");
    }
    if(btString == "Stop"){
      Serial.print("Stopped");
      moving = false;
      bluetooth.print("Stop Recieved");
    }
  }
  
  bool buttonVal = analogRead(buttonIn) > 750;
  updateButton(buttonVal);

  
  if (moving && buttonControl){
    driveForward();
  }
  else{
    stopTractor();
  }
}

void stopTractor(){
    digitalWrite(motorLForward,LOW);
    digitalWrite(motorLBackward,LOW);
    digitalWrite(motorRForward,LOW);
    digitalWrite(motorRBackward,LOW);
}

void driveForward(bool considerHorizontal){

    double idealAngleDeg;

    if(considerHorizontal){
     
      //the distance from the track, xDist, how far the tractor is to the right of its straight line
      double xDist = tractorX - lineX;
  
      //minAngleDeg: the ideal angle to hold, from relative to the angle of the line
      //when the tractor is on the right, this is a positive (ccw) angle
      //when it is on the left, xDist is negative, so the angle is clockwise
      idealAngleDeg = xDist/minAngleFactor + lineAngleDeg;

    }
    else{
      idealAngleDeg = lineAngleDeg;
    }
    
    //the furthest you should go, relative to the line, is perpendicular
    int maxAngleDeg = 90 + lineAngleDeg;
    int minAngleDeg = -90 + lineAngleDeg;
  
    //bound the ideal angle between the max and min
    idealAngleDeg = max(min(idealAngledeg,maxAngleDeg),minAngleDeg);

    //take the ideal angle and the current angle and create relative motor speeds
    double angleDiff = tractorAngle - idealAngleDeg;

    
    int motorLSpeed = 0; // -256 to 255
    int motorRSpeed = 0; // -256 to 255

    //at angleDiff>=90: L = 255, R = -255
    //at angleDiff=0: L = 255, R = 255
    //at angleDiff<=-90: L = -255, R = 255
    
    
    motorRSpeed = 255*(1-angleDiff/45);
    motorRSpeed = max(min(motorRSpeed,255),-255);

    
    motorLSpeed = 255*(1+angleDiff/45);
    motorLSpeed = max(min(motorLSpeed,255),-255);

    driveTractor(motorLSpeed, motorRSpeed);

}

void updateGyroPos(){
  int dt = millis() - gyroTimer;
  double Zangle = gyroscope.getAngleZ();
  velX += gyroscope.getAccX() * sin(Zangle*2*pi/360) * dt;
  velY += gyroscope.getAccY() * cos(Zangle*2*pi/360) * dt;
  posX += velX * dt;
  posY += velY * dt;
  gyroTimer = millis(); 
}

void gyroTest(){
  
    if(millis() - gyroPrintTimer > 5000){ 

      
      Serial.print(F("Displacement     X(cm): "));
      Serial.print(posX/(9.8*10000));
      Serial.print("\tY(cm): ");
      Serial.println(posY/(9.8*10000));
      
      Serial.print(F("Velocity     X(cm/s): "));
      Serial.print(velX/0.98);
      Serial.print("\tY(cm/s): ");
      Serial.println(velY/0.98);
      
      Serial.print(F("Aceleration     X: "));
      Serial.print(gyroscope.getAccX());
      Serial.print("\tY: ");
      Serial.println(gyroscope.getAccY());
      
      
      Serial.print(F("ANGLE     X: "));
      Serial.print(gyroscope.getAngleX());
      Serial.print("\tY: ");
      Serial.print(gyroscope.getAngleY());
      Serial.print("\tZ: ");
      Serial.println(gyroscope.getAngleZ());
  
      gyroPrintTimer = millis(); 
    }
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

void onButtonStateHigh(){
  buttonControl = !buttonControl;
}

void onButtonStateLow(){
  
}

void updateButton(bool buttonVal){
  if (buttonVal){
    highCount++;
    lowCount = 0;
    if(highCount > 5 && !buttonState){
      buttonState = true;
      onButtonStateHigh();
    }
  }
  if (!buttonVal){
    lowCount++;
    highCount = 0;
    if(lowCount > 5 && buttonState){
      buttonState = false;
      onButtonStateLow();
    }
  }
}
