//--------------------------------------------------------------//
// Arduino Uno R4 Wifi Board 
// Adafruit Rotary Encoder (5880) connected to Qwiic connector
// and Adafruit DotStar White LED Strip (2238) connected to 
// Pins 4 & 5 and 8 & 9 that provide data and clock
// Rotary encoder used to choose which LED "program" to run
//-------------------------------------------------------------//
#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"
#include "Adafruit_DotStar.h"
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>
#include "Wire.h"
#include <cstring> // Required for strlen

#define NUMLEDSINPHYSICALSTRIP 62
#define NUMPROGRAMMABLELEDS 62    // Number of LEDs that will be programmed in strip
#define BRIGHTNESS 50
#define MAXBRIGHTNESS 80
#define MINBRIGHTNESS 5
#define INCREMENTBRIGHTNESS 2
#define DELAYBRIGHTNESS 50

// Seesaw constants on Rotary Encoder
#define SS_SWITCH        24
#define SS_NEOPIX        6
#define SEESAW_ADDR      0x36  // the default I2C address for the encoder breakout

Adafruit_seesaw ss = Adafruit_seesaw(&Wire1);   // Adafruit seesaw
seesaw_NeoPixel sspix = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800, &Wire1);
int32_t encoder_position;

//--------------------------- 1rst LED Strip (direct connect to Arduino) -----------------------------------------------//
const int clockPin_strip1 = 9; // blue JST 4-pin
const int dataPin_strip1 = 8;  // green JST 4-pin 
// 
// clock: pin9
// arduino, Pin9 -> use blue jumper which is soldered to blue conductor (via a 470 ohm resistor) on 4-pin JST male connector --> soldered to blue  on JST Female --> yellow on LED strip
// data: pin8
// arduino, Pin8 -> use green jumper which is soldered to green conductor (via a 470 ohm resistor) on 4-pin JST male connector --> soldered to green  on JST Female --> green on LED strip

// 1B LED Strip (continuation of 1rst LED strip, with 5-volt power tapped in)
// data:  DO on 1rst LED Strip connected via black conductor of 2-Pin JST Male --> black conductor of 2-pin JST Female --> soldered to DI of 2nd LED Strip
// clock: CO on 1rst LED Strip connected via red   conductor of 2-Pin JST Male --> red conductor of 2-pin JST Female   --> soldered to CI of 2nd LED Strip
 
// GND = black on JST connector --> soldered to red on JST --> black on strip. not used!
//--------------------------- 2nd LED Strip (direct connect to Arduino) -----------------------------------------------//
const int clockPin_strip2 = 5; // blue JST 2-pin
const int dataPin_strip2 = 4;  // green JST 2-pin
//
// clock: pin5 -> use blue jumper which is soldered to red conductor (via a 470 ohm resistor) on 2-pin JST male connector 
// data: pin4  -> use green jumper which is soldered to black conductor (via a 470 ohm resistor) on 2-pin JST male connector --> black on LED strip2


Adafruit_DotStar physicalStripAll1(NUMLEDSINPHYSICALSTRIP, dataPin_strip1, clockPin_strip1);
Adafruit_DotStar physicalStripAll2(NUMLEDSINPHYSICALSTRIP, dataPin_strip2, clockPin_strip2);
Adafruit_DotStar strip1(NUMPROGRAMMABLELEDS, dataPin_strip1, clockPin_strip1);
Adafruit_DotStar strip2(NUMPROGRAMMABLELEDS, dataPin_strip2, clockPin_strip2);
ArduinoLEDMatrix matrix;
//------------------------------------------------------------------------------------------------//
//------------------------------------ scrollMatrix() --------------------------------------------//
void scrollMatrix(String str) {
  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(100);
  matrix.textFont(Font_4x6);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(str);             
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}
//--------------------------------------------------------------------------------------------------------//
//------------------------------------ updatePixelAndMatrix() --------------------------------------------//
void updatePixelAndMatrix(uint32_t color, char * number) {
      sspix.setPixelColor(0, color);
      sspix.show();
      matrix.clear();
      matrix.beginDraw();
      matrix.stroke(0xFFFFFFFF); // White color
      matrix.textFont(Font_5x7); // Using the 4x6 font
      size_t length = std::strlen(number);
      int startXPosition = 4;
      if(length > 1) startXPosition = 1;
      matrix.beginText(startXPosition, 1, 0xFFFFFF); // Start text at (startXPosition, 1) with white color
      matrix.println(number);
      matrix.endText();
      matrix.endDraw();
      delay(80);
}
//---------------------------------------------------------------------------------------------------------//
//------------------------------------ readEncoder() ------------------------------------------------------//
int readEncoder() {
  if (!ss.digitalRead(SS_SWITCH)) { // switch press resets the encoder position to 0
    ss.setEncoderPosition(0);
    updatePixelAndMatrix(0x000000, "0");
  }

  int32_t new_position = ss.getEncoderPosition();
  // limit encoder to have only 10 positions (OK, 11)
  if(new_position > 10) {
    ss.setEncoderPosition(10);
    new_position = 10;
    encoder_position = new_position - 1;
  } else if(new_position < 0) {
    ss.setEncoderPosition(0);
    new_position = 0;    
    encoder_position = new_position + 1;
  }
  if (encoder_position != new_position) {
    switch (new_position) {
      case 0:
        updatePixelAndMatrix(0x000000, "0");
        break;
      case 1:
        updatePixelAndMatrix(0x040007, "1");
        break;
      case 2:
        updatePixelAndMatrix(0x010207, "2");
        break;
      case 3:
        updatePixelAndMatrix(0x100000, "3");
        break;
      case 4:
        updatePixelAndMatrix(0x000900, "4");
        break;
      case 5:
        updatePixelAndMatrix(0x000004, "5");
        break;
      case 6:
        updatePixelAndMatrix(0x160001, "6");
        break;
      case 7:
        updatePixelAndMatrix(0x050202, "7");
        break;
      case 8:
        updatePixelAndMatrix(0x060200, "8");
        break;
      case 9:
        updatePixelAndMatrix(0x060001, "9");
        break;
      case 10:
        updatePixelAndMatrix(0x070707, "10");
        break;
    }
  }
  encoder_position = new_position;
  delay(50);
  return encoder_position;
}
//---------------------------------------------------------------------------------------------------------//
//------------------------------------ setup() ------------------------------------------------------------//
void setup() {
  //------------------------- Initialize Both LED Strips -------------------------//
  resetStrip(physicalStripAll1);
  resetStrip(physicalStripAll2);

  //------------------------- Initialize matrix and serial port -------------------------//
  matrix.begin();
  Serial.begin(115200);
  while (!Serial) delay(10); // Wait for Serial to be ready
  delay(1000);

  //------------------------- Initialize seesaw on Wire1 (Qwiic bus) -------------------------//
  while (!ss.begin(SEESAW_ADDR)) {
    Serial.println("Seesaw not found\n"); 
    scrollMatrix(".....Seesaw not found....");
    delay(500);
  }

  //------------------------- Check version of encoder -------------------------//
  while ((((ss.getVersion()>>16)) & 0xFFFF) != 4991) {
    Serial.println("Encoder version code not equal to 4991.\n"); 
    scrollMatrix("....Encoder version code not equal to 4991....");
    delay(500);
  }

  //------------------------- Turn on neopixel on rotary encoder -------------------------//
  sspix.begin(SEESAW_ADDR);
  sspix.setPin(SS_NEOPIX);
  sspix.updateLength(1);
  sspix.setPixelColor(0,0x000004);
  sspix.show();
  delay(2500);

  //------------------------- Use a pin for the built in encoder switch -------------------------//
  ss.pinMode(SS_SWITCH, INPUT_PULLUP);
  ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1);
  ss.enableEncoderInterrupt();

  //------------------------- initialize matrix to 0 -------------------------//
  ss.setEncoderPosition(0);
  updatePixelAndMatrix(0x000000, "0");
}
//--------------------------------------------------------------------------------------------------------//
//------------------------------------ loop() ------------------------------------------------------------//
void loop() {
  int program = readEncoder();
  switch (program) {
    case 0:
      resetStrip(physicalStripAll1);
      resetStrip(physicalStripAll2);
      break;
    case 1:
      constantBrightness(strip1, strip2);
      break;
    case 2:
      snake(strip1, strip2);
      break;
    case 3:
      reverseSnake(strip1, strip2);
      break;
    case 4:
      snake(strip1, strip2);
      reverseSnake(strip1, strip2);
      break;
    case 5:
      fadeInFadeOut(strip1);
      fadeInFadeOut(strip2);
      break;
    default:
      resetStrip(physicalStripAll1);
      resetStrip(physicalStripAll2);
      break;
  }
  delay(20);
}

//------------------------------------------------------------------------------------------------//
//           LED Programs Below                                                                   //
//------------------------------------------------------------------------------------------------//
//
void resetStrip(Adafruit_DotStar strip) {
  strip.begin(); // Initialize pins for output
  strip.setBrightness(0);
  strip.show();  // Turn all LEDs off ASAP
  delay(30);
}
//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//
void constantBrightness(Adafruit_DotStar firstStrip, Adafruit_DotStar secondStrip) {
  for (int i = 0; i < NUMLEDSINPHYSICALSTRIP; i++) {
    firstStrip.setPixelColor(i, MAXBRIGHTNESS); // Set to maximum brightness (white)
    secondStrip.setPixelColor(i, MAXBRIGHTNESS); // Set to maximum brightness (white)
  }
  firstStrip.show();  
  secondStrip.show();  
  delay(100);
}
//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//
void fadeInFadeOut(Adafruit_DotStar strip) {
  // Fade in
  for (int brightness = MINBRIGHTNESS; brightness <= MAXBRIGHTNESS; brightness += INCREMENTBRIGHTNESS) { // Increase brightness in steps
    strip.setBrightness(brightness); // Set the overall brightness of the strip
    for (int i = 0; i < NUMPROGRAMMABLELEDS; i++) {
      strip.setPixelColor(i, MAXBRIGHTNESS, MAXBRIGHTNESS, MAXBRIGHTNESS); // Set all pixels to white (full R, G, B)
    }
    strip.show(); // Update the strip with the new brightness and color
    delay(DELAYBRIGHTNESS);   // Short delay for smooth fading
  }
  // Fade out
  for (int brightness = MAXBRIGHTNESS; brightness >= MINBRIGHTNESS; brightness -= INCREMENTBRIGHTNESS) { // Decrease brightness in steps
    strip.setBrightness(brightness); // Set the overall brightness of the strip
    for (int i = 0; i < NUMPROGRAMMABLELEDS; i++) {
      strip.setPixelColor(i, MAXBRIGHTNESS, MAXBRIGHTNESS, MAXBRIGHTNESS); // Keep color white
    }
    strip.show(); // Update the strip
    delay(DELAYBRIGHTNESS);   // Short delay for smooth fading
  }
}
//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//
void snake(Adafruit_DotStar firstStrip, Adafruit_DotStar secondStrip) {
  int head = 0;
  int tail = -10;

  while (true) {
    firstStrip.setPixelColor(head, BRIGHTNESS);
    firstStrip.setPixelColor(tail, 0);
    firstStrip.show();
    secondStrip.setPixelColor(head, BRIGHTNESS);
    secondStrip.setPixelColor(tail, 0);
    secondStrip.show(); 
    delay(20);      // 20 ms

    if(++head >= NUMLEDSINPHYSICALSTRIP) head = NUMLEDSINPHYSICALSTRIP;
    if(++tail >= NUMLEDSINPHYSICALSTRIP) return; 
  }
}
//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//
void reverseSnake(Adafruit_DotStar firstStrip, Adafruit_DotStar secondStrip) {
  int head = NUMLEDSINPHYSICALSTRIP;
  int tail = NUMLEDSINPHYSICALSTRIP + 10;

  while (true) {
    firstStrip.setPixelColor(head, BRIGHTNESS);
    firstStrip.setPixelColor(tail, 0);
    firstStrip.show();
    secondStrip.setPixelColor(head, BRIGHTNESS);
    secondStrip.setPixelColor(tail, 0);
    secondStrip.show(); 
    delay(20);      // 20 ms

    if(--head < 0) head = -1;
    if(--tail < 0) return; 
  }
}
//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//


