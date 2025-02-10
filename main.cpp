/**
 * @file    distance_led_indicator.ino
 * @author  Marce Gonzalez (Marcelowis-code)
 * @date    2025-02-10
 * @brief   LED distance indicator using an ultrasonic sensor and OLED display.
 *
 * This project controls an LED strip based on distance measurements from an ultrasonic sensor.
 * The closer the object, the more LEDs turn on, changing colors from green to orange to red.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>

#define POT_PIN A0  // Potentiometer connected to A0
#define LED_PIN     14
#define NUM_LEDS    16
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SOUND_SPEED 0.034

CRGB leds[NUM_LEDS];
int numLedsOn = 0;

// Potentiometer variables
int ctrl = 0; 
int potValue = 0;

// OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Ultrasonic sensor
const int trigPin = 2;
const int echoPin = 16;

long duration;
int distanceCm;

void setup() {
  Serial.begin(115200);

  delay(1000); // Small delay for stability

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
}

void loop() {
  // Read potentiometer value
  potValue = analogRead(POT_PIN);
  ctrl = map(potValue, 0, 1023, 0, 100);  // Range from 1 cm to 100 cm

  // Read distance
  distanceCm = readDistance();

  // Display values on OLED
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("DIST:");
  display.println(distanceCm);
  display.print("CTRL:");
  display.println(ctrl);
  display.display(); 

  // If distance is greater than the control value, turn off LEDs
  if (distanceCm > ctrl) {
    FastLED.clear();
  } else {
    // Map distance to the number of LEDs to turn on
    numLedsOn = map(distanceCm, ctrl, 5, 0, NUM_LEDS);
    numLedsOn = constrain(numLedsOn, 0, NUM_LEDS);

    // Turn off all LEDs before updating
    FastLED.clear();

    // Turn on LEDs in 3 color zones
    for (int i = 0; i < numLedsOn; i++) {
      if (i < 4) {
        leds[i] = CRGB::Green;   // Green (first third)
      } else if (i < 9) {
        leds[i] = CRGB::Orange;  // Orange (second third)
      } else {
        leds[i] = CRGB::Red;     // Red (last third)
      }
    }
  }

  FastLED.show();
  delay(100);
}

int readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  return duration * SOUND_SPEED / 2;
}
