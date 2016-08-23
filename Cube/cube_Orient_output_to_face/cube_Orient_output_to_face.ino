#define RED 1
#define GREEN 2
#define BLUE 3
#define PURPLE 4

#define X 0
#define Y 1
#define Z 2
#define ANGLE 15

#define A 2  // defined for mapping, just place holders
#define B 3

#include "CurieIMU.h"

short colour = BLUE;  //**** define colour ***

short unimpressedPin = 19;
short perseveringPin = 18;

int motionResetTimer = 0;
boolean perseveringfaceDispaly = false;
boolean unimpressedfaceDisplay = false;

double acc[3];         // accelerometer values
double accRaw[3];

// x, Y, Z
short stateOrient[2][2][2] = {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};
short Buffer[3][3] = {{0, 0, 0}, {0, 0, 0}, {90, 90, 90}};
short orient = 0;

short ORIENTLEDMAP[6][3] = {{0, A, B}, {1, A, B}, {A, 1, B}, {A, 0, B}, {A, B, 0}, {A, B, 1}};
short orientledmap[6][3] = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}};
short withinAngle;

int axRaw, ayRaw, azRaw;         // accelerometer values
int gxRaw, gyRaw, gzRaw;

short ledLayer[2][2][3] = {{{11, 13, 12}, {10, 8, 9}}, {{3, 5, 4}, {14, 16, 15}}};
short ledZ[2] = {7, 6};
short RATIO[3] = {3, 6, 1};

short rgb[3] = {255, 0, 0}; // setting red component to 255 fixes a bug, no idea why?

short state[2][2][2][4] = {
  { {{255, 0, 0, 1}, {255, 255, 0, 1}},
    {{255, 0, 255, 1}, {0, 255, 0, 1}}
  },

  { {{0, 0, 255, 1}, {0, 255, 255, 1}},
    {{126, 200, 0, 1}, {60, 0, 120, 1}}
  }
};

void setup ()
{
  for (short k = 0; k < 2; k++)
  {
    for (short d = 0; d < 2; d++)
    {
      for (short r = 0; r < 3; r++)
      {
        pinMode(ledLayer[k][d][r], OUTPUT);
        digitalWrite(ledLayer[k][d][r], LOW);
      }
    }
  }
  pinMode(ledZ[0], OUTPUT);
  digitalWrite(ledZ[0], HIGH);
  pinMode(ledZ[1], OUTPUT);
  digitalWrite(ledZ[1], HIGH);

  pinMode(unimpressedPin, OUTPUT);
  pinMode(perseveringPin, OUTPUT);


  CurieIMU.begin();
  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);

  CurieIMU.attachInterrupt(eventCallback);

  /* Enable Motion Detection */
  CurieIMU.setDetectionThreshold(CURIE_IMU_MOTION, 10); // 20mg
  CurieIMU.setDetectionDuration(CURIE_IMU_MOTION, 5);  // trigger times of consecutive slope data points
  CurieIMU.interrupts(CURIE_IMU_MOTION);
}

void loop()
{
  for (short k = 0; k < 2; k++)
  {
    for (short d = 0; d < 2; d++)
    {
      for (short r = 0; r < 2; r++)
      {
        light(k, d, r, state[k][d][r]);
      }
    }
  }

  CurieIMU.readMotionSensor(axRaw, ayRaw, azRaw, gxRaw, gyRaw, gzRaw);
          accRaw[X] = axRaw;
          accRaw[Y] = ayRaw;
          accRaw[Z] = azRaw;

          convertRawAcceleration();
          pickColour(colour);
          lightOrient();

  if(millis() - motionResetTimer > 1000 && unimpressedfaceDisplay == true)
  {
    digitalWrite(unimpressedPin, LOW);
    unimpressedfaceDisplay = false;
  }
}

short pickColour(int COLOUR)
{
  switch (COLOUR)
  {
    case RED:
      rgb[0] = 255;
      rgb[1] = 0;
      rgb[2] = 0;
      break;

    case GREEN:
      rgb[0] = 0;
      rgb[1] = 255;
      rgb[2] = 0;
      break;

    case BLUE:
      rgb[0] = 0;
      rgb[1] = 0;
      rgb[2] = 255;
      break;

    case PURPLE:
      rgb[0] = 255;
      rgb[1] = 0;
      rgb[2] = 255;
      break;

    default:
      rgb[0] = 0;
      rgb[1] = 0;
      rgb[2] = 0;
      break;
  }
}

short convertRawAcceleration()
{
  for (short h = 0; h < 3; h++)
  {
    if (16410 - abs(accRaw[h]) < 0)
    {
      acc[h] = (Buffer[h][0] + Buffer[h][1] + Buffer[h][2]) / 3;
    }
    else
    {
      acc[h] = (asin(accRaw[h] / 16410) / 3.14159265354) * 180;
      Buffer[h][2] = Buffer[h][1];
      Buffer[h][1] = Buffer[h][0];
      Buffer[h][0] = acc[h];
    }
  }
}

short light(short x, short y, short z, short Data[4]) //3 rgb, 1 state, (4 as the last bit of data is whether the led is on or not)
{
  short totalTime = 0; //used to record the total time the function has taken to run
  if (Data[3] == 1) // check if the led is suppossed to be on (having it as part of the rgb makes it easier to keep all the info together
  {
    digitalWrite(ledZ[z], LOW);
    for (short i = 0; i < 3; i++)
    {
      short Delay = (Data[i] * 5) / RATIO[i];
      totalTime += Delay;
      digitalWrite(ledLayer[x][y][i], HIGH);
      delayMicroseconds(Delay);
      digitalWrite(ledLayer[x][y][i], LOW);
    }
    digitalWrite(ledZ[z], HIGH);
  }
  delayMicroseconds(2000 - totalTime); // standardise the time it takes to light an led so the intensity remains constant whe lighting multiple leds
}

short lightOrient()
{
  for (short i = 0; i < 2; i++)
  {
    for (short j = 0; j < 2; j++)
    {
      for (short k = 0; k < 2; k++)
      {
        stateOrient[i][j][k] = 0;
      }
    }
  }
  
  for (short r = 0; r < 3; r++)
  {
    withinAngle = -1;
    if (acc[r] - ANGLE > 0)
    {
      withinAngle = 0;

      if(r == Z && perseveringfaceDispaly == true)
      {
        digitalWrite(perseveringPin, LOW);  // signal the board is right way up
        perseveringfaceDispaly = false;
      }
    }
    else if (acc[r] + ANGLE < 0)
    {
      withinAngle = 1;

      if(r == Z && perseveringfaceDispaly == false)
      {
        digitalWrite(perseveringPin, HIGH);  // signal the board is upside-down
        perseveringfaceDispaly = true;
      }
    }

    if (withinAngle != -1)
    {
      for (short i = 0; i < 2; i++)
      {
        for (short j = 0; j < 2; j++)
        {
          for (short g = 0; g < 3; g++)
          {
            if (ORIENTLEDMAP[(r*2) + withinAngle][g] == A)
            {
              orientledmap[(r*2) + withinAngle][g] = i;
            }
            else if (ORIENTLEDMAP[(r*2) + withinAngle][g] == B)
            {
              orientledmap[(r*2) + withinAngle][g] = j;
            }
          }
          stateOrient[orientledmap[(r*2) + withinAngle][0]][orientledmap[(r*2) + withinAngle][1]][orientledmap[(r*2) + withinAngle][2]] += 1;
        }
      }
    }
  }
  
  orient = 0;

  for (short i = 0; i < 2; i++)
  {
    for (short j = 0; j < 2; j++)
    {
      for (short k = 0; k < 2; k++)
      {
        if (stateOrient[i][j][k] > orient)
        {
          orient = stateOrient[i][j][k];
        }
      }
    }
  }

  for (short i = 0; i < 2; i++)
  {
    for (short j = 0; j < 2; j++)
    {
      for (short k = 0; k < 2; k++)
      {
        if (stateOrient[i][j][k] == orient)
        {
          for (short d = 0; d < 3; d++)
          {
            state[i][j][k][d] = rgb[d];
          }
          state[i][j][k][3] = 1;
        }
        else
        {
          state[i][j][k][3] = 0;
        }
      }
    }
  }
}

static void eventCallback(void)
{
  if (unimpressedfaceDisplay == false)
  {
    digitalWrite(unimpressedPin, HIGH);
    unimpressedfaceDisplay = true;
  }
  motionResetTimer = millis();
}

