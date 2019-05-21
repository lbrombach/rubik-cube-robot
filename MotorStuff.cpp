/*
  Copyright (c) 2019 Lloyd Brombach. All right reserved.
  All parts of this project are free for personal or educational use only.
  This include the software and 3d model files.
  Selling any part of this project without permission is prohibited.
  Commercial interests please contact Lloyd Brombach at lbrombach2@yahoo.com

  Demo Video here: https://youtu.be/9KrlmVzIU54
  More details, documentation, and more of Lloyd's porjects at lloydbrombach.wordpress.com

  This project and software are provided without warranty, and are a work in progress. 
  Feedback, suggestions, bug reports, and robotics/software developer job offers are appreciated. 

  Lloyd Brombach is the bottom-to-top designer of the machine and this code, as well as a 15-year stationary engineer that specializes in equipment controls. 
  He spends his workdays repairing or designing and building controls for commercial equipment, and in his spare time 
  studies Computer Science at Wayne State University and contributes as to the University Robotics Team as chief jack-of-all trades. 
  His comfort with both software and hardware (sensors, actuators, motors, electrical, mechanical) makes him capable of making anything work. 
  Favorite quote "I have no special talent, I am only passionately curious." - Einstein, according to the internet.



  Special thanks to the developers of the Arduino, Stepper, 
  and I2C_Liquid_Crystal_Display libraries.
***Insert citations***

*/


#include <Arduino.h>
#include "MotorStuff.h"
#include <Stepper.h>


Robot::Robot()
{
  minMoves = 25;              //default minimum number of actuations per program cycle
  maxMoves = 30;              //default maximum number of actuations per program cycle
  myStepper.setSpeed(150);    //setting stepper speed. May need to vary depending on stepper and stepper controller
}

// menu option to adjust settings like min and max number of moves
//***REMEMBER: If digitalRead(pin) returns true, then switch IS NOT made, therefore if (!digitalRead(pin)) means the button is pressed or end switch is made
void Robot::Settings(LiquidCrystal_I2C& lcd)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("up/dn to set MIN");
  lcd.setCursor(0,1);
  lcd.print("GO to enter");
  lcd.setCursor(14,1);
  lcd.print(minMoves);    
  delay(1250);                    //pause to make sure we arent reading the button press that brought us to this menu
  while(digitalRead(GO))          //set min first, then when go is pressed, break out of this loop and move on to set the max
  {
 
    while(digitalRead(GO))        
    {
      delay(125);   
    
      if(!digitalRead(up))        //if UP button IS pressed, increment minMoves
      {
        minMoves = (minMoves == MAX_MAX) ? MAX_MAX : minMoves+1;
        if(minMoves > maxMoves)   //ensure min does not exceed the max
        {
          maxMoves = minMoves;
        }
        Serial.println(minMoves);
      }
      
      if(!digitalRead(dn))        //if down button IS pressed, decrement minMoves
        {
          minMoves = (minMoves == MIN_MIN) ? MIN_MIN : minMoves-1;
          Serial.println(minMoves);
        }
      lcd.setCursor(14,1);
      if(minMoves<10)
      {
        lcd.print(" ");
      }
      lcd.print(minMoves);    
    }
  }

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("up/dn to set MAX");
  lcd.setCursor(0,1);
  lcd.print("GO to enter");
  lcd.setCursor(14,1);
  lcd.print(maxMoves);  
  delay(1250);
     
  while(digitalRead(GO))   //now we set the maxMoves, breaking out when GO is pressed
  {
    lcd.setCursor(14,1);
    lcd.print(maxMoves); 
    while(digitalRead(GO))
      {
        delay(125);    
        if(!digitalRead(up))
        {
          maxMoves = (maxMoves == MAX_MAX) ? MAX_MAX : maxMoves+1;
        }
        if(!digitalRead(dn))
        {
          maxMoves = (maxMoves == MIN_MIN) ? MIN_MIN : maxMoves-1;
          if(maxMoves < minMoves)
          {
            minMoves = maxMoves;
          }
        }
        lcd.setCursor(14,1);
        if(maxMoves<10)
        {
          lcd.print(" ");
        }
        lcd.print(maxMoves);
      }
  }

}

//this is the basic scrambling routine
void Robot::Mode_1_GO(LiquidCrystal_I2C& lcd, int numLoops)
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Go for Mode 1");
  Serial.println("Go for mode 1");
  digitalWrite(STEPPER_ENABLE, HIGH);
  Load();
  randomSeed(numLoops);
  int i = random(minMoves, maxMoves);
  Serial.println(i);
  while(i-- > 0)
  {
    lcd.setCursor(7,1);
    lcd.print(i);
    lcd.print(" ");
    Move(numLoops+2*i);                 //send Move() another semi-random seed
  }
  
  Park();
  digitalWrite(STEPPER_ENABLE, LOW);    // turn steppers off when not in use
}


//loads cube from parked position to bottom of chamber, zeroing stepper motor while it's there
bool Robot::Load()
{
  MoveTo(flipRecoveryPos);
  RackToTop();
  MoveTo(flipTopPos);
}

//moves cube to top and lowers rack to bottom
bool Robot::Park()
{
  MoveTo(flipTopPos);
  Serial.println(currentPosition);
  RackToBottom();
  return true;
}



// this is the function that selects next move
// **todo: needs to be more intelligent than current, simple random method
// 
//
bool Robot::Move(int seed)
{
  static int lastMove;
  int move;
//  if(lastMove == 4 || lastMove == 5 ) 
//  {
    move =  random(1,6);
    if (move == lastMove)
    {
      move = random(1,6);
    }
//  }
//  else
//  {
//    move = random(1,6);
//  }
  lastMove = move;
  if(move==1) {Serial.println("twist bottom");return TwistX1();}
  if(move==2) {Serial.println("twist middle");return TwistX2();}
  if(move==3) {Serial.println("twist top");return TwistX3();}
  if(move==4) {Serial.println("Flip Y");return FlipY();}
  if(move==5) {Serial.println("Flip Z");return FlipZ();}
 
  return true;
}




bool Robot::TwistX1()
{
  Serial.println("        Twisting Bottom");
  MoveTo(turnBottomPos);
  RackToBottom();
  RackToTop();
  return true;
}

bool Robot::TwistX2()
{
  Serial.println("          Twisting Middle");
  MoveTo(turnMidPos);
  RackToBottom();
  RackToTop();
  return true;
}

bool Robot::TwistX3()
{
  Serial.println("         Twisting TOP");
  MoveTo(turnTopPos);
  RackToBottom();
  RackToTop();
  return true;
}

bool Robot::FlipY()
{
  Serial.println("         flipping Y");
  MoveTo(flipTopPos);
  RackToBottom();
  MoveTo(flipRecoveryPos);
  RackToTop();
  MoveTo(turnBottomPos);
  return true;  
}

bool Robot::FlipZ()
{
  Serial.println("        flipping Z");
  TwistX3();
  TwistX2();
  TwistX1();
  FlipY();
  return true;  
}


//Moves Rack and Tumbler to Bottom
bool Robot::RackToBottom()
{
  Serial.println("rack going to bottom");
  digitalWrite(rackEnablePin, HIGH);
  bool madeEndSW = false;
  int waited = 0;
  digitalWrite(rackDown, LOW);   
  Serial.print("waiting waitTime = ");
  Serial.println(waitTime);
  delay(waitTime);
    while(madeEndSW == false && waited < 100 )  //give time to make end switch
    {
      madeEndSW = !digitalRead(rackBottomES);
      delay(1);
      waited++;
    }
  digitalWrite(rackDown, HIGH);
  waited = 0;

  if(madeEndSW == false)                        //if failed reverse to unstick then retry
  {
    Serial.println("*******try 2******");
    myStepper.step(2);
    digitalWrite(rackUp, LOW);
    myStepper.step(-4);
    myStepper.step(2);
    delay(waitTime);
    digitalWrite(rackUp, HIGH);
    
  digitalWrite(rackDown, LOW); 
  delay(waitTime);
    while(madeEndSW == false && waited < 100)  //give time to make end switch
    {
      madeEndSW = !digitalRead(rackBottomES);
      delay(1);
      waited++;
    }
  digitalWrite(rackDown, HIGH);
  waited = 0;
  }

    if(madeEndSW == false)                     //if failed reverse to unstick then retry
  {
    Serial.println("*******try 3******");
    myStepper.step(2);
    digitalWrite(rackUp, LOW);
    myStepper.step(-4);
    myStepper.step(2);
    delay(waitTime);
    digitalWrite(rackUp, HIGH);
    
  digitalWrite(rackDown, LOW); 
    while(madeEndSW == false && waited < 100)  //give time to make end switch
    {
      madeEndSW = !digitalRead(rackBottomES);
      delay(1);
      waited++;
    }
  digitalWrite(rackDown, HIGH);
  }

  digitalWrite(rackEnablePin, LOW);
  
    if(!madeEndSW)
    {
      Serial.println("FAILURE RACKING DOWN");
      MoveTo(turnTopPos);
      return false;
    }
  

  return true;   
}  


//Moves Rack and Tumbler to Top
bool Robot::RackToTop()
{
  Serial.println("rack going to top");
  digitalWrite(rackEnablePin, HIGH);
  bool madeEndSW = false;
  int waited = 0;
  digitalWrite(rackUp, LOW); 
  delay(waitTime);
    while(madeEndSW == false && waited < 100 ) //give time to make end switch
    {
      
      madeEndSW = !digitalRead(rackTopES);
      delay(1);
      waited++;
    }
  digitalWrite(rackUp, HIGH);
  waited = 0;
    
  if(madeEndSW == false)                       //if failed reverse to unstick then retry
  {
    Serial.println("*******try 2******");
    myStepper.step(2);
    digitalWrite(rackDown, LOW);
    myStepper.step(-4);
    myStepper.step(2);
    delay(waitTime);
    digitalWrite(rackDown, HIGH);
    
  digitalWrite(rackUp, LOW); 
  delay(waitTime);
    while(madeEndSW == false && waited < 100)  //give time to make end switch
    {
      madeEndSW = !digitalRead(rackTopES);
      delay(1);
      waited++;
    }
  digitalWrite(rackUp, HIGH);
  waited = 0;
  }

  if(madeEndSW == false)                       //if failed reverse to unstick then retry - last try
  {
    Serial.println("*******try 3******");
    myStepper.step(2);
    digitalWrite(rackDown, LOW);
    myStepper.step(-4);
    myStepper.step(2);
    delay(waitTime);
    digitalWrite(rackDown, HIGH);
    
  digitalWrite(rackUp, LOW); 
  delay(waitTime);
    while(madeEndSW == false && waited < 100)  //give time to make end switch
    {      
      madeEndSW = !digitalRead(rackTopES);
      delay(1);
      waited++;
    }
  digitalWrite(rackUp, HIGH);
  }
  
  digitalWrite(rackEnablePin, LOW);
  
    if(!madeEndSW)
    {
      Serial.println("FAILURE RACKING UP");
      MoveTo(turnTopPos);
      return false;
    }
  

  return true;
}

//moves stepper to requested position
bool Robot::MoveTo(int requestedPos)
{
  delay(100);
  if(requestedPos == 0)
  {    
   Serial.println("moving to bottom");
   MoveTo(10);                         //quickly moves close to home/bottom/zero position
   while(digitalRead(slideBottomES))   //moves to end switch/home one step at a time, then zeros position counter
   {
    stepsCounted = 0;
    myStepper.step(1);
   }   
     currentPosition = 0;
     return true;
  }
  else if (requestedPos==currentPosition)
  {
    Serial.println("already here");
      return true;
  }
  else if (requestedPos>currentPosition)
  {
    Serial.print("moving up");
    Serial.println(0-(requestedPos-currentPosition));
    myStepper.step(0-(requestedPos-currentPosition));
    currentPosition = requestedPos;
    return true;
  }
  else if (requestedPos<currentPosition)
  {
    Serial.print("moving down");
    Serial.println(requestedPos-currentPosition);
    myStepper.step(0-(requestedPos-currentPosition));
    currentPosition = requestedPos;
    return requestedPos;
  } 
  
}

//initializes during startup - called by constructor 
bool Robot::initialize() {  
  Serial.println("initializing");

  pinMode(slideBottomES, INPUT_PULLUP);
  pinMode(rackBottomES, INPUT_PULLUP);
  pinMode(rackTopES, INPUT_PULLUP);   
  pinMode(rackUp, OUTPUT);
  pinMode(rackDown, OUTPUT); 
  pinMode(rackEnablePin, OUTPUT);
  pinMode(STEPPER_ENABLE, OUTPUT);
  digitalWrite(rackUp,HIGH);
  digitalWrite(rackDown,HIGH);
  
  RackToTop();
  digitalWrite(STEPPER_ENABLE, HIGH);
  while(digitalRead(slideBottomES))
    {
      Serial.println("stepping to home");
      myStepper.step(1);
    }

  currentPosition = 0;
  Park();
  digitalWrite(STEPPER_ENABLE, LOW);
  
  return true;
}
