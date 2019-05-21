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
#include <Stepper.h>
#include <LiquidCrystal_I2C.h>

const int dn = A3; //down button 
const int up = 8;  //up button
const int GO = 9;  //Go button - acts as enter button

class Robot
{
  public:
    // constructor
    Robot();
    
    // Runs top slide end to end and zeros stepper.
    bool initialize();

    // menu option to adjust settings like min and max number of moves
    void Settings(LiquidCrystal_I2C& lcd);
    
    //starts basic scrambling routine
    void Mode_1_GO(LiquidCrystal_I2C &lcd, int numLoops); 

    //todo: add future modes here
    //
    //
    
    //returns min number of moves set in settings
    int getMin(){return minMoves;}

    //returns max number of moves set in settings
    int getMax(){return maxMoves;}

    
  private:
    //performs one randomized number of actuations of cube on one randomized row OR performs one of two flips to change axis of future actuations
    bool Move(int seed);

    //raised piston and lowers rack to bottom position
    bool Park();

    //loads cube from parked position
    bool Load();

    //twists bottom row
    bool TwistX1();

    //twists middle row
    bool TwistX2();

    //twists top row
    bool TwistX3();

    //flips cube 90 degrees forward 
    bool FlipY();

    //twists all three rows once, effectively turning the cube 90 degrees, then flips. 
    //net effect is to flip the cube 90 degrees to the left
    bool FlipZ();
  
    //Moves Rack and Tumbler to Bbottom
    bool RackToBottom();
    
    //Moves Rack and Tumbler to Top  
    bool RackToTop();
    
    //STEPPER_MOVETO()
    bool MoveTo(int requestedPos);



    const int rackBottomES = A0;      //end switch. pulled high, goes low as proof
    const int rackTopES = A1;         //end switch. pulled high. goes low as proof
    const int slideBottomES = A2;     //end switch - goes low when stepper-actuated piston reaches bottom position
    const int rackDown = 13;          //normally high output - drive low to move rack up
    const int rackUp = 12;            //normally high output - drive low to move rack down
    const int rackEnablePin = 11;     //output - set high to enable rack motor

    const int STEPS_PER_REV = 200;    //const per motor specs
    const int STEPPER_ENABLE = 3;     //low to disable power to stepper, high to enable
    const int turnTopPos = 0;         //stepper position in steps from bottom home position
    const int turnMidPos = 84;        //stepper position in steps from bottom home position
    const int turnBottomPos = 162;    //stepper position in steps from bottom home position
    const int flipTopPos = 324;       //stepper position in steps from bottom home position
    const int flipRecoveryPos = 310;  //stepper position in steps from bottom home position
    const int MIN_MIN = 1;            //minimum number of moves user can request
    const int MAX_MAX = 99;           //maximum number of moves user can request


    // Create Instance of Stepper Class
    // Specify Pins used for motor coils
    // The pins used are 8,9,10,11 
    // Connected to L298N Motor Driver In1, In2, In3, In4 
    // Pins entered in sequence 1-2-3-4 for proper step sequencing
    Stepper myStepper = Stepper(STEPS_PER_REV, 4, 5, 6, 7);

  
    int currentPlane = 1;             //planes 1,2,3 == x,y,z. For tracking which way was originally up, etc
    int stepsCounted = 0;             //used for setting stepper postions constants. May be useful for future/new builds. 
    int currentPosition = 0;          //updated by number of steps sent through controller. No feedback so can become innacurate if machine jams or belt slips.
    int waitTime = 320;               //time allowed for rack to complete movements
    int minMoves;                     //the minimum number of actuations program will execute - settable by user in settings
    int maxMoves;                     //the maximum number of actuations program will execute - settable by user in settings
  
  
}; //////////Robot class ends here//////////
