#define RED 1
#define GREEN 2
#define BLUE 3
#define PURPLE 4

#define X 0
#define Y 1
#define Z 2
#define ANGLE 22

#define A 2  // defined for mapping, just place holders
#define B 3


#include "CurieIMU.h"


double acc[3];         // accelerometer values
double accRaw[3];

// x, Y, Z
short stateOrient[2][2][2] = {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};
short Buffer[3][3] = {{0, 0, 0}, {0, 0, 0}, {90, 90, 90}};
short orient = 0;

short ORIENTLEDMAP[6][3] = {{0, A, B}, {1, A, B}, {A, 1, B}, {A, 0, B}, {A, B, 0}, {A, B, 1}};
short orientledmap[6][3] = {{0, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 1}};
short flip;

int axRaw, ayRaw, azRaw;         // accelerometer values
int gxRaw, gyRaw, gzRaw;

short ledLayer[2][2][3] = {{{11, 13, 12}, {10, 8, 9}}, {{3, 5, 4}, {14, 16, 15}}};
short ledZ[2] = {7, 6};
short RATIO[3] = {3, 6, 1};

short colourCycleEnd;
short colourCycle = 0;
int timer = 0;

short DELAY = 0;
short repeat = 0;
short repeatEnd = 0;

String pattern = "";
String StringColour = "";
short colour = 0;
short patternCycleEnd;
short patternCycle = 0;

short rgb[3] = {255, 0, 0}; // setting red component to 255 fixes a bug, no idea why?


short testData[] = {0, -1};
short test[5] = {1, 1, 1, 1, 0};

//                 delay, pattern cycle end,
short twistData[] = {50   , 32               };
short twist[32][5] = {
  //X,Y, Z, ON/Off, DELAY / NO DELAY
  {0, 0, 0, 1, 1}, {0, 1, 0, 1, 1}, {1, 1, 0, 1, 1}, {1, 0, 0, 1, 0},
  {0, 0, 0, 0, 1}, {0, 1, 0, 0, 1}, {1, 1, 0, 0, 1}, {1, 0, 0, 0, 0},
  {0, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {1, 0, 1, 1, 1}, {0, 0, 1, 1, 0},
  {0, 1, 1, 0, 1}, {1, 1, 1, 0, 1}, {1, 0, 1, 0, 1}, {0, 0, 1, 0, 0},

  {1, 1, 0, 1, 1}, {1, 0, 0, 1, 1}, {0, 0, 0, 1, 1}, {0, 1, 0, 1, 0},
  {1, 1, 0, 0, 1}, {1, 0, 0, 0, 1}, {0, 0, 0, 0, 1}, {0, 1, 0, 0, 0},
  {1, 0, 1, 1, 1}, {0, 0, 1, 1, 1}, {0, 1, 1, 1, 1}, {1, 1, 1, 1, 0},
  {1, 0, 1, 0, 1}, {0, 0, 1, 0, 1}, {0, 1, 1, 0, 1}, {1, 1, 1, 0, 0}
};



short dropData[] = { 100, 32};
short drop[32][5] = {
  //X,Y, Z, ON/Off, DELAY / NO DELAY
  {0, 0, 1, 1, 1}, {0, 0, 1, 0, 0}, {0, 0, 0, 1, 1}, {0, 0, 0, 0, 0},
  {1, 0, 1, 1, 1}, {1, 0, 1, 0, 0}, {1, 0, 0, 1, 1}, {1, 0, 0, 0, 0},
  {0, 1, 1, 1, 1}, {0, 1, 1, 0, 0}, {0, 1, 0, 1, 1}, {0, 1, 0, 0, 0},
  {1, 1, 1, 1, 1}, {1, 1, 1, 0, 0}, {1, 1, 0, 1, 1}, {1, 1, 0, 0, 0},

  {0, 0, 1, 1, 1}, {0, 0, 1, 0, 0}, {0, 0, 0, 1, 1}, {0, 0, 0, 0, 0},
  {1, 0, 1, 1, 1}, {1, 0, 1, 0, 0}, {1, 0, 0, 1, 1}, {1, 0, 0, 0, 0},
  {0, 1, 1, 1, 1}, {0, 1, 1, 0, 0}, {0, 1, 0, 1, 1}, {0, 1, 0, 0, 0},
  {1, 1, 1, 1, 1}, {1, 1, 1, 0, 0}, {1, 1, 0, 1, 1}, {1, 1, 0, 0, 0}
};

short diagonalData[] = { 200, 32};
short diagonal[32][5] = {
  //X,Y, Z, ON/Off, DELAY / NO DELAY
  {0, 0, 0, 1, 0}, {0, 1, 0, 1, 1}, {0, 0, 0, 0, 0}, {0, 1, 0, 0, 0},
  {1, 0, 1, 1, 0}, {1, 1, 1, 1, 1}, {1, 0, 1, 0, 0}, {1, 1, 1, 0, 0},
  {0, 1, 0, 1, 0}, {1, 1, 0, 1, 1}, {0, 1, 0, 0, 0}, {1, 1, 0, 0, 0},
  {0, 0, 1, 1, 0}, {1, 0, 1, 1, 1}, {0, 0, 1, 0, 0}, {1, 0, 1, 0, 0},

  {1, 1, 0, 1, 0}, {1, 0, 0, 1, 1}, {1, 1, 0, 0, 0}, {1, 0, 0, 0, 0},
  {0, 1, 1, 1, 0}, {0, 0, 1, 1, 1}, {0, 1, 1, 0, 0}, {0, 0, 1, 0, 0},
  {1, 0, 0, 1, 0}, {0, 0, 0, 1, 1}, {1, 0, 0, 0, 0}, {0, 0, 0, 0, 0},
  {1, 1, 1, 1, 0}, {0, 1, 1, 1, 1}, {1, 1, 1, 0, 0}, {0, 1, 1, 0, 0}
};


short state[2][2][2][4] = {
  { {{255, 0, 0, 1}, {255, 255, 0, 1}},
    {{255, 0, 255, 1}, {0, 255, 0, 1}}
  },

  { {{0, 0, 255, 1}, {0, 255, 255, 1}},
    {{126, 200, 0, 1}, {60, 0, 120, 1}}
  }
};


void setup()
{

  Serial.begin(9600);
  Serial.setTimeout(100);
  while (!Serial);
  Serial.println("pattern,colour (default rainbow),repeat times (default 1)  (!!! NO SPACES!!!)");
  Serial.println("patterns: twist, drop, diagonal");
  Serial.println("colours: red, green, blue, purple, rainbow (default)");
  Serial.println("times: 1 - infinity (-1 = infinity)");

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


  CurieIMU.begin();
  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
  /* Initialise the IMU */
  CurieIMU.attachInterrupt(eventCallback);
  /* Enable Shock Detection */
  CurieIMU.setDetectionThreshold(CURIE_IMU_SHOCK, 1500); // 1.5g = 1500 mg
  CurieIMU.setDetectionDuration(CURIE_IMU_SHOCK, 50);   // 50ms
  CurieIMU.interrupts(CURIE_IMU_SHOCK);

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

  if (Serial.available() > 0)
  {
    pattern = Serial.readStringUntil(',');
    Serial.print(pattern); Serial.print(" : ");
    StringColour = Serial.readStringUntil(',');
    if (StringColour == "red")
    {
      colour = RED;
    }
    else if (StringColour == "green")
    {
      colour = GREEN;
    }
    else if (StringColour == "blue")
    {
      colour = BLUE;
    }
    else if (StringColour == "purple")
    {
      colour = PURPLE;
    }
    else
    {
      colour = 0;
      StringColour = "rainbow";
    }

    Serial.print(StringColour); Serial.print(" : ");
    repeatEnd = Serial.parseInt(); Serial.flush();
    if (repeatEnd == 0)
    {
      repeatEnd = 1;
    }
    Serial.println(repeatEnd);

    repeat = 0;
    patternCycle = 0;

    if (pattern == "twist")
    {
      DELAY = twistData[0];
      patternCycleEnd = twistData[1];
    }

    if (pattern == "drop")
    {
      DELAY = dropData[0];
      patternCycleEnd = dropData[1];
    }

    if (pattern == "diagonal")
    {
      DELAY = diagonalData[0];
      patternCycleEnd = diagonalData[1];
    }

    if (pattern == "test")
    {
      DELAY = test[0];
      patternCycleEnd = testData[1];
    }

    if (pattern == "orient")
    {
      repeatEnd = -1;
      patternCycleEnd = -1;
      DELAY = 0;
    }

    for (short e = 0; e < 2; e++)
    {
      for (short w = 0; w < 2; w++)
      {
        for (short n = 0; n < 2; n++)
        {
          for (short l = 0; l < 4; l++)
          {
            state[e][w][n][l] = 0;
          }
        }
      }
    }
    while (Serial.available() > 0)
    {
      char c = Serial.read();
      Serial.print("c");
    }
  }

  if (millis() - timer > DELAY)
  {
    if (repeat >= repeatEnd && repeatEnd != -1)
    {
      pattern = "";
    }
    else
    {
      if (patternCycle >= patternCycleEnd && patternCycleEnd != -1)
      {
        patternCycle = 0;
        repeat++;
      }
      else
      {
        if (pattern == "twist")
        {
          state[twist[patternCycle][0]][twist[patternCycle][1]][twist[patternCycle][2]][3] = twist[patternCycle][3];

          if (twist[patternCycle][3] == 1)
          {
            pickColour(colour);

            for (short d = 0; d < 3; d++)
            {
              state[twist[patternCycle][0]][twist[patternCycle][1]][twist[patternCycle][2]][d] = rgb[d];
            }
          }

          if (twist[patternCycle][4] == 0)
          {
            DELAY = 0;
          }
          else
          {
            DELAY = twistData[0];
          }
        }


        if (pattern == "drop")
        {
          state[drop[patternCycle][0]][drop[patternCycle][1]][drop[patternCycle][2]][3] = drop[patternCycle][3];

          if (drop[patternCycle][3] == 1)
          {
            pickColour(colour);

            for (short d = 0; d < 3; d++)
            {
              state[drop[patternCycle][0]][drop[patternCycle][1]][drop[patternCycle][2]][d] = rgb[d];
            }
          }

          if (drop[patternCycle][4] == 0)
          {
            DELAY = 0;
          }
          else
          {
            DELAY = dropData[0];
          }
        }

        if (pattern == "diagonal")
        {
          state[diagonal[patternCycle][0]][diagonal[patternCycle][1]][diagonal[patternCycle][2]][3] = diagonal[patternCycle][3];
          if (diagonal[patternCycle][3] == 1)
          {
            pickColour(colour);

            for (short d = 0; d < 3; d++)
            {
              state[diagonal[patternCycle][0]][diagonal[patternCycle][1]][diagonal[patternCycle][2]][d] = rgb[d];
            }
          }

          if (diagonal[patternCycle][4] == 0)
          {
            DELAY = 0;
          }
          else
          {
            DELAY = diagonalData[0];
          }
        }

        if (pattern == "test")
        {
          state[test[0]][test[1]][test[2]][3] = test[3];
          if (test[3] == 1)
          {
            pickColour(colour);

            for (short d = 0; d < 3; d++)
            {
              state[test[0]][test[1]][test[2]][d] = rgb[d];
            }
          }
          if (test[4] == 0)
          {
            DELAY = 0;
          }
          else
          {
            DELAY = testData[0];
          }
        }

        if (pattern == "orient")
        {
          CurieIMU.readMotionSensor(axRaw, ayRaw, azRaw, gxRaw, gyRaw, gzRaw);
          accRaw[X] = axRaw;
          accRaw[Y] = ayRaw;
          accRaw[Z] = azRaw;

          convertRawAcceleration();
          pickColour(colour);
          lightOrient();

        }


        patternCycle++;

      }
    }
    timer = millis();
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

short rainbow()
{
  colourCycleEnd = 27;
  float freq = 6.2832 / colourCycleEnd;  //ie 2 pi by colo..
  for (short s = 0; s < 3; s++)
  { //  \/ moves each wave out of phase
    rgb[s] = sin(freq * colourCycle + (2 * s)) * 127 + 128;
  }
  colourCycle++;

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
      rainbow();
      break;
  }
}

static void eventCallback(void)
{
  if (CurieIMU.getInterruptStatus(CURIE_IMU_SHOCK))
  {
    for (short e = 0; e < 2; e++)
    {
      for (short w = 0; w < 2; w++)
      {
        for (short n = 0; n < 2; n++)
        {
          state[e][w][n][0] = 255;
          state[e][w][n][1] = 0;
          state[e][w][n][2] = 0;
          state[e][w][n][3] = 1;
        }
      }
    }
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
    flip = -1;
    if (acc[r] - ANGLE > 0)
    {
      flip = 0;
    }
    else if (acc[r] + ANGLE < 0)
    {
      flip = 1;
    }

    if (flip != -1)
    {
      for (short i = 0; i < 2; i++)
      {
        for (short j = 0; j < 2; j++)
        {
            for (short l = 0; l < 3; l++)
            {
              if (ORIENTLEDMAP[(r*2) + flip][l] == A)
              {
                orientledmap[(r*2) + flip][l] = i;
              }
              else if (ORIENTLEDMAP[(r*2) + flip][l] == B)
              {
                orientledmap[(r*2) + flip][l] = j;
              }
            }
          stateOrient[orientledmap[(r*2) + flip][0]][orientledmap[(r*2) + flip][1]][orientledmap[(r*2) + flip][2]] += 1;
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
