#ifndef _UDPServer
#define _UDPServer
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include "../constants.h"
#include "./classes/config.h"
#include "./classes/sdcard.h"
#include "./classes/screen.h"
#define UDP_TX_PACKET_MAX_SIZE 100


byte mac[] = {0x16, 0xAD, 0xA4, 0xD2, 0x1A, 0x01};
char packetBuffer[UDP_TX_PACKET_MAX_SIZE];
char replyBuffer[UDP_TX_PACKET_MAX_SIZE]; 
char pinStatusArray[NUM_TOT_PIN];

class UDPServer
{

private:
    Screen *sc = 0;
    Config *cnf = 0;
    SdCard *sd = 0;
    EthernetUDP Udp;

protected:

public:
    UDPServer()
    {                
        initUdpServer();
    }

    void setDisplay(Screen *scr)
    {
        sc=scr;
    }   

    void initUdpServer() {

        cnf = new Config();
        applyNetworkConfig();
                    
        if (Ethernet.hardwareStatus() == EthernetNoHardware)
            Serial.println("Ethernet shield was not found");
        if (Ethernet.linkStatus() == LinkOFF)
            Serial.println("Ethernet cable is not connected.");
    
        Serial.print("Arduino's IP Address: ");
        Serial.println(Ethernet.localIP());        

        Serial.print("DNS Server's IP Address: ");
        Serial.println(Ethernet.dnsServerIP());

        Serial.print("Gateway's IP Address: ");
        Serial.println(Ethernet.gatewayIP());

        Serial.print("Network's Subnet Mask: ");
        Serial.println(Ethernet.subnetMask());
    
        Serial.println("UDP server is starting...");
        cnf=new Config();
        Udp.begin(3000);
    }

    int udpServerLoop() {
        
        memset(packetBuffer,0,UDP_TX_PACKET_MAX_SIZE);
        memset(replyBuffer,0,UDP_TX_PACKET_MAX_SIZE);
        int packetSize = Udp.parsePacket();
        if (packetSize>0 && packetSize<UDP_TX_PACKET_MAX_SIZE) {
            Serial.print("Received packet of size ");
            Serial.println(packetSize);
            Serial.print("From ");
            IPAddress remote = Udp.remoteIP();
            for (int i=0; i < 4; i++) {
                Serial.print(remote[i], DEC);
                if (i < 3) {
                    Serial.print(".");
                }
            }
            Serial.print(", port ");
            Serial.println(Udp.remotePort());

            // read the packet into packetBuffer
            Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
            Serial.println("Received command:");
            Serial.println(packetBuffer);
            parseCommand(packetBuffer);
            
        }
        return packetSize;
    }

    void applyNetworkConfig() {
        Serial.println("Applying configuration.");
        _configData cnfData = cnf->getConfig();
        byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
        IPAddress myIP(cnfData.ip_address[0], cnfData.ip_address[1], cnfData.ip_address[2], cnfData.ip_address[3]);
        IPAddress myNM(cnfData.netmask[0], cnfData.netmask[1], cnfData.netmask[2], cnfData.netmask[3]);
        IPAddress myGW(cnfData.gateway[0], cnfData.gateway[1], cnfData.gateway[2], cnfData.gateway[3]);
        IPAddress myDNS(cnfData.dns[0], cnfData.dns[1], cnfData.dns[2], cnfData.dns[3]);
        Ethernet.begin(mac, myIP, myDNS, myGW, myNM);
        Udp.begin(3000);
        Serial.println("Configuration applied.");
        if(sc) sc->printNetworkParams(Ethernet.localIP(), Ethernet.subnetMask(), Ethernet.gatewayIP(), Ethernet.dnsServerIP());        
    }

    void parseCommand(char bufCommand[UDP_TX_PACKET_MAX_SIZE]) {
        if(strncmp(bufCommand, "wfn set ip=", 11)==0) {
            char * ipString=bufCommand+11;
            unsigned char ipv4[4];
            char bufOut[100];
            Serial.println("Change IP to");
            Serial.println(ipString);
            sscanf(ipString,"%d.%d.%d.%d",ipv4,ipv4+1,ipv4+2,ipv4+3);
            cnf->setIpAddress(ipv4);
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn set nm=", 11)==0) {
            char * ipString=bufCommand+11;
            unsigned char ipv4[4];
            char bufOut[100];
            Serial.println("Change Netmask to");
            Serial.println(ipString);
            sscanf(ipString,"%d.%d.%d.%d",ipv4,ipv4+1,ipv4+2,ipv4+3);
            cnf->setNetmask(ipv4);
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn set gw=", 11)==0) {
            char * ipString=bufCommand+11;
            unsigned char ipv4[4];
            char bufOut[100];
            Serial.println("Change Gateway to");
            Serial.println(ipString);
            sscanf(ipString,"%d.%d.%d.%d",ipv4,ipv4+1,ipv4+2,ipv4+3);
            cnf->setGateway(ipv4);
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn set dns=", 12)==0) {
            char * ipString=bufCommand+12;
            unsigned char ipv4[4];
            char bufOut[100];
            Serial.println("Change DNS to");
            Serial.println(ipString);
            sscanf(ipString,"%d.%d.%d.%d",ipv4,ipv4+1,ipv4+2,ipv4+3);
            cnf->setDNS(ipv4);
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn set server=", 15)==0) {
            char * ipString=bufCommand+15;
            unsigned char ipv4[4];
            char bufOut[100];
            Serial.println("Change DNS to");
            Serial.println(ipString);
            sscanf(ipString,"%d.%d.%d.%d",ipv4,ipv4+1,ipv4+2,ipv4+3);
            cnf->setServerIP(ipv4);
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn set srvport=", 16)==0) {
            char * portString=bufCommand+16;
            unsigned int port;
            char bufOut[100];
            Serial.println("Change Server port to");
            Serial.println(portString);
            sscanf(portString,"%d",&port);            
            cnf->setServerPort(port);
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn set srvurl=", 15)==0) {
            char * serverUrl=bufCommand+15;
            cnf->setServerUrl(serverUrl);
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn set name=", 13)==0) {
            char * deviceName=bufCommand+13;
            cnf->setDeviceName(deviceName);
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn set default", 15)==0) {    
            cnf->setDefaultConfig();
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn print configuration", 23)==0) {
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn apply configuration", 23)==0) {       
            applyNetworkConfig();            
            cnf->printConfiguration();
        }

        if(strncmp(bufCommand, "wfn print pin status", 20)==0) {
            char bufToSend[UDP_TX_PACKET_MAX_SIZE];       
            for(int i=0;i<NUM_TOT_PIN;i++){
                int pinStatus=digitalRead(i);
                pinStatusArray[i]=pinStatus;
                sprintf(bufToSend,"pin %d status ->%d\n",i,pinStatus);
                sendString(bufToSend);
            }
        }

        if(strncmp(bufCommand, "wfn set pin on=", 15)==0) {
            char * pinString=bufCommand+15;
            unsigned int pin;
            char bufOut[100];
            Serial.println("Change pin status to on");
            Serial.println(pinString);
            sscanf(pinString,"%d",&pin);            
            digitalWrite(pin,HIGH);
        }

        if(strncmp(bufCommand, "wfn set pin off=", 16)==0) {
            char * pinString=bufCommand+16;
            unsigned int pin;
            char bufOut[100];
            Serial.println("Change pin status to on");
            Serial.println(pinString);
            sscanf(pinString,"%d",&pin);            
            digitalWrite(pin,LOW);
        }

        sendOK();
    }
    
    void sendString(char *bufToSend) {        
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(bufToSend);
        Udp.endPacket();
    }

    void sendOK() {        
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write("OK\n");
        Udp.endPacket();
    }

    void sendBaecon() {
        char bufBaecon[100];
        _configData cnfData=cnf->getConfig();
        sprintf(bufBaecon,"%s",cnfData.name);
        IPAddress broadcast(255,255,255,255);
        Udp.beginPacket(broadcast, 3000);
        Udp.write(bufBaecon);
        Udp.endPacket();        
    }
};

#endif