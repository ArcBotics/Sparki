#include <Sparki.h>  // Include the sparki library.

const float pi = 3.14159265358979323846;

// Robot + External World data:
const float initialHeading = 90.0; // Intial heading in the current coordinates system: 90 degs = pi/2 [rad]. 

// Robot data:
const int threshold = 500; // Line and edge sensors thereshold [0 - 1015 w/out units].
const int servoDelay = 350; // Minumum time to give to the servo to rotate 90 degrees [ms].
const int robotRadius = 9; // Necessary radius to rotate, from the robot's center [cm].
const int rangerToCentreDistanceFront = 4; // Distance from the measuring edge of the (centered) ultrasonic sensor to the robot's centre [cm].
const int rangerToCentreDistanceSide = 2; // Distance from the measuring edge of the (rotated) ultrasonic sensor to the robot's centre [cm]. Rounded to floor.
const int rangerToFrontDistance = 5; // Distnace from the measuring edge of the (centered) ultrasonic sensor to the gripper's extreme [cm].

// Types:
typedef struct 
{
  int minX, maxX;  // [cm].
  int minY, maxY;  // [cm].
} Room;

typedef struct
{
  int x, y;  // [cm].
} Position;

// Robot variables:
bool  edgeLeft = false,
      lineLeft = false,
      lineCenter = false,
      lineRight = false,
      edgeRight = false;
int ping = 0; // [cm].
String state = "undefined";
float heading = initialHeading; // [degs].
Position pos;
Position home;
int walkedDistanceX = 0; // [cm]
int walkedDistanceY = 0; // [cm]

// Map data: ##
int roomMaxX = 0; // [cm].
int roomMaxY = 0; // [cm].

const int roomsNumber = 3;
Room rooms[roomsNumber]; // Array of room data. The max number of rooms can be changed, of course.
Position doors[roomsNumber*2]; // Array for doors positions. roomsNumber is always bigger than the real possible number of rooms.
Position route[roomsNumber]; // Array for route points.

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

void showRoomData(float value0 = 0.0, float value1 = 0.0)
{
  sparki.clearLCD();

  if (state == "moveTo")
  {
    sparki.print("goToX=");
    sparki.println(value0);
    sparki.print("goToY=");
    sparki.println(value1);
  }
  else // Default:
  {
    sparki.print("roomMaxX=");
    sparki.println(roomMaxX);
    sparki.print("roomMaxY=");
    sparki.println(roomMaxY);
  }

  sparki.print("pos.x=");
  sparki.println(pos.x);
  sparki.print("pos.y=");
  sparki.println(pos.y);

  sparki.print("heading=");
  sparki.println(heading);
  
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

void rotate(float angle)
{
  if (angle > 0.0)
    sparki.moveLeft(angle);
  else
    sparki.moveRight(-angle);

  heading += angle;
}

void measureRoom(bool robotAtHome)
{
  state = "measureRoom";
  showSensorsAndState();
  
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
  pos.x = rangerToCentreDistanceSide + ping;
  showRoomData();

  // Finishes to measure the Y longitude:
  rotate(90); // The robot rotation has more relative error than the servo rotation:
  delay(servoDelay); // Just to stop the robot for a few milliseconds, so it can measure distance.
  ping = sparki.ping(); //"ping" variable is used to show the sensor value on the LCD.
  roomMaxY += rangerToCentreDistanceSide + ping;
  pos.y = rangerToCentreDistanceSide + ping;
  
  // If at home, centers the robot again:
  if (robotAtHome)
  {
    sparki.moveLeft();
    centerRobotOverHomeMark();
    heading = initialHeading; // Special case with external mark centering: the heading goes back to it's intial state.
    sparki.moveStop();
    
    home.x = pos.x;
    home.y = pos.y;
  }
  
  // Leaves the ultrasonic sensor centered:
  sparki.servo(SERVO_CENTER);
  delay(servoDelay);  
}

// Non diagonal (Cartesian) moveTo version. There are no negative possible positions in this coordinates system:
void moveTo(int x, int y)
{
  state = "moveTo";
  showRoomData(x - pos.x, y - pos.y);
 
   //To trask accumulative errors:
  walkedDistanceX += abs(x - pos.x);
  walkedDistanceY += abs(y - pos.y);
  
  // No security or other checks in this first version:
  rotate(-heading); // Rotates the robot to zero heading.
  if ((x - pos.x) > 0)
    sparki.moveForward(x - pos.y);
  else if ((x - pos.x) < 0)
    sparki.moveBackward(pos.x - x);
  pos.x = x;
  showRoomData(x - pos.x, y - pos.y);

  showRoomData(x - pos.x, y - pos.y);
  rotate(90);
  if ((y - pos.y) > 0)
    sparki.moveForward(y - pos.y);
  else if ((y - pos.y) < 0)
    sparki.moveBackward(pos.y - y);
  pos.y = y;
  showRoomData(x - pos.x, y - pos.y);
 }

void beepAndWait(int delayTime = 250)
{
  sparki.beep();
  delay(delayTime);
}

void navigate()
{
  // Uses the global array route[]:
  for (int i=0; i<=roomsNumber; i++)
  {
    moveTo(route[i].x, route[i].y);
    beepAndWait();
    showRoomData();
  }
}

int getRoom(int x, int y)
{
  for (int i=0; i<=roomsNumber; i++)
  {
    if ( (rooms[i].minX <= x) && (x <= rooms[i].maxX) && (rooms[i].minY <= y) && (y <= rooms[i].maxY) )
      return i;
  }
  return -1; //Error: point does not belong to any existing room.
}

// Give a position, returns the route to it (points including doors) in the route[] global array:
void getRoute(int x, int y)
{
  int currentRoom = getRoom(pos.x, pos.y);
  int targetRoom = getRoom(x, y);
  
  //##
}

void setup()
{
  /* ##
  for (int i=0; i<roomsNumber*2; i++)
  {
    doors[i].x = 0;
    doors[i].y = 0;
  }
  */
  
  pos.x = 0;
  pos.y = 0;
  home.x = 0;
  home.y = 0;
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

