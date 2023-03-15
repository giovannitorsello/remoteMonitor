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
  pinMode(22, OUTPUT);
  pinMode(24, OUTPUT);
  pinMode(26, OUTPUT);

  // sd = new SdCard();
  //  cnf = new Config();
  //  th = new Thermometer(ONE_WIRE_BUS);  
  udpServer= new UDPServer();
  srv = new IOTServices();
  sc = new Screen();
  if(sc) {
    udpServer->setDisplay(sc);
    srv->setDisplay(sc);
  }
}

void loop()
{
  udpServer->udpServerLoop();
  srv->webSocketLoop();
  delay(10);
  /*if (srv->listenClient())
  {
    alive_by_led();
  }*/
    
  //Section periodic operations UDP
  nLoopUDP++;
  if(nLoopUDP%300==0){
    nLoopUDP=0;
    udpServer->sendBaecon();
  }

  //Section periodic operations Screen
  nLoopBoard++;
  if(nLoopBoard%20==0){
    nLoopBoard=0;
    //srv->sendPingAliveToClient();
    //Update screen data
    if(sc) {
      sc->printSwitchStatus();
      sc->printAlimStatus();            
    }
  }
  
}
