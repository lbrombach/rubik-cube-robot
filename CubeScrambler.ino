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

 
#include "MotorStuff.h" 
#include <LiquidCrystal_I2C.h>


const int DELAY = 100;  //delay for button control and debouncing
const int MAX_MODE = 9; //currently only one mode available. More modes planned in future.
int mode = 1;           //current mode
int numLoops = 0;       //incremented each loop cycle - used for randomSeed()
Robot robot;            //create Robot object


LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

 
void printMenu();

 ///////////////////////////////SETUP()///////////////
void setup() {
  pinMode(dn, INPUT_PULLUP);  //set menu button inputs
  pinMode(up, INPUT_PULLUP);  //set menu button inputs
  pinMode(GO, INPUT_PULLUP);  //set menu button inputs
  lcd.init();                 //initialize the lcd
  lcd.backlight();            //turn on backlight
  Serial.begin(9600);         //just for debugging, all calls to Serial class can be commented out if desired
  Serial.println("Hello world");
  lcd.print("INITIALIZING");
  if(robot.initialize())      
  {
    printMenu();
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("INIT FAILED");
    lcd.setCursor(0,1);
    lcd.print("Check Stuck?");
    while(1);
    
  }
}


 ////////////////LOOP()/////////////////////////////
void loop() {
  numLoops++;
delay(DELAY);
  if(!digitalRead(GO))                                          //give time to make sure user isn't trying to enter settings
  {
    delay(100);  
  }
  if(!digitalRead(GO) && digitalRead(up) && digitalRead(dn))    //starts scrambling
  {   
    if(mode==1){robot.Mode_1_GO(lcd, numLoops); printMenu();}   //insert future mode calls below this line
  }
  else if(!digitalRead(up) && !digitalRead(GO))                 //pressing GO and up buttons takes user to settings
  {
    robot.Settings(lcd); printMenu();
  }
  else if(!digitalRead(dn) && digitalRead(up))                  //increment mode
  {
    mode = (mode == MAX_MODE) ? 1 : mode+1;
  }
  
}

////////////printMenu()//////////////////
void printMenu()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("MODE ");
  lcd.print(mode);
  lcd.print("     ");
  lcd.print(robot.getMin());
  lcd.print("/");
  lcd.print(robot.getMax());
  lcd.setCursor(0,1);
  lcd.print("up+GO=cfg or GO");
  delay(1250);
}
