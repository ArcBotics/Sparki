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
bool connectedRooms[roomsNumber][roomsNumber]; // Same as the number of doors.

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
  for (int i=0; i < roomsNumber*2; i++)
  {
    //##roomsRoute[i]
  }  
}

// Give a position, returns the route to it (points including doors) in the route[] global array:
void getRoute(int x, int y)
{
  //Find the rooms where the robot nees to go to reach the goal:
  int currentRoom = getRoom(pos.x, pos.y);
  int destRoom = getRoom(x, y);
  getRoomsRoute(currentRoom, destRoom);
  
  for (int i=currentRoom; i < roomsNumber-1; i++)
  {
    //##roomsRoute[i]
  }  
}

void navigate()
{
  // Uses the global array route[]:
  for (int i=0; i < roomsNumber; i++)
  {
    moveTo(route[i].x, route[i].y);
    beepAndWait();
    showData();
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
  connectedRooms[0][0] = true; // A room is always connectes with itself.
  connectedRooms[0][1] = true;
  connectedRooms[0][2] = false;

  connectedRooms[1][0] = true;
  connectedRooms[1][1] = true;
  connectedRooms[1][2] = true;

  connectedRooms[2][0] = false;
  connectedRooms[2][1] = true;
  connectedRooms[2][2] = true;

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
  //##.
}

void loop()
{
  ping = sparki.ping(); //update the ultrasonic sensor data to be displayed.
  showData();
  delay(50);
}

