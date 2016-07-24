#include <EEPROM.h>
#include <Servo.h>


//enums
int _system = 20, laser = 21, C0 = 0, C1 = 1, C2 = 2, S0 = 10, S1 = 11, S2 = 12;
int _delay = 0, _move = 1, _moveFast = 2,
    _attach = 3, _detach = 4,
    _setIntensity = 5, _setStepDelay = 6, _setStepDelayS = 7;

Servo C[3];
Servo S[3];
int lastPosC[3];
int lastPosS[3];
long stepDelay = 2000;
long stepDelayS = 500000;

int laserPin = 6;

int mapC[3][2] =
{
  {20, 125},
  {6, 180},
  {180, -10}
};
int mapS[3][2] =
{
  {0, 180},
  {0, 180},
  {0, 180}
};


void setup() {
  Serial.begin(9600);

  pinMode(laserPin, OUTPUT);

  Serial.read();
  Serial.print("rdy");

}

void loop() {
  int element, command, values[4];

  do {
    element = Serial.read();
  } while (element == -1);

  do {
    command = Serial.read();
  } while (command == -1);

  for (int i = 0 ; i < 4 ; i++) {
    do {
      values[i] = Serial.read();
    } while (values[i] == -1);
  }

  Serial.print("ack");

  int value = 0;
  for (int i = 0 ; i < 4 ; i++) {
    value = value << 8;
    value += values[i];
  }

  handleCommand(element, command, value);
}

void handleCommand(int element, int command, int value) {

  if (element == _system) {
    handleSystem(command, value);

  } else if (element == laser) {
    handleLaser(command, value);

  } else if (element >= 0 && element <= 2) {
    handleServoC(element, command, value);

  } else if (element == S0) {
    handleStepper(command, value);

  } else if (element == S1) {
    handleServoS(1, command, value);

  } else if (element == S2) {
    handleServoS(2, command, value);

  }
}

void handleLaser(int command, int value) {
  if (command == _setIntensity) {
    analogWrite(laserPin, value);
  }
}

void handleSystem(int command, int value) {
  if (command == _delay) {
    delay(value);

  } else if (command == _detach) {
    C[0].detach();
    C[1].detach();
    C[2].detach();
    S[1].detach();
    S[2].detach();
    //TODO add stepper S0

  } else if (command == _attach) {
    //C
    C[0].attach(9);
    C[1].attach(10);
    C[2].attach(11);
    mCFast(0, 30);
    mCFast(1, 0);
    mCFast(2, 0);
    //S
    S[1].attach(3);
    S[2].attach(5);
    mS(1, 30);
    mS(2, 90);
    
    //TODO add stepper S0
  } else if (command == _setStepDelay ) {
    stepDelay = value;
  } else if (command == _setStepDelayS ) {
    stepDelayS = value;
  }
}
void handleServoC(int which, int command, int value) {
  if (command == _move) {
    mC(which, value);
  } else if (command == _moveFast) {
    mCFast(which, value);
  }

}
void handleServoS(int which, int command, int value) {
  if (command == _move) {
    mS(which, value);
  } else if (command == _moveFast) {
    mSFast(which, value);
  }

}
void handleStepper(int command, int value) {
  //TODO
}

void mSFast(int which, int degree) {
  degree = mapDegreeS(which, degree);

  S[which].write(degree);
  lastPosS[which] = degree;
}

void mS(int which, int degree) {
  degree = mapDegreeS(which, degree);

  for (int pos = lastPosS[which]; pos < degree; pos++) {
    S[which].write(pos);
    delay(5);
//    delayMicroseconds(stepDelayS);
  }
  for (int pos = lastPosS[which]; pos > degree; pos--) {
    S[which].write(pos);
    delay(20);
//    delayMicroseconds(stepDelayS);
  }
  lastPosS[which] = degree;
}

void mCFast(int which, int degree) {
  degree = mapDegree(which, degree);

  C[which].write(degree);
  lastPosC[which] = degree;
}

void mC(int which, int degree) {
  degree = mapDegree(which, degree);

  for (int pos = lastPosC[which]; pos < degree; pos++) {
    C[which].write(pos);
    delayMicroseconds(stepDelay);
  }
  for (int pos = lastPosC[which]; pos > degree; pos--) {
    C[which].write(pos);
    delayMicroseconds(stepDelay);
  }
  lastPosC[which] = degree;
}

int mapDegree(int which, int degree) {
  degree = constrain(degree, 0, 180);
  return map(degree, 0, 180, mapC[which][0], mapC[which][1]);
}
int mapDegreeS(int which, int degree) {
  degree = constrain(degree, 0, 180);
  return map(degree, 0, 180, mapS[which][0], mapS[which][1]);
}

