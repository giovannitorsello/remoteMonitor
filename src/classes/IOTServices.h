#ifndef _IOTServices
#define _IOTServices
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "../../lib/WebSocket/WebSocketServer.h"
#include "./classes/config.h"
#include "./classes/sdcard.h"

#define UNKNOWN_PIN 0xFF
using namespace net;
EthernetServer server(80);
WebSocketServer webSocketServer(81);


void verifyClientCallback()
{
}

class IOTServices
{

private:
    const int sdPinSelect = 4;
    const int ethPinSelect = 10;
    Config *cnf = 0;
    SdCard *sd = 0;

protected:


    uint8_t getPinMode(uint8_t pin)
    {
        uint8_t bit = digitalPinToBitMask(pin);
        uint8_t port = digitalPinToPort(pin);

        // I don't see an option for mega to return this, but whatever...
        if (NOT_A_PIN == port) return UNKNOWN_PIN;

        // Is there a bit we can check?
        if (0 == bit) return UNKNOWN_PIN;

        // Is there only a single bit set?
        if (bit & (bit - 1)) return UNKNOWN_PIN;

        volatile uint8_t *reg, *out;
        reg = portModeRegister(port);
        out = portOutputRegister(port);

        if (*reg & bit)
            return OUTPUT;
        else if (*out & bit)
            return INPUT_PULLUP;
        else
            return INPUT;
    }
    
    int getPinsMode(WebSocket &ws) {
        for(int pin=0;pin<100;pin++) {
            uint8_t pinMode=getPinMode(pin);
        }
        return 0;
    }

    int getPinsData(char *buf, int nTotPins) {        
        for(int pin=0;nTotPins<100;pin++) {
            uint8_t pinData=digitalRead(pin);
            if(pinData) buf[pin]=49;
            else  buf[pin]=48;
        }
        return 0;
    }
    
public:
    IOTServices()
    {
        initWebServer();
        initWebSocket();
    }

    void sendBoardPinOutput() {
        char buffer[100];
        int len=0;
        for(int pin=0;pin<100;pin++) {
          uint8_t pinState=digitalRead(pin);
          sprintf(buffer, "{\"cmd\":\"%s\",\"pin\":\"%d\", \"state\":\"%d\"}","pinState",pin,pinState);
          len=strlen(buffer);
          webSocketServer.broadcast(WebSocket::DataType::TEXT, buffer,len);  
        }
        
    }
    
    void initWebSocket()
    {        
        webSocketServer.onConnection([](WebSocket &ws)
        {                   
            ws.onClose([](WebSocket &ws, const WebSocket::CloseCode code,const char *reason, uint16_t length){ 
                Serial.println("Client close connection."); 
            });

            ws.onMessage([](WebSocket &ws, const WebSocket::DataType dataType,const char *message, uint16_t length) { 
                char bufMessage[100];
                memset(bufMessage,0,100);
                Serial.println("Received: ");Serial.println(message);
                if(strcmp(message,"connect")==0) {sprintf(bufMessage,"{\"cmd\": \"%s\", \"status\": \"%s\", \"data\": \"\"}", message, "ok");}
                if(strncmp(message,"togglePin_",10)==0) {
                    char *pinString=(char *) message+10;                    
                    int pin=atoi(pinString);
                    pinMode(pin, OUTPUT);
                    int pinStatus=digitalRead(pin);
                    if(pinStatus==1) {
                        digitalWrite(pin,0);
                        Serial.println("Set pin off -->: ");Serial.println(pinString);
                    }
                    if(pinStatus==0) {
                        digitalWrite(pin,1);
                        Serial.println("Set pin on -->: ");Serial.println(pinString);
                    }
                    sprintf(bufMessage,"{\"cmd\": \"%s\", \"pin\": \"%d\", \"status\": \"%s\", \"data\": \"\"}", "tooglePin", pin, "ok");
                }
                if(strncmp(message,"setOnPin_",9)==0) {
                    char *pinString=(char *) message+9;                    
                    int pin=atoi(pinString);
                    pinMode(pin, OUTPUT);
                    digitalWrite(pin,1);
                    Serial.println("Set pin on -->: ");Serial.println(pinString);
                    sprintf(bufMessage,"{\"cmd\": \"%s\", \"pin\": \"%d\", \"status\": \"%s\", \"data\": \"\"}", "setOnPin", pin, "ok");
                }
                if(strncmp(message,"setOffPin_",10)==0) {
                    char *pinString=(char *) message+10;                    
                    int pin=atoi(pinString);
                    pinMode(pin, OUTPUT);
                    digitalWrite(pin,0);
                    Serial.println("Set pin off -->: ");Serial.println(pinString);
                    sprintf(bufMessage,"{\"cmd\": \"%s\", \"pin\": \"%d\", \"status\": \"%s\", \"data\": \"\"}", "setOffPin", pin, "ok");
                }
                
                int len=strlen(bufMessage);
                if(len>0)
                    ws.send(WebSocket::DataType::TEXT, bufMessage, len); 
                else {
                    sprintf(bufMessage,"{\"cmd\": \"%s\", \"status\": \"%s\", \"data\": \"\"}", message, "error");
                    ws.send(WebSocket::DataType::TEXT, bufMessage, strlen(bufMessage)); 
                }
            }); 
        });

        server.begin();
    }

    void initWebServer()
    {
        // INIT SD CARD FIRST
        sd = new SdCard();                
    }


    int webSocketLoop()
    {
        webSocketServer.listen();
        return 0;
    }

    int
    listenClient()
    {
        File webFile;
        EthernetClient client = server.available(); // try to get client
        int clientConnected = 0;
        if (client)
        { // got client?
            boolean currentLineIsBlank = true;
            while (client.connected())
            {
                if (client.available())
                {                           // client data available to read
                    char c = client.read(); // read 1 byte (character) from client
                    // last line of client request is blank and ends with \n
                    // respond to client only after last line received
                    if (c == '\n' && currentLineIsBlank)
                    {
                        // send a standard http response header
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: close");
                        client.println();

                        // Disable SD Card

                        // digitalWrite(sdPinSelect, LOW);
                        // digitalWrite(ethPinSelect, HIGH);
                        // send web page
                        SD.begin(sdPinSelect);
                        webFile = SD.open("index.htm"); // open web page file
                        // digitalWrite(sdPinSelect, HIGH);
                        // digitalWrite(ethPinSelect, LOW);

                        if (webFile)
                        {
                            clientConnected = 1;
                            while (webFile.available())
                            {
                                client.write(webFile.read()); // send web page to client
                            }
                            webFile.close();
                        }

                        break;
                    }
                    // every line of text received from the client ends with \r\n
                    if (c == '\n')
                    {
                        // last character on line of received text
                        // starting new line with next character read
                        currentLineIsBlank = true;
                    }
                    else if (c != '\r')
                    {
                        // a text character was received from client
                        currentLineIsBlank = false;
                    }
                }          // end if (client.available())
            }              // end while (client.connected())
            delay(50);     // give the web browser time to receive the data
            client.stop(); // close the connection
        }                  // end if (client)
        return clientConnected;
    }
};

#endif