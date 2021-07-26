#include<Servo.h>


// constants in caps

const byte NUMBER_OF_PUSH_BUTTONS = 4;
const byte NUMBER_OF_ROUTE_BUTTONS = 2;
const byte NUMBER_OF_TURNOUTS = 5;
#define TURNOUT_MOVE_SPEED 30              // [ms] lower number is faster

struct ServoTurnout
{
  int pin;
  int closedPosition;
  int thrownPosition;
  Servo servo;
  int targetPosition;  // renamed from turnoutTarget
  int currentPosition; // renamed from turnoutPosition
  unsigned long moveDelay;
  bool isClosed;

  void setup()
  {
    targetPosition = closedPosition;       // setup targets as closed positiion
    currentPosition = closedPosition;      // setup position to closed
    isClosed = true;                       // setup closed is true

    servo.attach(pin);                     // attach all the turnout servos
    servo.write(currentPosition);          // write the servos poistions
  }
};

// define the turnouts with their pins and end position angles.
ServoTurnout turnouts[NUMBER_OF_TURNOUTS] = {
  {2, 50, 90},
  {3, 80, 150},
  {4, 60, 120},
  {5, 60, 120},
  {6, 60, 120}
};

/* Setup servo arrays  */
int pushButtonPins[NUMBER_OF_PUSH_BUTTONS] = {7, 8, 9, 10}; // enter pin numbers. 1st number is for pushButtonPins[0], 2nd for pushButtonPins[1] etc
int routeButtonPins[NUMBER_OF_ROUTE_BUTTONS] = {11, 12};    // enter pin numbers. 1st number is for routeButtonPins[0], 2nd for routeButtonPins[1] etc

void throwTurnout(int i){
  turnouts[i].moveDelay = millis() + TURNOUT_MOVE_SPEED;
  if (turnouts[i].currentPosition < turnouts[i].targetPosition) turnouts[i].currentPosition++;
  if (turnouts[i].currentPosition > turnouts[i].targetPosition) turnouts[i].currentPosition--;
  turnouts[i].servo.write(turnouts[i].currentPosition);
  if (turnouts[i].currentPosition == turnouts[i].closedPosition) {
    turnouts[i].isClosed = true;
    if (i == 3) turnouts[i+1].targetPosition = turnouts[i+1].closedPosition;
  }
  if (turnouts[i].currentPosition == turnouts[i].thrownPosition) {
    turnouts[i].isClosed = false;
    if (i == 3) turnouts[i+1].targetPosition = turnouts[i+1].thrownPosition;
  }
}

void setup() {
  Serial.begin(9600);
  /* setup all the servos*/
  for (int thisTurnout = 0; thisTurnout < NUMBER_OF_TURNOUTS; thisTurnout++) {
      turnouts[thisTurnout].setup();
  }
  
  /* setup all the pushbuttons */
  for (int i = 0; i < NUMBER_OF_PUSH_BUTTONS;  i++) pinMode(pushButtonPins[i], INPUT_PULLUP);  // initailise all the push buttons with pullups 
  for (int i = 0; i < NUMBER_OF_ROUTE_BUTTONS; i++) pinMode(routeButtonPins[i], INPUT_PULLUP); // initailise all the route buttons with pullups 

}

void loop() {
  /* loop through all the push buttons */
  for (int i = 0; i < NUMBER_OF_PUSH_BUTTONS; i++){
    if (digitalRead(pushButtonPins[i]) == LOW){                                           // is push button pressed
      if (turnouts[i].isClosed)  turnouts[i].targetPosition = turnouts[i].thrownPosition; // set turnout target to thrown if it is closed
      if (!turnouts[i].isClosed) turnouts[i].targetPosition = turnouts[i].closedPosition; // set turnout target to closed if it is thrown
    }
  }
  
  /* read route buttons and move turnouts when pressed */
  if (digitalRead(routeButtonPins[0]) == LOW){                                          // is route button pressed
    if (!turnouts[0].isClosed) turnouts[0].targetPosition = turnouts[0].closedPosition; // set turnout target to closed if it is thrown
    if (!turnouts[1].isClosed) turnouts[1].targetPosition = turnouts[1].closedPosition; // set turnout target to closed if it is thrown
  }
  if (digitalRead(routeButtonPins[1]) == LOW){                                          // is route button pressed
    if (turnouts[0].isClosed) turnouts[0].targetPosition = turnouts[0].thrownPosition;  // set turnout target to thrown if it is thrown
    if (turnouts[1].isClosed) turnouts[1].targetPosition = turnouts[1].thrownPosition;  // set turnout target to thrown if it is thrown
  }

  
  /* loop through the turnout servos */
  for (int thisTurnout = 0; thisTurnout < NUMBER_OF_TURNOUTS; thisTurnout++) {
    if (turnouts[thisTurnout].currentPosition != turnouts[thisTurnout].targetPosition) { // check that they are not at there taregt position
      if (millis() > turnouts[thisTurnout].moveDelay) {                                  // check that enough delay has passed
           throwTurnout(thisTurnout);                                                    // call throwTurnout to move the servo 1 degree towards its target
      }
    }
  }
}
