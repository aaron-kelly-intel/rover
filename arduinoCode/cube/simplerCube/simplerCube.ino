#define RED 1
#define GREEN 2
#define BLUE 3
#define PURPLE 4

#define X 0
#define Y 1
#define Z 2
#define ANGLE 22


#include "CurieIMU.h"

//  arrays with three values corrispondes to X, Y, Z
double acc[3];         // accelerometer values
double accRaw[3];

//  arrays with three sets of two are values corrisponding to the leds in X, Y, Z (ie. var[X][Y][Z])
int stateOrientToMap[2][2][2] = {{{0, 0}, {0, 0}}, {{0, 0}, {0, 0}}};  // place to sum orientation values to be mapped to lights
int Buffer[3][3] = {{0, 0, 0}, {0, 0, 0}, {90, 90, 90}};               // use the average of the last three if accRaw falls outside the usual range, which would lead to an error
int orientMaxPoint = 0;                                                // the point or points with corrispond to the lowest (orentation wise) LED in the cube and is to be lit

int axRaw, ayRaw, azRaw;         // accelerometer values
int gxRaw, gyRaw, gzRaw;

int ledLayer[2][2][3] = {{{11, 13, 12}, {10, 8, 9}}, {{3, 5, 4}, {14, 16, 15}}};  //  [X][Y][R,G,B} reference to the rgb columns on the cube
int ledZ[2] = {7, 6}; // reference the two z ground pins
int RATIO[3] = {3, 6, 1}; // ratio of the intensities or R G and B so mixed colours look as the should

int colourCycleEnd;  //the amount of different colour stages in the cycle
int colourCycle = 0; //the current stage in the colour cycle

int timer = 0;       // used to keep the main loop running while only changing the pattern every so often

int DELAY = 0;       // delay between the pattern change
int repeatEnd = 0;   // the amount of times the pattern is to be run
int repeat = 0;      // current time the pattern is being run

String pattern = "";        // name of the pattern being run
String StringColour = "";   // name of the colour of the pattern being run
int colour = 0;             // index number of colour being run
int patternCycleEnd;        // the total number of stages in the pattern
int patternCycle = 0;       // current stage of the pattern

int rgb[3] = {255, 0, 0}; // gobal variable where the colour for the stage of the pattern is held (should be initionalised as 0,0,0 but setting red component to 255 fixes a bug, no idea why?)

int state[2][2][2][4] = {  // the place where the current led info is held, which led X, Y, Z and then the RGB and on/off of each
  { {{255, 0, 0, 1}, {255, 255, 0, 1}},
    {{255, 0, 255, 1}, {0, 255, 0, 1}}
  },

  { {{0, 0, 255, 1}, {0, 255, 255, 1}},
    {{126, 200, 0, 1}, {60, 0, 120, 1}}
  }
};

void setup()
{


  for (int k = 0; k < 2; k++)                         // initalise all the pin outs
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
  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);  // auto calibrate, 1 cause on g of force straight down
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 1);
  CurieIMU.attachInterrupt(eventCallback);
}

void loop()
{
  for (int k = 0; k < 2; k++)
  {
    for (int d = 0; d < 2; d++)
    {
      for (int r = 0; r < 2; r++)
      {
        light(k, d, r, state[k][d][r]);  // loop through lighting each led accarding to the info in the "state" array
      }
    }
  }

  colour = RED;

  repeatEnd = -1;
  patternCycleEnd = -1;
  DELAY = 0;

  for (int e = 0; e < 2; e++)
  {
    for (int w = 0; w < 2; w++)
    {
      for (int n = 0; n < 2; n++)
      {
        for (int l = 0; l < 4; l++)
        {
          state[e][w][n][l] = 0;    // once there is data entered wipe the initial walues in "state", turning all the leds off
        }
      }
    }
  }

  if (millis() - timer > DELAY)     // wait "DELAY" amount of time
  {
    if (repeat >= repeatEnd && repeatEnd != -1)  // once the number of repeats reaches the max reset the pattern unless the pattern is to run continously
    {
      pattern = "";
    }
    else
    {
      if (patternCycle >= patternCycleEnd && patternCycleEnd != -1)  // onne the end of the pattern os reached increment the number of times the pattern has been repeated
      {
        patternCycle = 0;
        repeat++;
      }
      else
      {
        CurieIMU.readMotionSensor(axRaw, ayRaw, azRaw, gxRaw, gyRaw, gzRaw); // read in the sensor data
        accRaw[X] = axRaw;  // assign sensor data to an array to keep it all together
        accRaw[Y] = ayRaw;
        accRaw[Z] = azRaw;

        convertRawAcceleration();  // turn raw data into degrees
        pickColour(colour);        // select the clolur / next colour in the sequence
        lightOrient();             // determine the leds to be lit

      }


      patternCycle++;   // next pattern stage is done cycle to the next

    }
  }
  timer = millis(); // reset the timer,
}


int light(int x, int y, int z, int Data[4]) //3 rgb, 1 state, (4 as the last bit of data is whether the led is on or not)
{
  int totalTime = 0; //used to record the total time the function has taken to run (needed to keep the consistency of the intensities of th leds
  if (Data[3] == 1) // check if the led is suppossed to be on (having it as part of the rgb makes it easier to keep all the info together) if not don't bother assigning colour
  {
    digitalWrite(ledZ[z], LOW);             // select the Z axis to light
    for (int i = 0; i < 3; i++)
    {
      int Delay = (Data[i] * 5) / RATIO[i];  // 'Delay' is the time the led is on corrisponding the the intensity of the color desired
      totalTime += Delay;                    // record it to the total time the function will take
      digitalWrite(ledLayer[x][y][i], HIGH);
      delayMicroseconds(Delay);
      digitalWrite(ledLayer[x][y][i], LOW);
    }
    digitalWrite(ledZ[z], HIGH);            //  turn Z pin off
  }
  delayMicroseconds(2000 - totalTime); // standardise the time it takes to light an led so the intensity remains constant whe lighting multiple leds
}

int rainbow()
{
  colourCycleEnd = 27;  // amount of colours in the cycle
  float freq = 6.2832 / colourCycleEnd;  //ie 6.2832  = 2 pi a full circle
  for (int s = 0; s < 3; s++)
  { //  \/ moves each wave out of phase while, moving in a wave through the colours
    rgb[s] = sin(freq * colourCycle + (2 * s)) * 127 + 128;
  }
  colourCycle++;

}

int pickColour(int COLOUR)
{
  switch (COLOUR)   // pre set colours
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
  if (pattern == "shock")  // only exicute if actually selected it to
  {
    if (CurieIMU.getInterruptStatus(CURIE_IMU_SHOCK))
    {
      for (int e = 0; e < 2; e++)
      {
        for (int w = 0; w < 2; w++)
        {
          for (int n = 0; n < 2; n++)
          {
            state[e][w][n][3] = 1; // turn all leds on

            for (int d = 0; d < 3; d++)  // asign the same colour to them all
            {
              state[e][w][n][d] = rgb[d];
            }
          }
        }
      }
    }
  }
}

int convertRawAcceleration()
{
  for (int h = 0; h < 3; h++)  // calculate for each axis
  {
    if (16410 - abs(accRaw[h]) < 0)   // if the raw number falls outside the usual range use the average of the past three values
    {
      acc[h] = (Buffer[h][0] + Buffer[h][1] + Buffer[h][2]) / 3;
    }
    else
    {
      acc[h] = (asin(accRaw[h] / 16410) / 3.14159265354) * 180;  // convert vector component in the axis direction to angle (based on the fact it is a component of one g its just inverse sin of the vector component)
      // devide by 16410 ( should really be 2^14 that the munber 1g is mapped to but moving the arduino as well as the 1g of gravity can often exceed the 1g added a little extra to reduce errors in the sin calculation by it being out of range
      // then convert to degrees
      Buffer[h][2] = Buffer[h][1];
      Buffer[h][1] = Buffer[h][0];                               // record new data to the average array
      Buffer[h][0] = acc[h];
    }
  }
}

int lightOrient()
{
  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      for (int k = 0; k < 2; k++)
      {
        stateOrientToMap[i][j][k] = 0;  //reset the temp map array
      }
    }
  }

  if (acc[X] - ANGLE > 0)  // if the x axis (0 degrees being the line from the usb to the 6 pins on the other sid of the board) of the board is less the "ANGLE" from vertical
  {
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
      {
        stateOrientToMap[0][i][j] += 1;   // add +1 to four leds on the lower half (orientation wise )
      }
    }
  }
  else if (acc[X] + ANGLE < 0)  // else if its within "ANGLE" degrees of 180 (still 0 on the other side as it goes 0, 90 , 0 not 0 , 90 , 180)
  {
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
      {
        stateOrientToMap[1][i][j] += 1;
      }
    }
  }

  if (acc[Y] - ANGLE > 0)  // same as above
  {
    //Serial.print("y  1 : ");
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
      {
        stateOrientToMap[i][1][j] += 1;
      }
    }
  }
  else if (acc[Y] + ANGLE < 0)
  {
    //Serial.print("y -1 : ");
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
      {
        stateOrientToMap[i][0][j] += 1;
      }
    }
  }

  if (acc[Z] - ANGLE > 0)  // same as above
  {
    //Serial.print("z  1 : ");
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
      {
        stateOrientToMap[i][j][0] += 1;
      }
    }
  }
  else if (acc[Z] + ANGLE < 0)
  {
    //Serial.print("z -1 : ");
    for (int i = 0; i < 2; i++)
    {
      for (int j = 0; j < 2; j++)
      {
        stateOrientToMap[i][j][1] += 1;
      }
    }
  }

  orientMaxPoint = 0;   // reset the max point ( ie the led(s) with the most amount of 'turn on points'

  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      for (int k = 0; k < 2; k++)
      {
        if (stateOrientToMap[i][j][k] > orientMaxPoint)  // find the new max point
        {
          orientMaxPoint = stateOrientToMap[i][j][k];
        }
      }
    }
  }

  for (int i = 0; i < 2; i++)
  {
    for (int j = 0; j < 2; j++)
    {
      for (int k = 0; k < 2; k++)
      {
        if (stateOrientToMap[i][j][k] == orientMaxPoint)  // turn on any leds at the max point
        {
          for (int d = 0; d < 3; d++)  // assign colours to it
          {
            state[i][j][k][d] = rgb[d];
          }
          state[i][j][k][3] = 1;
        }
        else
        {
          state[i][j][k][3] = 0;                          // else turn them off
        }
      }
    }
  }
}
