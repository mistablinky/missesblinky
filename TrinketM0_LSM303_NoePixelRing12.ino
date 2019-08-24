#include <Adafruit_NeoPixel.h>
#include <Adafruit_DotStar.h>
#include <Adafruit_LSM303_U.h>
//#include <Adafruit_SleepyDog.h>

// ---------------------- MISSES BLiNKY 0.3 ----------------------------------
/////////////////////////////////////////////////////////////////////////////
// Motion controlled LED earring
// 2017-2019 by Thomas Schaefer (thomas@blinky.berlin)
/////////////////////////////////////////////////////////////////////////////
// Hardware Setup:
// - Adafruit Trinket M0 3.3V 48MHz
//   ATSAMD21E18 32-bit Cortex M0+ Microcontroller
//   256K Flash, 32k RAM
// - Adafruit Flora I2C LSM303 (Accelerometer/Magnetometer)
//   SDA -------> Pin SDA
//   SCL -------> Pin SCL
// - ADAFRUIT RGBW NeoPixel Ring (12 pixel)
//   DIN-Front -> Pin 1 (through 470 ohm resistor)
// - Adafruit Pro Trinket LiPo Backpack Charger
//   BAT -------> VIN
//   G ---------> GND
//   5V --------> USB+
// - LiPo Akku 50mAh 3.7V
/////////////////////////////////////////////////////////////////////////////
// Free Culture License:
// This work is licensed under a Creative Commons
// >>> Attribution-ShareAlike 4.0 International License (CC BY-SA 4.0) <<<
// Adaptations and commercial use are allowed AS LONG AS OTHERS SHARE ALIKE
// License terms: https://creativecommons.org/licenses/by-sa/4.0/
/////////////////////////////////////////////////////////////////////////////

// MISSES BLiNKY SETTINGS -----------------------------------------------------
#define SENSITIVITY 0.3 // m/s^2, the lower the value the higher the motion sensitivity
#define SPEED        10 // milli seconds, the lower the value the higher the glitter effect speed
#define BRIGHTNESS   20 // 2...255, the lower the value the higher the battery life time
unsigned long lastRun;  // storage for the speed timer
// ----------------------------------------------------------------------------

// NEOPIXEL SETTINGS ----------------------------------------------------------
#define NEO_PIN         1
#define NUMPIXELS      12 // Number of NeoPixels on the ring
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEO_PIN, NEO_GRBW + NEO_KHZ800);
boolean anyPixelOn;
// ----------------------------------------------------------------------------

// MOTION SENSOR SETUP --------------------------------------------------------
// Create LSM303 acceleration sensor instance
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
float last_accel_x;
float last_accel_y;
float last_accel_z;  // not enough ram for 3D space calculation on Trinket 8MHz 
// ----------------------------------------------------------------------------

// ONBOARD LED AND USB SETUP --------------------------------------------------------
#define ONBOARD_LED             13
#define ONBOARD_DOTSTAR_DI       7
#define ONBOARD_DOTSTAR_CL       8
Adafruit_DotStar dotstar = Adafruit_DotStar(1, ONBOARD_DOTSTAR_DI, ONBOARD_DOTSTAR_CL, DOTSTAR_BRG);

#if defined(ARDUINO_ARCH_SAMD)
  // Required for Serial on Zero based boards
  #define Serial SERIAL_PORT_USBVIRTUAL
#endif
// ----------------------------------------------------------------------------

// WATCHDOG AND DEBUG SETTINGS --------------------------------------------------
#define DEBUG     0   // 0 = debug mode off, 1 = debug mode off
//int countdownMS = Watchdog.enable(4000);
// ----------------------------------------------------------------------------


void setup() {

//  // start serial communication
//  Serial.begin(9600);
//  while(!Serial); // wait for Arduino Serial Monitor (native USB boards)
//  Serial.println("Booting up MISSES BLiNKY v0.3");
  
  // Initializes the onboard LED, the dotstar LED and the neopixel ring
  pinMode(ONBOARD_LED, OUTPUT);
  dotstar.begin();
  pixels.begin();
  anyPixelOn = false;
  
  // Initialize dotstar LED to 'off'
  dotstar.setPixelColor(0, 0, 0, 0);
  dotstar.show();

  // Initializes the acceleration sensor on the LSM303 board
  //Serial.print("looking for LSM303 motion sensor...");
  accel.begin();
  //Serial.println("found");

  // reset speed timer
  lastRun = millis();

//  Serial.print(millis());
//  Serial.println(" - Let's go!");

} // END SETUP

void loop() {

  //Watchdog.reset();

  if (millis()-lastRun >= SPEED) {
//  if (true) {

    // turn off all neopixel and onboard LED
    if (anyPixelOn) {
      
      for(uint8_t i=0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(0,0,0,0)); // off
      }
      pixels.show();
      
      digitalWrite(ONBOARD_LED, LOW);
      anyPixelOn = false;
    }
  
    // get new acceleration data in m/s^2
    sensors_event_t event;
    accel.getEvent(&event);
  
    // if a motion is detected ...
    // (by calculating the distance of the acceleration vectors)
    if ( abs(last_accel_x - event.acceleration.x) > SENSITIVITY ||
         abs(last_accel_y - event.acceleration.y) > SENSITIVITY ||
         abs(last_accel_z - event.acceleration.z) > SENSITIVITY ) {

      digitalWrite(ONBOARD_LED, HIGH);
      
      // ... choose randomized a neo pixel ...
      uint8_t i = (uint8_t)random(0, NUMPIXELS);

      //Serial.println(i);
      
      
      // ... and set its color randomized
      pixels.setPixelColor(i, pixels.Color(
        (uint8_t)random(0, BRIGHTNESS),   // red
        (uint8_t)random(0, BRIGHTNESS),   // green
        (uint8_t)random(0, BRIGHTNESS),   // blue
        (uint8_t)random(0, BRIGHTNESS))); // white
      
      // finally update neo pixel ring
      // and turn onboard LED on
      pixels.show();
      
      
      digitalWrite(ONBOARD_LED, LOW);
      anyPixelOn = true;
    }
  
    // reset acceleration vector
    last_accel_x = event.acceleration.x;
    last_accel_y = event.acceleration.y;
    last_accel_z = event.acceleration.z;
      
    // reset speed timer
    lastRun = millis();    
  }

  //Watchdog.sleep(SPEED);

} // END LOOP
