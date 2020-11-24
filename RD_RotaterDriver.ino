/* 
 * Rotating DC Motor with rotation feedback by a Potentiometer
 * - Serial inputs are added for testing purposes
 * 
 * Created by Furkan Cetin on 24/11/2020
 */

//Pin Definitions
#define pinReset 4
#define pinDir 9
#define pinPwmL 5
#define pinPwmR 6
#define pinLed 13

#define pinPotans A2
#define pinCurrentSense A3


int ROTATE_PWM_MAX = 55; //255 is ultimate maximum
float ROTATE_ACC = 0.1;

float posNow = 0;
float posTarget = 0;

float speedMotor = 0;

// Simple sign function in math
int sign(float a) {
  if (a > 0) return 1;
  else if (a < 0) return -1;
  else return 0;
}


void setup() {
  Serial.begin(115200);

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
  posNow = analogRead(pinPotans);
  posTarget = posNow;
}

// _speed = [-255,255]
void setMotor(int _speed) { 
  _speed = min(255, max(-255, _speed));

  analogWrite(pinPwmR, abs(_speed));

  if (_speed > 0)
    digitalWrite(pinDir, 1);
  else
    digitalWrite(pinDir, 0);

}


void loop() {
  posNow = analogRead(pinPotans);
  speedMotor += sign(posTarget - posNow) * ROTATE_ACC;

  if (abs(posTarget - posNow) < 1)
    speedMotor = 0;

  speedMotor = min(ROTATE_PWM_MAX, max(-ROTATE_PWM_MAX, speedMotor));
  setMotor(speedMotor);

  Serial.println(speedMotor);

  //For basic testing
  if (Serial.available()) {
    char data = Serial.read();

    if (data >= '0' && data <= '9') {
      if (posTarget < 1000 && posNow < 1000)
        if (posTarget > 100 && posNow > 100)
          posTarget += (data - '5')*3;

    }
  }

}
