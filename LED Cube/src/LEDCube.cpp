#define FASTLED_ALLOW_INTERRUPTS 0
#include <FastLED.h>
#include <BlynkSimpleSerialBLE.h>
#define DATA_PIN 2
#define LED_TYPE PL9823
#define COLOR_ORDER RGB
#define NUM_LEDS 125
#define analogin A2
#define UPDATES_PER_SECOND 100

#define BLYNK_USE_DIRECT_CONNECT

CRGB leds[NUM_LEDS];
int BRIGHTNESS;
int BLE_BRIGHTNESS = 120;

//Setup palette
CRGBPalette16 currentPalette;
TBlendType currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;


//Blynk app variables
char auth[] = "f4cc6eeb33244495a3627c087c6ad004";
int BLE_BRIGHTNESS_TOGGLE = 0;
int animationSelect;
BlynkTimer timer;

void myTimerEvent();
void runColorPalettes();
void updateBrightness();
int getLEDIndexByCoordinate(int x, int y, int z);
void towerPulse();
void twoColorCenterMeetUp();
void cubeOutline();
void solidCubeColor(CRGB color);
void solidHueCylce();
void FillLEDsFromPaletteColors(int colorIndex);
void ChangePalettePeriodically();
void SetupTotallyRandomPalette();
void SetupBlackAndWhiteStripedPalette();
void SetupPurpleAndGreenPalette();

void setup()
{
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.clear();

  currentPalette = OceanColors_p;
  currentBlending = LINEARBLEND;


  // Blynk will work through Serial
  // Do not read or write this serial manually in your sketch
  // Serial.begin(9600);
  // Blynk.begin(Serial, auth);
  // timer.setInterval(1000L, myTimerEvent);
}

void loop()
{
  updateBrightness();
  // Blynk.run(); //Runs all blynk input functions, currently messing with FastLED's timing
  // timer.run();

  //Randomly swap animations
  animationSelect = 4;//random(0, 4);

  switch (animationSelect)
  {
  case 0:
    solidHueCylce();
    break;
  case 1:
    towerPulse();
    break;
  case 2:
    cubeOutline();
    break;
  case 3:
    twoColorCenterMeetUp();
    break;
  case 4:
      runColorPalettes();
      break;
  default:
    runColorPalettes();
  }
}

void myTimerEvent(){
  Blynk.virtualWrite(V3, "\n\nBLE TOGGLE: ", BLE_BRIGHTNESS_TOGGLE, "\nBLE_BRIGHTNESS: ", BLE_BRIGHTNESS, "\nAnimation: ", animationSelect, "\nBrightness: ", BRIGHTNESS, "\n");
}

//Blink Settings
BLYNK_WRITE(V0)
{
  animationSelect = param.asInt();
}
BLYNK_WRITE(V29)
{
  BLE_BRIGHTNESS = param.asInt();
}
BLYNK_WRITE(V30)
{
  BLE_BRIGHTNESS_TOGGLE = param.asInt();
}

void runColorPalettes()
{
  ChangePalettePeriodically();

  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */

  FillLEDsFromPaletteColors(startIndex);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void updateBrightness()
{
  //Get Brightness level for cube
  if (BLE_BRIGHTNESS_TOGGLE)
  {
    BRIGHTNESS = BLE_BRIGHTNESS;
  }
  else
  {
    BRIGHTNESS = map(analogRead(analogin), 0, 1023, 0, 255);
  }
  FastLED.setBrightness(BRIGHTNESS);
}

//Here is a very useful function for when making animations which
//work better with 3D coordinates
int getLEDIndexByCoordinate(int x, int y, int z)
{
  //Only O(1) complexity!!! But this is using hardcoded numbers for a specific cube size of 5x5x5 consiting of 125 leds
  int ledIndex = 25 * z; // Add the height by 25 leds per layer

  switch (y)
  {
  case 0:
    ledIndex += x;
    break;
  case 1:
    ledIndex += (9 - x);
    break;
  case 2:
    ledIndex += (10 + x);
    break;
  case 3:
    ledIndex += (19 - x);
    break;
  case 4:
    ledIndex += (20 + x);
    break;
  }
  return ledIndex;
}

void towerPulse()
{
  FastLED.clear();
  int ledsPerLayer = 25;
  double startDelay = 500;
  double delayStep = 1.25;
  double minDelay = 30;
  int rapidCount = 0;

  CRGB color1 = CRGB::White;
  CRGB color2 = CRGB::Blue;
  for (int k = startDelay; rapidCount <= 15; k /= delayStep)
  {
    if (k < minDelay)
    {
      k = minDelay;
      rapidCount++;
    }
    for (int i = 1; i <= NUM_LEDS / ledsPerLayer; i++)
    {
      updateBrightness();
      FastLED.clear();
      for (int j = ledsPerLayer * (i - 1); j < ledsPerLayer * i; j++)
      {
        if (j % 2 == 0)
          leds[j] = color1;
        else
          leds[j] = color2;
      }
      FastLED.show();
      delay(k);
    }
  }

  double maxDelay = 500;
  //Do same thing but start slowing down
  for (int k = minDelay; k <= maxDelay; k *= delayStep)
  {
    for (int i = 1; i <= NUM_LEDS / ledsPerLayer; i++)
    {
      updateBrightness();
      FastLED.clear();
      for (int j = ledsPerLayer * (i - 1); j < ledsPerLayer * i; j++)
      {
        if (j % 2 == 0)
          leds[j] = color1;
        else
          leds[j] = color2;
      }
      FastLED.show();
      delay(k);
    }
  }
}

void twoColorCenterMeetUp()
{
  FastLED.clear();
  uint8_t Hue1 = random(255 / 2, 255);
  uint8_t Hue2 = random(0, 255 / 2);
  CRGB color1 = CHSV(Hue1, 255, 255);
  CRGB color2 = CHSV(Hue2, 255, 255);

  int trailSize = 4;
  int trailCounter = 0;
  int blinkDelay = 100;
  int blinkAmount = 10;
  int explosionSpeed = 200; //Bigger number is slower

  for (int i = 0, j = NUM_LEDS - 1; i <= j; i++, j--)
  {
    updateBrightness();
    trailCounter++;
    if (trailCounter == trailSize)
    {
      leds[i - trailSize] = CRGB::Black;
      leds[j + trailSize] = CRGB::Black;
      trailCounter--;
    }
    if (i == j)
    {
      CRGB temp = blend(color1, color2, 127);
      leds[i] = temp;
      FastLED.show();
      for (int k = 0; k <= trailSize; k++)
      {
        leds[i] = CRGB::Black;
        leds[i + trailSize - k] = CRGB::Black;
        leds[i - trailSize + k] = CRGB::Black;
        FastLED.show();
        FastLED.delay(blinkDelay);
        leds[i] = temp;
        FastLED.show();
        FastLED.delay(blinkDelay);
      }
      //Make Explosion from middle
      for (int k = 1; k <= 2; k++)
      {
        FastLED.clear();
        leds[i - 5 * k] = temp;
        leds[i + 5 * k] = temp;
        leds[i - 1 * k] = temp;
        leds[i + 1 * k] = temp;

        leds[i + 4 * k] = temp;
        leds[i - 4 * k] = temp;
        leds[i + 6 * k] = temp;
        leds[i - 6 * k] = temp;

        leds[i + 25 * k] = temp;
        leds[i - 25 * k] = temp;
        leds[i + 24 * k] = temp;
        leds[i - 24 * k] = temp;
        leds[i + 26 * k] = temp;
        leds[i - 26 * k] = temp;

        FastLED.show();
        FastLED.delay(explosionSpeed);
      }
      for (int k = 0; k < NUM_LEDS / 2; k++)
      {
        leds[random(125)] = temp;
        FastLED.delay(5);
        FastLED.show();
      }
    }
    else
    {
      leds[i] = CHSV(Hue1, 255, 255);
      leds[(NUM_LEDS - 1) - i] = CHSV(Hue2, 255, 255);
    }

    FastLED.show();
    FastLED.delay(100);
  }
  FastLED.clear();
}

void cubeOutline()
{
  FastLED.clear();
  CRGB color1 = CRGB::White;
  for (int x = 0; x < 5; x++)
  {
    leds[getLEDIndexByCoordinate(0, 0, x)] = color1;
    leds[getLEDIndexByCoordinate(0, 4, x)] = color1;
    leds[getLEDIndexByCoordinate(4, 0, x)] = color1;
    leds[getLEDIndexByCoordinate(4, 4, x)] = color1;
    if (x == 0 || x == 4)
    {
      for (int y = 0; y < 5; y++)
      {
        int index1 = getLEDIndexByCoordinate(x, y, 0);
        int index2 = getLEDIndexByCoordinate(x, y, 4);
        leds[index1] = color1;
        leds[index2] = color1;
      }
    }
    int index1 = getLEDIndexByCoordinate(x, 0, 0);
    int index2 = getLEDIndexByCoordinate(x, 4, 0);
    int index3 = getLEDIndexByCoordinate(x, 0, 4);
    int index4 = getLEDIndexByCoordinate(x, 4, 4);
    leds[index1] = color1;
    leds[index2] = color1;
    leds[index3] = color1;
    leds[index4] = color1;
  }
  FastLED.show();
  FastLED.delay(5000);
}

void solidCubeColor(CRGB color)
{
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = color;
  }
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void solidHueCylce()
{
  for (int i = 0; i < 255; i++)
  {
    updateBrightness();
    CRGB color = CHSV(i, 255, 255);
    solidCubeColor(color);
    FastLED.delay(100);
  }
}

void FillLEDsFromPaletteColors(int colorIndex)
{
  int brightness = 255;

  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.  All are shown here.

void ChangePalettePeriodically()
{
  uint8_t secondHand = (millis() / 1000) % 60;
  static uint8_t lastSecond = 99;

  if (lastSecond != secondHand)
  {
    lastSecond = secondHand;
    if (secondHand == 0)
    {
      currentPalette = RainbowColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 10)
    {
      currentPalette = RainbowStripeColors_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 15)
    {
      currentPalette = RainbowStripeColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 20)
    {
      SetupPurpleAndGreenPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 25)
    {
      SetupTotallyRandomPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 30)
    {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = NOBLEND;
    }
    if (secondHand == 35)
    {
      SetupBlackAndWhiteStripedPalette();
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 40)
    {
      currentPalette = CloudColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 45)
    {
      currentPalette = PartyColors_p;
      currentBlending = LINEARBLEND;
    }
    if (secondHand == 50)
    {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = NOBLEND;
    }
    if (secondHand == 55)
    {
      currentPalette = myRedWhiteBluePalette_p;
      currentBlending = LINEARBLEND;
    }
  }
}

// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
  for (int i = 0; i < 16; i++)
  {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid(currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV(HUE_PURPLE, 255, 255);
  CRGB green = CHSV(HUE_GREEN, 255, 255);
  CRGB black = CRGB::Black;

  currentPalette = CRGBPalette16(
      green, green, black, black,
      purple, purple, black, black,
      green, green, black, black,
      purple, purple, black, black);
}

// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
    {
        CRGB::Red,
        CRGB::Gray, // 'white' is too bright compared to red and blue
        CRGB::Blue,
        CRGB::Black,

        CRGB::Red,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Black,

        CRGB::Red,
        CRGB::Red,
        CRGB::Gray,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Blue,
        CRGB::Black,
        CRGB::Black
      };