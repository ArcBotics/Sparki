#include <Sparki.h>  // Include the sparki library.

// Robot data:
const int threshold = 500; // Line and edge sensors thereshold [0 - 1015 w/out units].
const int servoDelay = 350; // Minumum time to give to the servo to rotate 90 degrees [ms].
const int rangerToCentreDistanceFront = 4; // Distance from the measuring edge of the (centered) ultrasonic sensor to the robot's centre [cm].
const int rangerToCentreDistanceSide = 2; // Distance from the measuring edge of the (rotated) ultrasonic sensor to the robot's centre [cm]. Rounded to floor.

// Robot variables:
bool  edgeLeft = false,
      lineLeft = false,
      lineCenter = false,
      lineRight = false,
      edgeRight = false;
int ping = 0; // [cm].
String state = "undefined";

// Map variables:
int roomMaxX = 0; // [cm].
int roomMaxY = 0; // [cm].


void printPingData()
{
  sparki.print("Ping="); // Ultrasonic ranger on screen.
  sparki.print(ping);
  sparki.println(" cm"); 
}

void showSensorsAndState()
{
  sparki.clearLCD();

  sparki.print("edgeL=");
  sparki.println(edgeLeft);
  sparki.print("lineL=");
  sparki.println(lineLeft);
  sparki.print("lineC=");
  sparki.println(lineCenter);
  sparki.print("lineR=");
  sparki.println(lineRight);
  sparki.print("edgeR=");
  sparki.println(edgeRight);  

  printPingData();
  
  sparki.println(String("state=") + state);

  sparki.updateLCD();
}

void showRoomData()
{
  sparki.clearLCD();

  sparki.print("roomMaxX=");
  sparki.println(roomMaxX);

  sparki.print("roomMaxY=");
  sparki.println(roomMaxY);
  
  printPingData();
  
  sparki.println(String("state=") + state);

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
  showSensorsAndState();
}

void measureRoom(bool robotAtHome)
{
  // Starts to measure the Y longitude: 
  sparki.servo(SERVO_CENTER);
  delay(servoDelay);
  ping = sparki.ping(); //"ping" variable is used to show the sensor value on the LCD.
  roomMaxY = rangerToCentreDistanceFront + ping;
  showRoomData();

  // Measures the X longitude: 
  sparki.servo(SERVO_RIGHT);
  delay(servoDelay);
  ping = sparki.ping();
  roomMaxX = rangerToCentreDistanceSide + ping;
  showRoomData();

  sparki.servo(SERVO_LEFT);
  delay(2*servoDelay); // Twice the time of 1/4 of revolution rotation.
  ping = sparki.ping();
  roomMaxX += rangerToCentreDistanceSide + ping;
  showRoomData();

  // Finishes to measure the Y longitude:
  sparki.moveLeft(90); // The robot rotation has more relative error than the servo rotation:
  delay(servoDelay); // Just to stop the robot for a few milliseconds, so it can measure distance.
  ping = sparki.ping(); //"ping" variable is used to show the sensor value on the LCD.
  roomMaxY += rangerToCentreDistanceSide + ping;
  
  // If at home, centers the robot again:
  if (robotAtHome)
  {
    sparki.moveLeft();
    centerRobotOverHomeMark();
    sparki.moveStop();
  }
  
  // Leaves the ultrasonic sensor centered:
  sparki.servo(SERVO_CENTER);
  delay(servoDelay);  
}

void setup()
{
  centerRobotOverHomeMark();
  delay(1500); // Give time to the human to take her/his hands off.
  measureRoom(true);
}

void loop()
{
  ping = sparki.ping(); //update the ultrasonic sensor data to be displayed.
  showRoomData();
  delay(50);
}

