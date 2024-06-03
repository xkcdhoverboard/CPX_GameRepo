#include <Adafruit_CircuitPlayground.h>
#include <AsyncDelay.h>
//OBJECTIVE:create a game where players scramble to figure out which sensors change the lights to match the set lights.
//NOTE: Pixels 0-4 change while 5-9 are the refrences
//TO ADD: 30secs and 10sec countdown sounds.
//BUGS/FIXES:
//1: the reset switch is way to easy to fat finger may change it to use the switch
//2: some of the lights can start in the solved position
//3: lights near 255 or 0 can be harder to lock due to the value being wierd

volatile int LightCase[5] = {0,0,0,0,0};
volatile int LightHues[10] = {0,0,0,0,0,0,0,0,0,0};
volatile int LockTest[5] = {0,0,0,0,0};
volatile int CASE = 0;
volatile int Pixel = 0;
volatile int LevEndTest = 0;
volatile int LevelNum = 0;
volatile int LevelTime = 0;
AsyncDelay LevTimer;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  CircuitPlayground.begin();
  while(!Serial){}
  Reset();
}

void loop() {
  //use switch case to call functions pertaining to each LED
  //write random values to an array to signify witch case to use.
  if(CircuitPlayground.readCap(3) > 500){
    //reset level number
    LevReset();
    //reset level
    Reset();
    //wait until you stop touching it
    while(CircuitPlayground.readCap(3) > 500){}
  }
  //if the timer is still going run the game
  if (!(LevTimer.isExpired())){

    for(int i = 0; i < 5;i++){
      //both of these variables can be removed but are here for more readability and redundancy.
      Pixel = i;
      CASE = LightCase[i];

      if(LightHues[Pixel]>255){
        LightHues[Pixel] = 0;
      }

      if(LightHues[Pixel]<0){
        LightHues[Pixel] = 255;
      }
      
      switch(CASE) {
        case 0:
          //this light is locked do nothing
          break;
        case 1:
          LButton();
          break;
        case 2:
          RButton();
          break;
        case 3:
          SGyroX();
          break;
        case 4:
          SGyroY();
          break;
        case 5:
          SGyroZ();
          break;
        case 6:
          NLButton();
          break;
        case 7:
          NRButton();
          break;
        case 8:
          //yay
          break;
        case 9:
          //yay
          break;
        default:
          break;
      }
    }
    Lock();
    if(LevEndTest == 0){
      Serial.println("reset");
      Reset();
    }
  }
  else{
    //print the end of game serial output
    Serial.println("loss");
    Serial.println("Level Reached:");
    Serial.print(LevelNum);
    //wait until reset is preformed
    while(CircuitPlayground.readCap(3) < 500){}
    //reset level number
    LevReset();
    //reset level
    Reset();
    //wait until you stop touching it
    while(CircuitPlayground.readCap(3) > 500){}
  }
}


//THESE ARE THE CASES FOR HOW LIGHTS CAN CHANGE.


//change hue as left button is pressed
void LButton(){
  if(CircuitPlayground.leftButton()){
    LightHues[Pixel] = LightHues[Pixel] + 10;
    CircuitPlayground.setPixelColor(Pixel, CircuitPlayground.colorWheel((LightHues[Pixel]) & 255));
  }
}
//change hue as right button is pressed
void RButton(){
  if(CircuitPlayground.rightButton()){
    LightHues[Pixel] = LightHues[Pixel] + 10;
    CircuitPlayground.setPixelColor(Pixel, CircuitPlayground.colorWheel((LightHues[Pixel]) & 255));
  }
}

//change hue as the board is tilted
void SGyroX(){
  LightHues[Pixel] = LightHues[Pixel] + CircuitPlayground.motionX();
  CircuitPlayground.setPixelColor(Pixel, CircuitPlayground.colorWheel((LightHues[Pixel]) & 255));
}

void SGyroY(){
  LightHues[Pixel] = LightHues[Pixel] + CircuitPlayground.motionY();
  CircuitPlayground.setPixelColor(Pixel, CircuitPlayground.colorWheel((LightHues[Pixel]) & 255));
}

void SGyroZ(){
  LightHues[Pixel] = LightHues[Pixel] + CircuitPlayground.motionZ();
  CircuitPlayground.setPixelColor(Pixel, CircuitPlayground.colorWheel((LightHues[Pixel]) & 255));
}
void NLButton(){
  if(!CircuitPlayground.leftButton()){
    LightHues[Pixel] = LightHues[Pixel] + 10;
    CircuitPlayground.setPixelColor(Pixel, CircuitPlayground.colorWheel((LightHues[Pixel]) & 255));
  }
}
void NRButton(){
  if(!CircuitPlayground.rightButton()){
    LightHues[Pixel] = LightHues[Pixel] + 10;
    CircuitPlayground.setPixelColor(Pixel, CircuitPlayground.colorWheel((LightHues[Pixel]) & 255));
  }
}



//function to lock the lights
void Lock(){
  //set the test to 0
  LevEndTest = 0;
  //may be able to change this with an async delay?
  delay(10);
  //loop through changeable lights
  for(int i = 0; i < 5;i++){
    //are the hues of the two compared lights close enough to each other?
    //BUG #3
    if(((LightHues[9-i]+15) > LightHues[i]) && ((LightHues[9-i]-15) < LightHues[i])){
        //this is so hacky but it works lol
      LockTest[i] = LockTest[i] + 1;
      

      if(LockTest[i]>10){
        //make the colors the same
        LightHues[i] = LightHues[9-i];
        //update the color (!could just set it to the refrence color and ignore changing the hue value but the more redundancy the better).
        CircuitPlayground.setPixelColor(i, CircuitPlayground.colorWheel((LightHues[i]) & 255));
        //set the state to locked
        LightCase[i] = 0;
      }
    }
    else{
      LockTest[i] = 0;
      LevEndTest = 1;
    }
  }
}

void LevReset(){
  LevelNum = 0;
}
//reset game completly (!probably should have more safegaurds to activate than it has currently)
void Reset() {
  //turn off every light
  CircuitPlayground.clearPixels();
  //incriment the level number
  LevelNum = LevelNum + 1;
  //set up what sensors for each light
  for(int i = 0; i < 5;i++){
    LightCase[i] = random(1,8);
    LockTest[i] = 0;
    //print the CASE values to serial for easier debugging
    Serial.println(LightCase[i]);
  }
  //set up the colors of all the lights (!need to make sure they are out of the lock range at the start.)
  //BUG #2
  for(int i = 0; i < 10;i++){
    LightHues[i] = random(0,256);
    //display to pixels
    CircuitPlayground.setPixelColor((i), CircuitPlayground.colorWheel((LightHues[(i)]) & 255));
  }
  //set the timer shorter and shorter code
  if (LevelNum <= 46){
    //linearly lower level time to a minimum of 10s
    LevelTime = 60000 - ((LevelNum - 1) * 1000);
  }

  LevTimer.start(LevelTime, AsyncDelay::MILLIS);
  Serial.println("start");
  Serial.println(LevelTime);
  Serial.println(LevelNum);
}
