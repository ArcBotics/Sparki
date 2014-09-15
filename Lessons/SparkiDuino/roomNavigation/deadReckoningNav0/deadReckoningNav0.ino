#include <Sparki.h>  // Include the sparki library.

const float pi = 3.14159265358979323846;

// Robot + External World data:
const float initialHeading = 90.0; // Intial heading in the current coordinates system: 90 degs = pi/2 [rad]. 

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

// Robot data:
const int threshold = 500; // Line and edge sensors thereshold [0 - 1015 w/out units].
const int servoDelay = 350; // Minumum time to give to the servo to rotate 90 degrees [ms].
const int robotRadius = 9; // Necessary radius to rotate, from the robot's center [cm].
const int rangerToCentreDistanceFront = 4; // Distance from the measuring edge of the (centered) ultrasonic sensor to the robot's centre [cm].
const int rangerToCentreDistanceSide = 2; // Distance from the measuring edge of the (rotated) ultrasonic sensor to the robot's centre [cm]. Rounded to floor.
const int rangerToFrontDistance = 5; // Distnace from the measuring edge of the (centered) ultrasonic sensor to the gripper's extreme [cm].

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
int walkedDistanceX = 0; // [cm]
int walkedDistanceY = 0; // [cm]

//Map:
const int roomsNumber = 3;
Position home;
Room rooms[roomsNumber]; // Array of room data. The max number of rooms can be changed, of course.
Position doors[roomsNumber*2]; // Array for doors positions. roomsNumber*2 is always bigger than the real possible number of rooms.
int connectedRooms[roomsNumber][roomsNumber]; // This array is a table which stores the door number that connects each pair of rooms.

//Route results:
int roomsRoute[roomsNumber]; //Array to stoer a room numbers route.
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

void showData(float value0 = 0.0, float value1 = 0.0)
{
  sparki.clearLCD();

  if (state == "moveTo")
  {
    sparki.print("goToX=");
    sparki.println(value0);
    sparki.print("goToY=");
    sparki.println(value1);
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

// Non diagonal (Cartesian) moveTo version. There are no negative possible positions in this coordinates system:
void moveTo(int x, int y)
{
  state = "moveTo";
  showData(x - pos.x, y - pos.y);
 
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
  showData(x - pos.x, y - pos.y);

  showData(x - pos.x, y - pos.y);
  rotate(90);
  if ((y - pos.y) > 0)
    sparki.moveForward(y - pos.y);
  else if ((y - pos.y) < 0)
    sparki.moveBackward(pos.y - y);
  pos.y = y;
  showData(x - pos.x, y - pos.y);
 }

void beepAndWait(int delayTime = 250)
{
  sparki.beep();
  delay(delayTime);
}

int getRoom(int x, int y)
{
  for (int i=0; i < roomsNumber; i++)
  {
    if ( (rooms[i].minX <= x) && (x <= rooms[i].maxX) && (rooms[i].minY <= y) && (y <= rooms[i].maxY) )
      return i;
  }
  return -1; //Error: point does not belong to any existing room.
}

void getRoomsRoute(int sourceRoom, int destRoom)
{
  //The result of this function is stored in the roomsRoute[] array:
  for (int i=0; i < roomsNumber; i++)
  {
    roomsRoute[i] = //##
  }  
}

// Given a position, returns the route to it (points including doors) in the route[] global array:
void getRoute(int x, int y)
{
  //Resets the result array:
  int i = 0;
  for (i=0; i < roomsNumber; i++)
  {
    route[i].x = -1; //Invalid position to indicate that the robot should not move.
    route[i].y = -1; //Invalid position to indicate that the robot should not move.
  }
  
  //Find the rooms where the robot needs to go to reach the goal:
  int currentRoom = getRoom(pos.x, pos.y);
  int destRoom = getRoom(x, y);
  getRoomsRoute(currentRoom, destRoom);
  
  int doorNumber = 0;
  for (i=currentRoom; i <= destRoom; i++)
  {
    doorNumber = connectedRooms[roomsRoute[i]][roomsRoute[i+1]];
    route[i-currentRoom].x = doors[doorNumber].x; //route[] is zero based: that's why using "i-currentRoom".
    route[i-currentRoom].y = doors[doorNumber].y;
  }
}

void navigate(int x, int y)
{
  // Calculates the route:
  getRoute(x, y);
  
  //Moves through the calculated route (stored in the route[] array):
  Position tempPos;
  for (int i=0; i < roomsNumber; i++)
  {
    tempPos.x = route[i].x;
    tempPos.y = route[i].y;
    if ( (tempPos.x>0) && (tempPos.y>0) ) //Invalid positions in the route[] array are marked with x=-1 and y=-1.
    {
      moveTo(tempPos.x, tempPos.y);
      beepAndWait();
      showData();
    }
  }
}

void setup()
{
  // Initialize the map (this has to be done by the user):
  // Rooms:
  rooms[0].minX = 0; rooms[0].minY = 0;
  rooms[0].maxX = 45; rooms[0].maxY = 31;

  rooms[1].minX = 0; rooms[1].minY = 31;
  rooms[1].maxX = 45; rooms[1].maxY = 63;

  rooms[2].minX = 45; rooms[2].minY = 0;
  rooms[2].maxX = 90; rooms[2].maxY = 63;

  //Doors (each array entry contains the center of a door):
  doors[0].x = 25; doors[0].y = 31;
  doors[1].x = 45; doors[1].y = 46;
  
  //Connected rooms (these data can be calculated by the program in the future):
  connectedRooms[0][0] = -1; // A room does not have a door to connect with itself.
  connectedRooms[0][1] = 0; // door[0] connects room[0] with room[1].
  connectedRooms[0][2] = -1; //No door connection between room[0] and room[2].

  connectedRooms[1][0] = 0;
  connectedRooms[1][1] = -1;
  connectedRooms[1][2] = 1; //door[1] connects room[1] with room[2].

  connectedRooms[2][0] = -1;
  connectedRooms[2][1] = 1;
  connectedRooms[2][2] = -1;

  //Home:
  home.x = 10; home.y = 10;

  //Initializes the robot:  
  pos.x = 0;
  pos.y = 0;
  sparki.servo(SERVO_CENTER);
  centerRobotOverHomeMark();
  delay(1500); // Give time to the human to take her/his hands off.
  beepAndWait();
  
  //User application:
  //##Document these tests in the webpage and delete these comments...
  //##Test 1: moving the robot to the doors.
  moveTo(doors[0].x, doors[0].y);
  moveTo(doors[1].x, doors[1].y);

  //##Test 2: finding the rooms for different points:
  //##  
}

void loop()
{
  ping = sparki.ping(); //update the ultrasonic sensor data to be displayed.
  showData();
  delay(50);
}

