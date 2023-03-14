#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#include "./classes/config.h"
#include "./classes/sdcard.h"
#include "./classes/UDPServer.h"
#include "./classes/IOTServices.h"
#include "./classes/screen.h"
#include "./classes/thermometer.h"

#include "constants.h"

Config *cnf = 0;
SdCard *sd = 0;
IOTServices *srv = 0;
Screen *sc = 0;
Thermometer *th = 0;
UDPServer *udpServer=0;

int nLoopUDP=0;
int nLoopBoard=0;

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
  srv = new IOTServices();
  udpServer= new UDPServer();
  sc = new Screen();
}

void loop()
{
  udpServer->udpServerLoop();
  srv->webSocketLoop();
  if (srv->listenClient())
  {
    alive_by_led();
  }
  delay(1);
  
  //Section periodic operations
  nLoopUDP++;
  if(nLoopUDP%3000==0){
    nLoopUDP=0;
    udpServer->sendBaecon();
  }

  nLoopBoard++;
  if(nLoopBoard%3000==0){
    nLoopBoard=0;
    srv->sendBoardPinOutput();
  }

}
