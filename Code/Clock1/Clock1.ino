//   Clock1.1
//   Richard Hawthorn
//   Cambridge Hackspace
//   Based on code from Kevin Rye
//   2015-05-15
//
//   7 segment led pixels
//
//    _____         __A__
//   |     |       F     B
//   |_____|  -->  |--G--|
//   |     |       E     C
//   |_____|       |__D__|
//
//   Characters
//    _____     _____     _____     _____ 
//   |     |   |     | . |     |   |     |
//   |_____|   |_____|   |_____|   |_____|
//   |     |   |     | . |     |   |     |
//   |_____| o |_____| o |_____| o |_____| o
//      1         2         3         4
//

//includes
#include <Wire.h>
#include <RTClib.h>
RTC_DS1307 RTC;

//define time variables
int hours; //24 hour time
int hh; //12 hour
int mm;
int ss;
int timecc;

//how long to pause between each character
int displayPause = 1;

//define Pins
int segmentD = 5;
int segmentE = 6;
int segmentF = 7;
int segmentA = 2;
int segmentB = 3;
int segmentC = 4;
int segmentG = 8;
int ampm = 9;
int digitOne = 10;
int digitTwo = 11;
int digitThree = 12;
int digitFour = 13;
int hoursButton = A0;
int minsButton = A1;
int decimal = A2;

//colon pin used for clock2.1 variant
int colon = A3;
boolean colonState = false;
long colonCounter = 0;

//setup
void setup() {

  //pin modes
  pinMode(segmentA, OUTPUT);
  pinMode(segmentB, OUTPUT);
  pinMode(segmentC, OUTPUT);
  pinMode(segmentD, OUTPUT);
  pinMode(segmentE, OUTPUT);
  pinMode(segmentF, OUTPUT);
  pinMode(segmentG, OUTPUT);
  pinMode(digitOne, OUTPUT);
  pinMode(decimal, OUTPUT);
  pinMode(digitTwo, OUTPUT);
  pinMode(digitThree, OUTPUT);
  pinMode(digitFour, OUTPUT);
  pinMode(colon, OUTPUT);
  
  //buttons (with pullups)
  pinMode(hoursButton, INPUT_PULLUP);
  pinMode(minsButton, INPUT_PULLUP);
  
  //setup serial line for debugging
  Serial.begin(9600);
  
  //start the i2c and clock
  Wire.begin();
  RTC.begin();  

  if (! RTC.isrunning()) {
    Serial.println("Clock is not running!");
  }
  
  //flash the colon at 1hz
  flashColon();
  
}





//format and display the time
void displayTime(){
  timecc = hh*100+mm;
  showNum(timecc);
}

//get the current time and perform basic checks
void getTime(){
  
  DateTime now = RTC.now(); //time from the clock in 24 hour mode
  hh = hours = now.hour(); //used to check AM/PM status
  mm = now.minute();
  ss = now.second(); 
  
   //Basic time check
  if (hours > 24){
    hours = 0;
  } 
  
  if (mm >= 60){
    mm = 0;
  } 
  
}

//convert the time to 12 hour format
void convert12h(){
  
  if (hours < 1) {
    hh = 12;
  }

  if ((hours > 12) && (hours < 24)) {
    hh = hours - 12;
  }
  
}

//check the buttons, do we need to update the time
void checkButtons(){
  
   //hours button pressed
  if (digitalRead(hoursButton) == LOW) {
    //basic debounce
    delay(200);
    hours++;

    if (hours > 23) {
      hours = 0;
    }
    
    updateRTC();

  } 

  //minutes button pressed
  if (digitalRead(minsButton) == LOW) {
    //basic debounce
    delay(200);

    mm++;
    ss = 0;

    if (mm >59) {
      mm = 0;
    }
    
    updateRTC();

  }
  
}

//display the time
void showNum(int n) {

  //display digit X
  //digit 1 - only display if time is greater thatn 9:59
  int n1 = (n%10000)/1000;
  if (n > 999) {
    printNumber(n1,1);
  }
  delay(displayPause);
  resetNumbers();

  //digit 2
  int n2 = (n%1000)/100;
  printNumber(n2,2); 
  delay(displayPause);
  resetNumbers();

  //digit 3
  int n3 = (n%100)/10;
  printNumber(n3,3);
  delay(displayPause);
  resetNumbers();

  //digit 4
  int n4 = n%10;
  printNumber(n4,4); 
  delay(displayPause);
  resetNumbers();

}

//reset the screen, so everything is off
void resetNumbers(){
  
    digitalWrite(digitOne,LOW);
    digitalWrite(digitTwo,LOW);
    digitalWrite(digitThree,LOW);
    digitalWrite(digitFour,LOW);
    digitalWrite(decimal,HIGH); //decimal off
    digitalWrite(segmentA,HIGH);
    digitalWrite(segmentB,HIGH);
    digitalWrite(segmentC,HIGH);
    digitalWrite(segmentD,HIGH);
    digitalWrite(segmentE,HIGH);
    digitalWrite(segmentF,HIGH);
    digitalWrite(segmentG,HIGH);  
  
}

//output a specific number in a specific position
//n = number, d = position
void printNumber(int n, int d) {
 
  if ((n > 9) || (n < 0)){
    return;  
  }
  
  if ((d > 4) || (d < 1)){
    return;  
  }
  
  //d is the display to illuminate
  if(d==1)  {
    digitalWrite(digitOne,HIGH);
    digitalWrite(digitTwo,LOW);
    digitalWrite(digitThree,LOW);
    digitalWrite(digitFour,LOW);
    digitalWrite(decimal,HIGH); //decimal off
  } 

  if(d==2) {
    digitalWrite(digitOne,LOW);
    digitalWrite(digitTwo,HIGH);
    digitalWrite(digitThree, LOW);
    digitalWrite(digitFour,LOW);
    digitalWrite(decimal,HIGH); //decimal off
  }

  if(d==3) {
    digitalWrite(digitOne,LOW);
    digitalWrite(digitTwo,LOW);
    digitalWrite(digitThree,HIGH);
    digitalWrite(digitFour,LOW);
    digitalWrite(decimal,HIGH); //decimal off
  }

  if(d==4) {
    digitalWrite(digitOne,LOW);
    digitalWrite(digitTwo,LOW);
    digitalWrite(digitThree,LOW);
    digitalWrite(digitFour,HIGH);
    digitalWrite(decimal,HIGH); //decimal off
  }
 
  //n is for the number to be displayed
  switch(n) {
  case 0:
    digitalWrite(segmentA,LOW);
    digitalWrite(segmentB,LOW);
    digitalWrite(segmentC,LOW);
    digitalWrite(segmentD,LOW);
    digitalWrite(segmentE,LOW);
    digitalWrite(segmentF,LOW);
    digitalWrite(segmentG,HIGH);
    break;
    

  case 1:
    digitalWrite(segmentA,HIGH);
    digitalWrite(segmentB,LOW);
    digitalWrite(segmentC,LOW);
    digitalWrite(segmentD,HIGH);
    digitalWrite(segmentE,HIGH);
    digitalWrite(segmentF,HIGH);
    digitalWrite(segmentG,HIGH);
    break;

  case 2:
    digitalWrite(segmentA,LOW);
    digitalWrite(segmentB,LOW);
    digitalWrite(segmentC,HIGH);
    digitalWrite(segmentD,LOW);
    digitalWrite(segmentE,LOW);
    digitalWrite(segmentF,HIGH);
    digitalWrite(segmentG,LOW);
    break;

  case 3:
    digitalWrite(segmentA,LOW);
    digitalWrite(segmentB,LOW);
    digitalWrite(segmentC,LOW);
    digitalWrite(segmentD,LOW);
    digitalWrite(segmentE,HIGH);
    digitalWrite(segmentF,HIGH);
    digitalWrite(segmentG,LOW);
    break;

  case 4:
    digitalWrite(segmentA,HIGH);
    digitalWrite(segmentB,LOW);
    digitalWrite(segmentC,LOW);
    digitalWrite(segmentD,HIGH);
    digitalWrite(segmentE,HIGH);
    digitalWrite(segmentF,LOW);
    digitalWrite(segmentG,LOW);
    break;

  case 5:
    digitalWrite(segmentA,LOW);
    digitalWrite(segmentB,HIGH);
    digitalWrite(segmentC,LOW);
    digitalWrite(segmentD,LOW);
    digitalWrite(segmentE,HIGH);
    digitalWrite(segmentF,LOW);
    digitalWrite(segmentG,LOW);
    break;

  case 6:
    digitalWrite(segmentA,LOW);
    digitalWrite(segmentB,HIGH);
    digitalWrite(segmentC,LOW);
    digitalWrite(segmentD,LOW);
    digitalWrite(segmentE,LOW);
    digitalWrite(segmentF,LOW);
    digitalWrite(segmentG,LOW);
    break;

  case 7:
    digitalWrite(segmentA,LOW);
    digitalWrite(segmentB,LOW);
    digitalWrite(segmentC,LOW);
    digitalWrite(segmentD,HIGH);
    digitalWrite(segmentE,HIGH);
    digitalWrite(segmentF,HIGH);
    digitalWrite(segmentG,HIGH);
    break;

  case 8:
    digitalWrite(segmentA,LOW);
    digitalWrite(segmentB,LOW);
    digitalWrite(segmentC,LOW);
    digitalWrite(segmentD,LOW);
    digitalWrite(segmentE,LOW);
    digitalWrite(segmentF,LOW);
    digitalWrite(segmentG,LOW);
    break;

  case 9:
    digitalWrite(segmentA,LOW);
    digitalWrite(segmentB,LOW);
    digitalWrite(segmentC,LOW);
    digitalWrite(segmentD,LOW);
    digitalWrite(segmentE,HIGH);
    digitalWrite(segmentF,LOW);
    digitalWrite(segmentG,LOW);
    break;
  }
}

//tell the clock to flash the colon
void flashColon(){
  
  //set SQW on clock to 1 hz for flashing colon
  Wire.beginTransmission(104);
  Wire.write(0x0E); // point to SQW address
  Wire.write(0b00000000); // send 1Hz
  Wire.endTransmission();
  
}

//convert decimal number to binary
byte decToBcd(byte val) {
  return ( (val/10*16) + (val%10) );
}

//convert binary number to decimal
byte bcdToDec(byte val) {
  return ( (val/16*10) + (val%16) );
}

//update the clock with the updated time
void updateRTC() {
  Wire.beginTransmission(104);
  Wire.write(0);
  Wire.write(decToBcd(ss));
  Wire.write(decToBcd(mm));
  Wire.write(decToBcd(hours));
  Wire.endTransmission();
}

void checkColon(){

  //code used to flash the colon on the v2.1 variant
  colonCounter++;

  if (colonCounter > 86){
    colonCounter = 0;
    if (colonState){
      colonState = false;
      digitalWrite(colon,HIGH);
    } else {
      colonState = true;
      digitalWrite(colon,LOW);
    }
  }
  
}



//main loop
void loop() {
 
  getTime();
  //convert12h();
  displayTime();
  checkButtons();
  checkColon();
  
}


