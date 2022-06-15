#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Servo.h>
#include <SD.h>

Servo a;
Servo b;
Servo c;


const int MPU_ADDR = 0x68; 

int16_t accelerometer_x, accelerometer_y, accelerometer_z;
int16_t gyro_x, gyro_y, gyro_z; 
int16_t temperature; 

char tmp_str[7]; 

char* convert_int16_to_str(int16_t i) {
  sprintf(tmp_str, "%6d", i);
  return tmp_str;
}


const int t = 10;
float accelsX[t];
float accelsY[t];
float accelsZ[t];

void setup() {
  Serial.begin(9600);

 

  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);


  

  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  a.attach(8);
  c.attach(9);
  b.attach(10);

  zeroServo(a);
  zeroServo(c);
  zeroServo(b);
  delay(1000);
  setServo(a, 90);
  setServo(c, 90);
  setServo(b, 90);
  delay(1000);
  zeroServo(a);
  zeroServo(c);
  zeroServo(b);
}


void loop() {
  
 
 
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 7*2, true);

  float x = Wire.read()<<8 | Wire.read();
  float y = Wire.read()<<8 | Wire.read();
  //float z = Wire.read()<<8 | Wire.read();

  x/=-15736;
  y/=-15736;
  float ang = atan(y/x) * (180/3.1415);
  const float multiplier = 40;
  if(x > 0 && y > 0){

    
    //Serial.println("ab");
    float aVal = x + ((sqrt(3)*y)/3);
    float bVal = (2*sqrt(3)*y)/3;
    setServo(c, 0);
    setServo(a, aVal * multiplier);
    setServo(b, bVal * multiplier);
    

    
  }else if(x > 0 && y < 0){

    
    //Serial.println("ca");
    float cVal = (-2*sqrt(3)*y)/3;
    setServo(b, 0);
    float aVal = x - ((sqrt(3)*y)/3);
    setServo(a, aVal * multiplier);
    setServo(c, cVal * multiplier);

    
  }else if(x < 0 && y > 0){
    
    ang*=-1;
    if(ang > 60){
      //Serial.println("ab");

      float aVal = x + ((sqrt(3)*y)/3);
      float bVal = (2*sqrt(3)*y)/3;
      setServo(a, aVal * multiplier);
      setServo(b, bVal * multiplier);
      setServo(c, 0);
    }else{
      //Serial.println("bc");
      float bVal =  -x + ((sqrt(3)*y)/3);
      float cVal =  (-1* ((sqrt(3)*y)/3)) -x;
      setServo(a, 0);
      setServo(b, bVal * multiplier);
      setServo(c, cVal * multiplier);
    }
    
  }else if(x < 0 && y < 0){

    if(ang < 60){
      //Serial.println("bc");

      setServo(a, 0);
      float bVal =  -x + ((sqrt(3)*y)/3);
      float cVal =  (-1* ((sqrt(3)*y)/3)) -x;
      setServo(b, bVal * multiplier);
      setServo(c, cVal * multiplier);
    }else{
      //Serial.println("ca");

      float cVal = (-2*sqrt(3)*y)/3;
      setServo(b, 0);
      float aVal = x - ((sqrt(3)*y)/3);
      setServo(a, aVal * multiplier);
      setServo(c, cVal * multiplier);
    }
    delay(30);
    
  }

  
  
  

  
}



void zeroServo(Servo servo){
  rotateServo(servo, -1000);
}

void setServo(Servo servo, int ang){
  int angle = servo.read();

  if(ang < 0){
    ang = 0;
  }else if(ang > 180){
    ang = 180;
  }

  rotateServo(servo, ang - angle);
}

void rotateServo(Servo servo, int deg){
  int sign = deg/abs(deg);//1 or -1
 
  int angle = servo.read();
 
  if(angle + deg > 180){
    deg = 180-angle;
  }else if(angle + deg < 0){
    deg = -1*angle;
  }

  int j = 0;
  for(int i = angle; j < abs(deg); i+=sign){
    servo.write(i);
    j++;
  }
}
