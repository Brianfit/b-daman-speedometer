#include <StopWatch.h>
#include <LiquidCrystal.h>

//A speedometer for measuring the speed of a B-Daman marble:
//The marble is fired along a short track. At the beginning of the track,
//the marble breaks a lightbeam shining on a photosensor signalling on port A0
//then hits a bar at the back of the track triggering a press-switch signalling on port A5
//the speed is calculated and displayed on a 16x1 LCD display
//By calclating the time between breaking the beam and the tripping of the switch, 
//and dividing that time by the length of the run, we can determine the marble's speed. 

// initialize the LCD Display library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
//Initialize the stopwatch
StopWatch MySW;
//I have no idea what this does, just copied it from the example.
StopWatch SWarray[5];
//RunwayLength in millimeters: measure distance from the photosensor to the switch
//and put result here CHANGE THIS TO MATCH YOUR RUNWAY
float RunwayLength = 250;
//Create a variable to hold the speed of the marble
float Speed;
//Create a very precise variable that you can calculate fractions with
float RawSpeed;
//Create a variable for the reading from the photosensor
int LightReading;
//The LightAverage will be the value we test against. If a marble crosses the beam, the
//Lightreading will drop by a certain amount below the Lightaverage value.
int LightAverage;
//And We'll calculate our LightAverage value with three readings:
int LightLevel1;
int LightLevel2;
int LightLevel3;
// The LED will trigger to confirm the marble crosses the lightbeam
const int ledPin =  13; 
float StopWatchTime;


void setup() {
Serial.begin(115200);
//Calibrate the photosensor by taking 3 readings and calculating an average value  
//Wow did it take me a long time to figure out how important recalibration is: the ambient light
//changes the readings so quickly. 
LightLevel1 = analogRead(0);
delay(500);
LightLevel2 = analogRead(0);
delay(500);
LightLevel3 = analogRead(0);
delay(500);
LightAverage = ((LightLevel1 + LightLevel2 + LightLevel3)/3); 
Serial.print("LightAverage: ");
Serial.println(LightAverage);
}


void loop(){   
//Read the photosensor and announce readiness state
lcd.print(" OK!");
//Do nothing but watch the light sensor for that marble. If the light reading drops more than 10 
//points below average, something broke the beam. Your measure of how big a drop may vary depending on
//sensitivity of your photosensor. Don't make the mistake I did and run other lines of code during this
//phase: the beam has to be monitored millisecond by millisecond to detect the break: it's fast! A Do..While
//loop that traps only that fuction was the only thing I found that worked. 
  do
    {
      LightReading = analogRead(0);
    }
  while (LightReading > (LightAverage - 10));

    SWarray[0].start(); //Start the stopwatch

// The following Do...While loop just stalls for time until the marble hits the press-switch
// and sends a pulse to A5  

    do {digitalWrite(ledPin, HIGH); 
      StopWatchTime = SWarray[0].elapsed();
    } while(analogRead(5) < 100);
  
//Now the marble has pressed the switch, so turn off the LED     
   digitalWrite(ledPin, LOW);  
//And stop the clock
  SWarray[0].stop();
  SWarray[0].reset();
  
  //Send data to the Serial Port
  Serial.print("Time in miliseconds: ");
  Serial.println(StopWatchTime);   
  StopWatchTime = (StopWatchTime);
  RawSpeed =(RunwayLength / (StopWatchTime));
  
  Serial.print("Speed in milimeters per millisecond: ");
  Serial.println(RawSpeed);
  Speed =(RawSpeed);  
  Serial.print("Speed in Kilometers/hour: ");
  //Convert millimeters per milliseconds to Km per hour:
  Speed = (Speed * 3.6);
  Serial.println(Speed);
  
  //And display the speed on the LCD
  //This code is only necessary if you are running an LCD screen, but it won't
  //do any harm if you leave it in. NB: My LCD is a one row one: 16x1, most are 16x2 -- the
  //Arduino LCD library has the code for a normal 16x2 display, it's slightly
  //different in how it initializes and moves the cursor.
  lcd.begin(8, 2);     //is 16x1, adressed as 8x2
  lcd.setCursor(0,1);  //init right hand side
  lcd.home();          //back to start
  lcd.clear();
  // Print a message to the LCD.
  lcd.print("KMH:");  //print left side
       lcd.setCursor(0,1);     //go to right
  lcd.print(Speed);      //print right side
  //blinking cursor
  lcd.cursor();
  lcd.blink();
  
//Reset the Stopwatch
   StopWatchTime = 0;
//And Delay big time so the marble doesn't reset the photosensor on the rebound
      delay(3500);
//Recalibrate the photosensor by taking 3 readings and calculating an average value  
LightLevel1 = analogRead(0);
delay(500);
LightLevel2 = analogRead(0);
delay(500);
LightLevel3 = analogRead(0);
delay(500);
LightAverage = ((LightLevel1 + LightLevel2 + LightLevel3)/3); 
Serial.print("LightAverage: ");
Serial.println(LightAverage);
//When this finishes the unit is ready for another measure, and the OK! readiness indicator comes
//on at the top of the loop. 
}


