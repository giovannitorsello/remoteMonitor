#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include "./classes/config.h"
#include "./classes/sdcard.h"
#include "./classes/webguiserver.h"
#include "./classes/screen.h"
#include "./classes/thermometer.h"

// I2C pin definition
#define I2C_SDA 20
#define I2C_SCL 21

// oneWire channel protocol pin
#define ONE_WIRE_BUS 14

// Screen definition
#define SCREEN_ADDRESS 0x3C // Screen i2C Address
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin
#define LED_ONBOARD 13

Config *cnf = 0;
SdCard *sd = 0;
WebGuiServer *srv = 0;
Screen *sc = 0;
Thermometer *th = 0;

void alive_by_led()
{
  for (int i = 0; i < 10; i++)
  {
    digitalWrite(LED_ONBOARD, 1);
    delay(100);
    digitalWrite(LED_ONBOARD, 0);
    delay(150);
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(LED_ONBOARD, OUTPUT);

  // sd = new SdCard();
  //  cnf = new Config();
  //  sc = new Screen(SCREEN_ADDRESS, SCREEN_WIDTH, SCREEN_HEIGHT, OLED_RESET, &Wire);
  //  th = new Thermometer(ONE_WIRE_BUS);
  srv = new WebGuiServer();
}

void loop()
{
  srv->webSocketLoop();
  if (srv->listenClient())
  {
    alive_by_led();
  }
  delay(1);
}
