#include <DNSServer.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include "ESPAsyncWebServer.h"

DNSServer dnsServer;
AsyncWebServer server(80);
String buffer = "[No New Message Yet]";

class CaptiveRequestHandler : public AsyncWebHandler {
public:
    CaptiveRequestHandler() {}
    virtual ~CaptiveRequestHandler() {}

    bool canHandle(AsyncWebServerRequest *request){
        //request->addInterestingHeader("ANY");
        return true;
    }

    void handleRequest(AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("text/html");
        response->print("<!DOCTYPE html><html><head><title>LoRa Reader</title></head><body>");
        response->print("<p>This is the message received from LoRa</p>");
        response->printf("<p>%s</p>", buffer.c_str());
        response->print("</body></html>");
        request->send(response);
    }
};


void setup(){
    Serial.begin(115200);
    //your other setup stuff...
    WiFi.softAP("esp-captive");
    dnsServer.start(53, "*", WiFi.softAPIP());
    server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
    //more handlers...
    server.begin();
}

void loop(){
    buffer = "";
    while(Serial.available()){
        char c = Serial.read();
        if(c == '\n'){
            break;
        } else{
            buffer += c;
        }
    }
    dnsServer.processNextRequest();
}