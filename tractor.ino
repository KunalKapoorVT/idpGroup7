#include <SoftwareSerial.h>
#include <Wire.h>
#include <MPU6050_light.h>

const double pi = 3.14159265358979323846264;

//Used for gyro
const double minAngleFactor = 10000;

//motor pins
const int motorLForward = 11;
const int motorLBackward = 10;
const int motorRForward = 5;
const int motorRBackward = 6;

//push button
const int buttonIn = A3;

//Bluetooth
const int BT_PIN_RXD = 7;
const int BT_PIN_TXD = 8;

//IR photodiode
const int IR_PIN = A0;
int val = 0;
int limit = 1010;
bool turning = false;
int turnDelay = 0;
int lastUpdateMS = 0;

//gyro
unsigned long gyroTimer = 0;
double posX = 0.0;
double posY = 0.0;
double velX = 0.0;
double velY = 0.0;
double gyroTotalAccX;
double gyroTotalAccY;
unsigned long sendDataTimer = 0;

double lineAngleDeg = 0;
double lineX = 0;
double tractorAngle = 0;
double tractorX = 0;

//push button
int lowCount = 0;
int highCount = 0;

//states
bool buttonState = false;
bool buttonControl = false;
bool moving = false;
String turn = "Forward";
int counter = 0;

bool obstacle = false;

SoftwareSerial bluetooth(BT_PIN_RXD, BT_PIN_TXD);

MPU6050 gyroscope(Wire);

void setup() {
  pinMode(motorLForward, OUTPUT);
  pinMode(motorLBackward, OUTPUT);
  pinMode(motorRForward, OUTPUT);
  pinMode(motorRBackward, OUTPUT);

  //pinMode(trigPin, OUTPUT);
  //pinMode(echoPin, INPUT);

  //  Serial.begin(9600);
  Wire.begin();
  bluetooth.begin(9600);

  byte gyroStatus = gyroscope.begin();
  bluetooth.print(F("MPU6050 status: "));
  bluetooth.println(gyroStatus);

  bluetooth.println(F("Calculating offsets, do not move MPU6050"));
  delay(100);
  gyroscope.calcOffsets(true, true);  // gyro and accelero
  bluetooth.println("Done!\n");
}

void loop() {

  gyroscope.update();

  updateGyroPos();

  sendLocationData();

  ultraEStop();

  IRtest();

  if (turning)
    turnTimer();

  if (bluetooth.available()) {
    String btString = bluetooth.readString();
    if (btString == "Go") {
      moving = true;
      bluetooth.println("Go Recieved");
    } else if (btString == "Stop") {
      moving = false;
      bluetooth.println("Stop Recieved");
    } else if (btString == "Right") {
      lineAngleDeg -= 90;
      bluetooth.println("R");
      bluetooth.println(lineAngleDeg);
    } else if (btString == "Left") {
      lineAngleDeg += 90;
      bluetooth.println("L");
      bluetooth.println(lineAngleDeg);
    }
  }

  bool buttonVal = analogRead(buttonIn) > 512;
  //bluetooth.println(analogRead(buttonIn));
  updateButton(buttonVal);

  //bluetooth.println(moving);
  if (moving) {
    driveForward(false);
  } else {
    stopTractor();
  }
}

void stopTractor() {
  digitalWrite(motorLForward, LOW);
  digitalWrite(motorLBackward, LOW);
  digitalWrite(motorRForward, LOW);
  digitalWrite(motorRBackward, LOW);
}

void sendLocationData(){
  
  if (millis() - sendDataTimer > 5000) {
    
    bluetooth.print("ANGLEZ=");
    bluetooth.println(gyroscope.getAngleZ());

    bluetooth.print("PosX=");
    bluetooth.println(posX);
    bluetooth.print("PosY=");
    bluetooth.println(posY);
    bluetooth.print("VelX=");
    bluetooth.println(velX);
    bluetooth.print("VelY=");
    bluetooth.println(velY);

    sendDataTimer = millis();
  }
  
}

void driveForward(bool considerHorizontal) {

  double idealAngleDeg = lineAngleDeg;

  if (considerHorizontal) {

    //the distance from the track, xDist, how far the tractor is to the right of its straight line
    double xDist = tractorX - lineX;

    //minAngleDeg: the ideal angle to hold, from relative to the angle of the line
    //when the tractor is on the right, this is a positive (ccw) angle
    //when it is on the left, xDist is negative, so the angle is clockwise
    idealAngleDeg = xDist / minAngleFactor + lineAngleDeg;
  }

  //the furthest you should go, relative to the line, is perpendicular
  int maxAngleDeg = 90 + lineAngleDeg;
  int minAngleDeg = -90 + lineAngleDeg;

  //bound the ideal angle between the max and min
  idealAngleDeg = max(min(idealAngleDeg, maxAngleDeg), minAngleDeg);

  //take the ideal angle and the current angle and create relative motor speeds
  double angleDiff = tractorAngle - idealAngleDeg;


  int motorLSpeed = 0;  // -256 to 255
  int motorRSpeed = 0;  // -256 to 255

  //at angleDiff>=90: L = 255, R = -255
  //at angleDiff=0: L = 255, R = 255
  //at angleDiff<=-90: L = -255, R = 255


  motorLSpeed = 255 * (1 - angleDiff / 45);
  motorLSpeed = max(min(motorLSpeed, 255), -255);


  motorRSpeed = 255 * (1 + angleDiff / 45);
  motorRSpeed = max(min(motorRSpeed, 255), -255);

  //    Serial.print("Motor L Speed: ");Serial.println(motorLSpeed);
  //    Serial.print("Motor R Speed: ");Serial.println(motorRSpeed);

  driveTractor(motorLSpeed, motorRSpeed);
}

void updateGyroPos() {
  int dt = millis() - gyroTimer;
  double Zangle = gyroscope.getAngleZ();
  tractorAngle = Zangle;
  velX += (gyroscope.getAccX() * cos(Zangle * 2 * pi / 360) + gyroscope.getAccY() * sin(Zangle * 2 * pi / 360)) * dt;
  velY += (gyroscope.getAccY() * cos(Zangle * 2 * pi / 360) - gyroscope.getAccX() * sin(Zangle * 2 * pi / 360)) * dt;
  posX += velX * dt;
  posY += velY * dt;
  gyroTimer = millis();
}

void driveTractor(int LSpeed, int RSpeed) {

  //convert wheel speed into forward and backward signal
  byte motorLForwardSpeed = (LSpeed > 0) ? LSpeed : 0;
  byte motorRForwardSpeed = (RSpeed > 0) ? RSpeed : 0;
  byte motorLBackwardSpeed = (LSpeed < 0) ? -LSpeed : 0;
  byte motorRBackwardSpeed = (RSpeed < 0) ? -RSpeed : 0;

  //set the wheels' speeds
  analogWrite(motorLForward, motorLForwardSpeed);
  analogWrite(motorLBackward, motorLBackwardSpeed);
  analogWrite(motorRForward, motorRForwardSpeed);
  analogWrite(motorRBackward, motorRBackwardSpeed);
}

void onButtonStateHigh() {
  bluetooth.println("Button up");
  buttonControl = !buttonControl;
  moving = buttonControl;
  bluetooth.print("MOVING=");
  bluetooth.print(moving);
  /*if (buttonControl)
    {
    //bluetooth.println(("Restarting"));
    bluetooth.println(F("Calculating offsets, do not move MPU6050"));
    delay(100);
    gyroscope.calcOffsets(true,true); // gyro and accelero
    bluetooth.println("Done!\n");
    }*/
}

void onButtonStateLow() {
  bluetooth.println("Button down");
  moving = buttonControl;
}

void updateButton(bool buttonVal) {
  if (buttonVal) {
    highCount++;
    lowCount = 0;
    if (highCount > 5 && !buttonState) {
      buttonState = true;
      onButtonStateHigh();
    }
  }
  if (!buttonVal) {
    lowCount++;
    highCount = 0;
    if (lowCount > 5 && buttonState) {
      buttonState = false;
      onButtonStateLow();
    }
  }
}

void IRtest() {
  val = analogRead(IR_PIN);  //photodiode reading
  
  if (val <= limit && turnDelay < 0)  // NO OBSTICLE
  {
    //bluetooth.println("Read Floor");
    //delay(20);
    turning = false;
  } else if (val > limit && !turning)  // OBSTICLE DETECTED
  {
    bluetooth.println("Read Tape");
    //delay(20);
    //makeNextTurn();
    //turnDelay = 200 for left 300 for right
    if (counter <= 1) {
      turnDelay = 200;
    } else if (counter <= 3) {
      turnDelay = 300;
    } else if (counter <= 5) {
      turnDelay = 200;
    }

    lastUpdateMS = millis();
    turning = true;
  }
}

void makeNextTurn() {
  if (counter <= 1) {
    lineAngleDeg += 90;
  } else if (counter <= 3) {
    lineAngleDeg -= 90;
    bluetooth.println("R");
  } else if (counter <= 5) {
    lineAngleDeg += 90;
    bluetooth.println("L");
  }
  bluetooth.print("GOALANGLE=");
  bluetooth.println(lineAngleDeg);
  counter++;
}

void ultraEStop() {
  if (digitalRead(3) == 0)  {
    if (moving) {
      moving = false;
      obstacle = true;
      bluetooth.println("Obstacle Found");
    }
  }
  else if (obstacle)
  {
    moving = true;
    obstacle = false;
      bluetooth.println("Obstacle Cleared");
  }
}

void turnTimer() {
  if (turnDelay > 0) {
    turnDelay -= millis() - lastUpdateMS;
    lastUpdateMS = millis();
    if (turnDelay <= 0)
      makeNextTurn();
  }
}
