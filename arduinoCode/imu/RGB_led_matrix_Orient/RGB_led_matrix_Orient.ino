#define RED 1
#define GREEN 2
#define BLUE 3
#define PURPLE 4

#include "CurieIMU.h"

int ledLayer[2][2][3] = {{{11, 13, 12}, {10, 8, 9}}, {{3, 5, 4}, {14, 16, 15}}};
int ledZ[2] = {7, 6};
int RATIO[3] = {3, 6, 1};

int colourCycleEnd;
int colourCycle = 0;
int timer = 0;

int DELAY = 100;
int repeat = 0;
int repeatEnd = 0;

String pattern = "";
String StringColour = "";
int colour = 0;
int patternCycleEnd;
int patternCycle = 0;

int rgb[3] = {255, 0, 0}; // setting red component to 255 fixes a bug, no idea why?


int testData[] = {0, -1};
int test[5] = {1, 1, 1, 1, 0};

//                 delay, pattern cycle end,
int twistData[] = {50   , 32               };
int twist[32][5] = {
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



int dropData[] = { 100, 32};
int drop[32][5] = {
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


int diagonalData[] = { 200, 32};
int diagonal[32][5] = {
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


int state[2][2][2][4] = {
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
  
  for (int k = 0; k < 2; k++)
  {
    for (int d = 0; d < 2; d++)
    {
      for (int r = 0; r < 3; r++)
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

  
  /* Initialise the IMU */
  CurieIMU.begin();
  CurieIMU.attachInterrupt(eventCallback);
  /* Enable Shock Detection */
  CurieIMU.setDetectionThreshold(CURIE_IMU_SHOCK, 1500); // 1.5g = 1500 mg
  CurieIMU.setDetectionDuration(CURIE_IMU_SHOCK, 50);   // 50ms
  CurieIMU.interrupts(CURIE_IMU_SHOCK);

}

void loop()
{
  for (int k = 0; k < 2; k++)
  {
    for (int d = 0; d < 2; d++)
    {
      for (int r = 0; r < 2; r++)
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

    for (int e = 0; e < 2; e++)
    {
      for (int w = 0; w < 2; w++)
      {
        for (int n = 0; n < 2; n++)
        {
          for (int l = 0; l < 4; l++)
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

            for (int d = 0; d < 3; d++)
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

            for (int d = 0; d < 3; d++)
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

            for (int d = 0; d < 3; d++)
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

            for (int d = 0; d < 3; d++)
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

        patternCycle++;

      }
    }
    timer = millis();
  }
}


int light(int x, int y, int z, int Data[4]) //3 rgb, 1 state, (4 as the last bit of data is whether the led is on or not)
{ 
  int totalTime = 0; //used to record the total time the function has taken to run
  if (Data[3] == 1) // check if the led is suppossed to be on (having it as part of the rgb makes it easier to keep all the info together
  {
    digitalWrite(ledZ[z], LOW);
    for (int i = 0; i < 3; i++)
    {
      int Delay = (Data[i] * 5) / RATIO[i];
      totalTime += Delay;
      digitalWrite(ledLayer[x][y][i], HIGH);
      delayMicroseconds(Delay);
      digitalWrite(ledLayer[x][y][i], LOW);
    }
    digitalWrite(ledZ[z], HIGH);
  }
  delayMicroseconds(2000 - totalTime); // standardise the time it takes to light an led so the intensity remains constant whe lighting multiple leds
}

int rainbow()
{
  colourCycleEnd = 27;
  float freq = 6.2832 / colourCycleEnd;  //ie 2 pi by colo..
  for (int s = 0; s < 3; s++)
  { //  \/ moves each wave out of phase
    rgb[s] = sin(freq * colourCycle + (2 * s)) * 127 + 128;
  }
  colourCycle++;

}

int pickColour(int COLOUR)
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
    for (int e = 0; e < 2; e++)
    {
      for (int w = 0; w < 2; w++)
      {
        for (int n = 0; n < 2; n++)
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

