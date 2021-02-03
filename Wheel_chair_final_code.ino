
/*
 * 
 * This code is basic usage MPU-6050 Accelerometer and Gyroscope
 * 
 * This code displays all data:
 * -GyroScopeX, Gyro Y, Gyro Z
 * -Gyro Angle X, Gyro Angle Y, Gyro Angle Z 
 * -Accel X, Accel Y, Accel Z
 * -Accel Angle X, Accel Angle Y,Accel Angle Z,
 * 
 * Library and code have been taken from:
 * https://github.com/tockn/MPU6050_tockn
 * 
 * Updated by Ahmad Shamshiri on July 03, 2018 in Ajax, Ontario, Canada
 * for Robojax.com
 * Get this code from Robojax.com
 * Watch video instruction for this code at:https://youtu.be/uhh7ik02aDc
 * 
 */
 
#include <MPU6050_tockn.h>
#include <Wire.h>

int ML = 11, MR = 9;
int state = 0;

MPU6050 mpu6050(Wire);

float prev_X = 0, prev_Y = 0;
int len_que = 10;
int queue_X[10];
int kernal_X_up[] = {-1,-6,-11,-6,-1,1,6,11,6,1}; 
int kernal_X_down[] = {1,6,11,6,1,-1,-6,-11,-6,-1};
bool up=false, down=false;

int theta = 0;

void setup() {
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  
  Serial.begin(9600);
  pinMode(ML, OUTPUT);
  pinMode(MR, OUTPUT);
}

void loop() {
  mpu6050.update();
  float aX = mpu6050.getAngleX();
  float aY = mpu6050.getAngleY();

  int dX=0, dY=0;
  dX = int(aX - prev_X);
  dY = int(aY - prev_Y);
//-------------------------------------------------------------
  update_queue_X(dX); 
  int match_up = find_similarity_X_up();
  int match_down = find_similarity_X_down();
  
  up = match_up>400?true:false;
  down = match_down>400?true:false;
//-------------------------------------------------------------

  theta += dY;
  
  Serial.print(up);
  Serial.print(" ");
  Serial.print(down);
  Serial.print(" ");
  Serial.println(theta); 

  if(up){
    state = 1;
  }
  else if(down){
    state = 0;
  }
  else if(theta > 30 && state!=1){
    state = 2; //Left
  }
  else if(theta < -30 && state!=1){
    state = 3; //Right
  }
  else if(theta <30 && theta >-30 && state!=1){
    state = 0;
  }
  

  drive_motor(state);
  
  prev_X = aX;
  prev_Y = aY;
  delay(100);
} 

void update_queue_X(int val){
  for(int i=0;i<len_que-1;i++){
    queue_X[i] = queue_X[i+1];
  }
  queue_X[len_que-1] = val;
}

void display_queue_X(){
  for(int i=0;i<len_que;i++){
    Serial.print(queue_X[i]);
    Serial.print(',');
  }
  Serial.println();
}

int find_similarity_X_up(){
  int sum=0;
  for(int i=0;i<len_que;i++){
    sum += (queue_X[i]*kernal_X_up[i]);
  }
  return sum;
}

int find_similarity_X_down(){
  int sum=0;
  for(int i=0;i<len_que;i++){
    sum += (queue_X[i]*kernal_X_down[i]);
  }
  return sum;
}

void drive_motor(int s){
  if(s==1){
    analogWrite(ML, 50);
    analogWrite(MR, 0);
  }
  else if(s==0){
    analogWrite(ML, 255);
    analogWrite(MR, 255);
  }
  else if(s==2){ //left
    analogWrite(ML, 255);
    analogWrite(MR, 0);
  }
  else if(s==3){ //right
    analogWrite(ML, 50);
    analogWrite(MR, 255);
  }
}
