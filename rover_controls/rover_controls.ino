#include <Servo.h>

String command;
long unsigned previousTime=0;
class DCMotor {
  public:
  int IN1;
  int IN2;
  void sendPWM(String direction, int value);
  
  DCMotor(int x, int y){
    IN1=x;
    IN2=y;
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
  }
};

void DCMotor::sendPWM(String direction, int value){
  	if (direction=="forward"){
      digitalWrite(IN1,LOW);
      analogWrite(IN2,value);
  	}
  	else if (direction=="backward"){
      digitalWrite(IN2,LOW);
      analogWrite(IN1,value);
  	}
    else {
      Serial.println("Wrong entry for direction ");
    }
}
Servo wheelRF;
Servo wheelLF;
Servo wheelRB;
Servo wheelLB;

class Rover{
  public:

  int initRF=70;
  int initLF=67;
  int initRB=67;
  int initLB=67;
  int posRF=0;
  int posLF=0;
  int posRB=0;
  int posLB=0;
  void initServos();
  void steer(int valRF, int valLF, int valRB, int valLB, int speed);
  void goToSpeed(String direction, int targetSpeed, int currentSpeed);
  void keepRoverAtSpeed(String direction, int speed);

};

DCMotor motorRF(46,10);//U7
DCMotor motorLF(2,3);
DCMotor motorRM(13,44);
DCMotor motorLB(4,5);//changed
DCMotor motorLM(8,9);//changed
DCMotor motorRB(7,6);//changed

void Rover::initServos(){
  wheelRF.attach(25);
  wheelLF.attach(23);
  wheelRB.attach(24);
  wheelLB.attach(22);
  delay(25);
  wheelRF.write(initRF);
  posRF=initRF;
  delay(25);
  wheelLF.write(initLF);
  posLF=initLF;
  delay(25);
  wheelRB.write(initRB);
  posRB=initRB;
  delay(25);
  wheelLB.write(initLB);
  posLB=initLB;
  delay(25);
  //delays to avoid current peak
}

void Rover::steer(int valRF, int valLF, int valRB, int valLB, int speed){
  int incrementRF;
  int incrementLF;
  int incrementRB;
  int incrementLB;
  if (valRF > posRF){
    incrementRF=1;}
  else {
    incrementRF=-1;}
  if (valLF > posLF){
    incrementLF=1;}
  else {
    incrementLF=-1;}
  if (valRB > posRB){
    incrementRB=1;}
  else {
    incrementRB=-1;}
  if (valLB > posRB){
    incrementLB=1;}
  else {
    incrementLB=-1;}
  while(posRF != valRF || posLF != valLF || posRB != valRB || posLB != valLB){
    
    if(posRF != valRF){
      posRF=posRF+incrementRF;
      wheelRF.write(posRF);
    }
    if(posLF != valLF){
      posLF=posLF+incrementLF;
      wheelLF.write(posLF);
    }
    if(posRB != valRB){
      posRB=posRB+incrementRB;
      wheelRB.write(posRB);
    }
    if(posLB != valLB){
      posLB=posLB+incrementLB;
      wheelLB.write(posLB);
    }
    delay(speed);
  }

}

void Rover::goToSpeed(String direction, int targetSpeed, int currentSpeed){
  //acceleration
  if (targetSpeed>currentSpeed){
    for (int i=currentSpeed; i<=targetSpeed; i++){
      motorRF.sendPWM(direction,i);
      motorLF.sendPWM(direction,i);
      motorRM.sendPWM(direction,i);
      motorLM.sendPWM(direction,i);
      motorRB.sendPWM(direction,i);
      motorLB.sendPWM(direction,i);
      delay(5);
    }
  }
  //deceleration
  if (currentSpeed>targetSpeed){
    for(int i=currentSpeed; i>=targetSpeed; i--){
      motorRF.sendPWM(direction,i);
      motorLF.sendPWM(direction,i);
      motorRM.sendPWM(direction,i);
      motorLM.sendPWM(direction,i);
      motorRB.sendPWM(direction,i);
      motorLB.sendPWM(direction,i);
      delay(5);
    }
  }
}

void Rover::keepRoverAtSpeed(String direction, int speed){
  motorRF.sendPWM(direction,speed-100);
  motorLF.sendPWM(direction,speed-91);
  motorRM.sendPWM(direction,speed-91);
  motorLM.sendPWM(direction,speed-50);
  motorRB.sendPWM(direction,speed);
  motorLB.sendPWM(direction,speed-91);
}
Rover Adversity;
int redLed=48;
int greenLed=47;
int blueLed=49;

void setup()
{
  Serial.begin(115200);
  Adversity.initServos();
  delay(500);
  pinMode(redLed,OUTPUT);
  pinMode(greenLed,OUTPUT);
  pinMode(blueLed,OUTPUT);
  digitalWrite(redLed,LOW);
  digitalWrite(blueLed,LOW);
  
  for (int i=0; i<5; i++){
      digitalWrite(greenLed,HIGH);
      delay(300);
      digitalWrite(greenLed,LOW);
      delay(300);
  }
  //Serial.println("setup ready");  
}

void loop()
{
  Executor();
}

String readRoverTeleop(){
  String input;
  if(Serial.available()>0){
    input=Serial.readStringUntil('\r');
    Serial.println("received");
    return input;
  }
  else {
    return "empty";
  }
}

void Executor(){
  while (readRoverTeleop()=="empty");
  command=readRoverTeleop();
  switch (command){
    case F:
      while (readRoverTeleop()=="empty"){
        digitalWrite(blueLed,HIGH);
        Adversity.keepRoverAtSpeed("forward", 255);
      }
      digitalWrite(blueLed,LOW);
      break;
    case B:
      while (readRoverTeleop()=="empty"){
        digitalWrite(blueLed,HIGH);
        Adversity.keepRoverAtSpeed("backward",255);
      }
      digitalWrite(blueLed,LOW);
      break;
    case R:
      digitalWrite(blueLed,HIGH);
      digitalWrite(greenLed,HIGH);
      Adversity.steer(Adversity.posRF+5, Adversity.posLF+5,Adversity.posRB+5,Adversity.posLB+5,20);
      previousTime=millis();
      while (millis()-previousTime < 500){}
      digitalWrite(blueLed,LOW);
      digitalWrite(greenLed,LOW);
      break;
    case L:
      digitalWrite(blueLed,HIGH);
      digitalWrite(greenLed,HIGH);
      Adversity.steer(Adversity.posRF+5, Adversity.posLF+5,Adversity.posRB+5,Adversity.posLB+5,20);
      previousTime=millis();
      while (millis()-previousTime < 500){}
      digitalWrite(blueLed,LOW);
      digitalWrite(greenLed,LOW);
      break;
  }
}
