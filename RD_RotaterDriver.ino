/*
   Rotating DC Motor with rotation feedback by a Potentiometer
   - Serial inputs are added for testing purposes

   Created by Furkan Cetin on 24/11/2020
   Modified on 27/11/2020:
    - Renaming with variables
    - Baudrate changed to  9600
    - New Functions are added
*/

//Pin Definitions
#define pinReset 4
#define pinDir 9
#define pinPwmL 5
#define pinPwmR 6
#define pinLed 13

#define pinPotans A2
#define pinCurrentSense A3

#define BYTE_SET_ANGLE_TARGET_ABS     'a'
#define BYTE_SET_ANGLE_TARGET_REL     'r'
#define BYTE_SET_ANGLE_RANGE          'm'
#define BYTE_SET_ANGLE_CENTER_TO_NOW  'c'
#define BYTE_SET_PWM_MAX              'p'

float CONST_ANGLE_TO_BITS = 1024.0 / 300.0;

int PWM_MAX = 190;

int ANGLE_RANGE = 65 *  CONST_ANGLE_TO_BITS;
float ANGLE_ACC = 255 * 0.5; // final acc = ANGLE_ACC/255
float ANGLE_CENTER = 512;

float angleTarget = 0;
float angleNow = 0;

float pwmMotor = 0;

// Simple sign function in math
int sign(float a) {
  if (a > 0) return 1;
  else if (a < 0) return -1;
  else return 0;
}


void setup() {
  Serial.begin(9600);

  pinMode(pinReset, OUTPUT);
  digitalWrite(pinReset, 1);

  pinMode(pinDir, OUTPUT);
  pinMode(pinPwmL, OUTPUT);
  pinMode(pinPwmR, OUTPUT);
  pinMode(pinLed, OUTPUT);

  pinMode(pinPotans, INPUT);
  pinMode(pinCurrentSense, INPUT);

  digitalWrite(pinPwmL, 1);

  delay(200);

  //angleNow = analogRead(pinPotans);
  angleTarget = ANGLE_CENTER;
}

void loop() {
  //CALCULATION LAYER
  angleNow = analogRead(pinPotans);
  pwmMotor += sign(angleTarget - angleNow) * 5.0;

  if (abs(angleTarget - angleNow) < 4)
  {
    pwmMotor = 0;
    angleTarget = angleNow;
  }

  //ACTUATION LAYER
  if (pwmMotor > 0) {
    pwmMotor = min(pwmMotor, PWM_MAX);
    digitalWrite(pinDir, 1);
  }
  else {
    pwmMotor = max(pwmMotor, -PWM_MAX);
    digitalWrite(pinDir, 0);
  }

  analogWrite(pinPwmR, abs(pwmMotor));

  //Serial.print(angleNow);
  //Serial.print("|");
  //Serial.println(pwmMotor);

  Serial.write(255);
  Serial.write(int((ANGLE_CENTER - angleNow) / CONST_ANGLE_TO_BITS) );
  Serial.write(int(pwmMotor));

  //COMMUNICATION LAYER
  while (Serial.available() >= 3) {
    if (Serial.read() == 255) {
      char msgType = Serial.read();
      int  msgData = Serial.read();

      if (msgType == BYTE_SET_ANGLE_TARGET_ABS) {
        setAngleTargetAbs(msgData - 127);
      }
      else if (msgType == BYTE_SET_ANGLE_TARGET_REL) {
        setAngleTargetRel(msgData - 127);
      }
      else if (msgType == BYTE_SET_ANGLE_RANGE) {
        setAngleRange(msgData);
      }
      else if (msgType == BYTE_SET_ANGLE_CENTER_TO_NOW) {
        if (msgData == BYTE_SET_ANGLE_CENTER_TO_NOW)
          setAngleCenter();
      }
      else if (msgType == BYTE_SET_PWM_MAX) {
        setPwmMax(msgData);
      }
    }
  }
}


void setAngleTargetAbs(int _angle) { //_angle: [-127,+127]
  _angle = min(90, max(_angle, -90));

  float angleTargetPre = angleTarget;
  angleTarget = ANGLE_CENTER + _angle * CONST_ANGLE_TO_BITS ;

  if (sign(angleTargetPre - angleNow) != sign(angleTarget - angleNow))
  {
    pwmMotor = 0;
  }
}

void setAngleTargetRel(int _angle) { //_angle: [-127,+127]
  angleTarget += _angle * CONST_ANGLE_TO_BITS;
  angleTarget = min(ANGLE_CENTER + ANGLE_RANGE, max(angleTarget, ANGLE_CENTER - ANGLE_RANGE));
}

void setAngleRange(int _angle) {      //_angle: [-127,+127]
  if (pwmMotor == 0) {
    _angle = min(65, max(_angle, 5));
    ANGLE_RANGE = _angle * CONST_ANGLE_TO_BITS;
  }
}

void setAngleCenter() {
  if (pwmMotor == 0)
    ANGLE_CENTER = angleNow;
}

void setPwmMax(int _pwm) {
  _pwm = min(255, max(_pwm, 0));
  if (pwmMotor < _pwm)
    PWM_MAX = _pwm;
}
