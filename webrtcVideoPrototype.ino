#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h> 
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include <FS.h>

#include <string.h>
#include <ArduinoJson.h>

int ledPin1 = 5; 
int ledPin2 = 4; 
const byte DNS_PORT = 53;
DNSServer dnsServer;
// Update to contain your network information :)
const char *ssid = "SSIDNAME";
const char *password = "PASSWORD";

// A few variables which we'll need throughout the program

uint8_t Offer_Answer=255;
uint8_t CreaterNumber=255;
uint8_t JointNumber=255;
String offerToken="";
String answerToken="";
String ledToken="";
uint8_t offerflag=0;
uint8_t answerflag=0;
// Initialise websockets, web server and servo
WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80);
IPAddress ip(192, 168, 2, 2); 
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0); 

void setup(void){
  // Wait a second before we jump into starting everything
  delay(1000);
  
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring wifi...");

  // If you'd like to use the ESP as a wifi access point instead of a client, 
  // comment out everything between HERE...

  //WiFi.begin(ssid, password);
  //while (WiFi.status() != WL_CONNECTED) {
  //  delay(500);
  //  Serial.print(".");
  //}
  //IPAddress myIP = WiFi.localIP();

  // AND HERE... Then uncomment these 2 lines:
  WiFi.softAP(ssid,password);
  WiFi.softAPConfig(ip, gateway, subnet);
  IPAddress myIP = WiFi.softAPIP();

  // Print out the IP address
  Serial.print("IP address: ");
  Serial.println(myIP);


  pinMode(ledPin1, OUTPUT);
  digitalWrite(ledPin1, LOW);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin2, LOW);
  // Begin access to our file system (which stores the HTML)
  SPIFFS.begin();

  // Start the websockets and link the events function (defined below, under the loop)
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "esp", ip);
  // Configure web server to host HTML files
  server.onNotFound([](){
    if(!handleFileRead(server.uri()))
      server.send(404, "text/plain", "FileNotFound");
  });
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  // Process any incoming HTTP or WebSockets requests
  webSocket.loop();
  server.handleClient();
  dnsServer.processNextRequest();
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
  
  
}

// A function to handle our incoming sockets messages
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {

  switch(type) {

    // Runs when a user disconnects
    case WStype_DISCONNECTED: {
      Serial.printf("User #%u - Disconnected!\n", num);
      break;
    }
    
    // Runs when a user connects
    case WStype_CONNECTED: {
      IPAddress ip = webSocket.remoteIP(num);
      Serial.printf("--- Connection. IP: %d.%d.%d.%d Namespace: %s UserID: %u\n", ip[0], ip[1], ip[2], ip[3], payload, num);
      break;
    }

    // Runs when a user sends us a message
    case WStype_TEXT: {
      Serial.printf("UserID: %u,Namespace: %s \n",num, payload);
      DynamicJsonBuffer jsonBuffer(100);
      JsonObject& parsed = jsonBuffer.parseObject(payload); //Parse message
      const char * sensorType=parsed["type"];
       if (strcmp(sensorType,"offer")==0)
      {Offer_Answer=0;
     }
      else if (strcmp(sensorType,"answer")==0)
      {Offer_Answer=1;
      }
      else if (strcmp(sensorType,"led")==0)
      {
        Offer_Answer=2;
        parsed.printTo(ledToken);
            if(strcmp(parsed["no"],"1")==0)
            {
              if (strcmp(parsed["status"],"ON")==0)
              {digitalWrite(ledPin1, HIGH);}
              else
              {digitalWrite(ledPin1, LOW);}
             }
             else if(strcmp(parsed["no"],"2")==0)
            {
              if (strcmp(parsed["status"],"ON")==0)
              {digitalWrite(ledPin2, HIGH);}
              else
              {digitalWrite(ledPin2, LOW);}
             }
        webSocket.broadcastTXT(ledToken);
        ledToken="";
      }
      else
      {Offer_Answer=3;
      }
  

      if(Offer_Answer==0&& answerflag==0){
        CreaterNumber=num;
        parsed.printTo(offerToken);
        answerflag=1;
        }
      
      else if (Offer_Answer==1&& offerflag==0){
        
        JointNumber=num;
        webSocket.sendTXT(JointNumber,offerToken);
        offerflag=1;
        }
      else if(Offer_Answer==1&& answerflag==1&&offerflag==1){
        
        parsed.printTo(answerToken);
        webSocket.sendTXT(CreaterNumber,answerToken);
 
        }
      Serial.printf(" CreaterNumber:%u",CreaterNumber);
      Serial.printf(" JointNumber:%u",JointNumber);
      Serial.printf(" \r\n");
      break;
    }
    
  }

}

// A function we use to get the content type for our HTTP responses
String getContentType(String filename){
  if(server.hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

// Takes a URL (for example /index.html) and looks up the file in our file system,
// Then sends it off via the HTTP server!
bool handleFileRead(String path){
 // #ifdef DEBUG
    Serial.println("handleFileRead: " + path);
 // #endif
if(path.endsWith("create.html")){offerflag=0;offerToken="";Serial.println("create linked" );};
if(path.endsWith("join.html")) {answerflag=0;answerToken="";Serial.println("join linked" );}
  if(SPIFFS.exists(path)){
    File file = SPIFFS.open(path, "r");
    size_t sent = server.streamFile(file, getContentType(path));
    file.close();
    return true;
  }
  return false;
}

