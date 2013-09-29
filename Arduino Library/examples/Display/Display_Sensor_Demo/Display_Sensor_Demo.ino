#include <Sparki.h>  // include the robot library

SparkiClass robot;

void setup() {
  // put your setup code here, to run once:
  robot.begin();
  delay(1000);
}

void loop() {
  int x = 8;
  int gap =23;
  robot.clear(); // clear the display so that we can write new things on it
  
  //Left Edge IR
  yBarGraph(x+gap*0,0,2,robot.edgeLeft());
  
  // Left Center IR
  yBarGraph(x+gap*1,0,2,robot.lineLeft());
  
  // Center Center IR
  yBarGraph(x+gap*2,0,2,robot.lineCenter());
  
  // Right Center IR
  yBarGraph(x+gap*3,0,2,robot.lineRight());
  
  // Right Edge IR
  yBarGraph(x+gap*4,0,2,robot.edgeRight());
  
  // Photo Left
  yBarGraph(x+gap*1,25,5,robot.lightLeft());
  
  // Photo Center
  yBarGraph(x+gap*2,25,5,robot.lightCenter()); 
  
  // Photo Right
  yBarGraph(x+gap*3,25,5,robot.lightRight());
  
  robot.display(); // show the new image data on the LCD (it doesn't show on the LCD until here)
  
  delay(100); // slow it down to make the text readable
}

void yBarGraph(int xStart, int yStart, int yChar, int value){
  int maxVal = 1024;
  int yMax = 10;
  
  //int dispVal = 1;
  int dispVal = (int)((float)value/((float)maxVal/((float)yMax)));
  
  robot.fillrect(xStart,
                yStart+(yMax-dispVal+1),
                5,
                (dispVal+1),
                0); 
  
  int dispChar;
  
  robot.drawchar(0 * 6 + xStart, yChar, (value/100)%10+48);
  robot.drawchar(1 * 6 + xStart, yChar, (value/10)%10+48);
  robot.drawchar(2 * 6 + xStart, yChar, value%10+48);
  
  robot.fillrect(xStart,
                yStart+(yMax-dispVal+1),
                5,
                (dispVal+1),
                1);    
}
