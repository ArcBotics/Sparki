#include <Sparki.h>  // include the sparki library

const int threshold = 500; //line and edge sensors thereshold.

bool  edgeLeft = false,
      lineLeft = false,
      lineCenter = false,
      lineRight = false,
      edgeRight = false;

String state = "undefined";

void showSensorsAndState()
{
  sparki.clearLCD();

  sparki.print("eL = ");
  sparki.println(edgeLeft);
  sparki.print("lL = ");
  sparki.println(lineLeft);
  sparki.print("lC = ");
  sparki.println(lineCenter);
  sparki.print("lR = ");
  sparki.println(lineRight);
  sparki.print("eR = ");
  sparki.println(edgeRight);
  sparki.println();
  sparki.println(String("state = ") + state);

  sparki.updateLCD();
}

void readIRSensors()
{
  //each sensor is 1 if reading white, and 0 if reading black:
  edgeLeft =  sparki.edgeLeft() > threshold;
  lineLeft =  sparki.lineLeft() > threshold;
  lineCenter = sparki.lineCenter() > threshold;
  lineRight =  sparki.lineRight() > threshold;
  edgeRight = sparki.edgeRight() > threshold;
}

//Returns true if the robot is centered over the Home mark:
bool robotIsCenteredOverHomeMark()
{
  //The robot is centered once all it's edge and line sensors read black:
  return !edgeLeft && !lineLeft && !lineCenter && !lineRight && !edgeRight;
}

void centerRobotOverHomeMark()
{
  state = "centering";
  readIRSensors();
  while(!robotIsCenteredOverHomeMark())
  {
    readIRSensors();
    showSensorsAndState();
  }
  sparki.beep();
}


void setup()
{
  sparki.servo(SERVO_CENTER);
  centerRobotOverHomeMark();
}

void loop()
{
  showSensorsAndState();
  delay(50);  
}

