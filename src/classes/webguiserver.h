#ifndef _WEBGUISERVER
#define _WEBGUISERVER
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include "./classes/config.h"
#include "./classes/sdcard.h"
EthernetServer server(80);

class WebGuiServer
{

private:
    const int sdPinSelect = 4;
    const int ethPinSelect = 10;
    Config *cnf = 0;
    SdCard *sd = 0;

public:
    WebGuiServer()
    {
        initServer();
    }

    void initServer()
    {
        // INIT SD CARD FIRST
        sd = new SdCard();

        // Disable SD Card
        // pinMode(sdPinSelect, OUTPUT);
        // pinMode(ethPinSelect, OUTPUT);
        // digitalWrite(sdPinSelect, HIGH);
        // digitalWrite(ethPinSelect, LOW);

        // initialize the Ethernet shield using DHCP:
        Serial.println("Obtaining an IP address using DHCP");
        byte mac[] = {0x16, 0xAD, 0xA4, 0xD2, 0x1A, 0x01};
        IPAddress ip(192, 168, 1, 12);
        // Ethernet.begin(mac, ip);
        Ethernet.init(10);
        if (Ethernet.begin(mac) == 0)
        {
            Serial.println("Failed to obtaining an IP address");
            if (Ethernet.hardwareStatus() == EthernetNoHardware)
                Serial.println("Ethernet shield was not found");
            if (Ethernet.linkStatus() == LinkOFF)
                Serial.println("Ethernet cable is not connected.");
        }
        else
        {
            Serial.print("Arduino's IP Address: ");
            Serial.println(Ethernet.localIP());

            Serial.print("DNS Server's IP Address: ");
            Serial.println(Ethernet.dnsServerIP());

            Serial.print("Gateway's IP Address: ");
            Serial.println(Ethernet.gatewayIP());

            Serial.print("Network's Subnet Mask: ");
            Serial.println(Ethernet.subnetMask());
        }
        /*cnf = new Config();
        _configData cnfData = cnf->getConfig();
        cnf->printConfiguration();
        // Select the IP address according to your local network
        byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
        IPAddress myIP(cnfData.ip_address[0], cnfData.ip_address[1], cnfData.ip_address[2], cnfData.ip_address[3]);
        IPAddress mySN(cnfData.netmask[0], cnfData.netmask[1], cnfData.netmask[2], cnfData.netmask[3]);
        IPAddress myGW(cnfData.gateway[0], cnfData.gateway[1], cnfData.gateway[2], cnfData.gateway[3]);
        IPAddress myDNS(cnfData.dns[0], cnfData.dns[1], cnfData.dns[2], cnfData.dns[3]);

        Ethernet.begin(mac, myIP, myDNS, myGW, mySN);*/
    }

    int listenClient()
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