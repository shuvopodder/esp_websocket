#include <WiFi.h>  
#include <WebSocketsClient.h>
WebSocketsClient webSocket;

const char *ssid = "..............";
const char *password = "..............";

const char *host = "192.168.0.3";
const int port = 8080;
const char *url = "/websocket/";

String sendCMD = "[\"SEND\\ndestination:/app/cloud\\n\\nESP\\n\\n\\u0000\"]";
String connectCMD = "[\"CONNECT\\naccept-version:1.1,1.0\\n\\nheart-beat:10000,10000\\n\\n\\u0000\"]";

bool state = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  Serial.println("Event" + String((char*)payload));

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("Disconnected!\n");
      state = false;
			break;
		case WStype_CONNECTED:
			Serial.printf("Connected to url: %s\n", payload);
      state=true;
			break;
		case WStype_TEXT:
			Serial.printf("Get text: %s\n", payload);
      if (payload[0] == 'o' && !state) {
        state = true;
        webSocket.sendTXT(connectCMD.c_str(), connectCMD.length()+1);//_connectStomp();  
      }
			// send message to server
			break;
		case WStype_BIN:
			Serial.printf("Get binary length: %u\n", length);
			//hexdump(payload, length);
			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:
      state = false;
      break;			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}

//generate stomp connect url
String generateURL(){
  String stomp_url = url; 
  stomp_url += random(0, 999);
  stomp_url += "/";
  stomp_url += random(0, 999999);
  stomp_url += "/websocket";

  return stomp_url;   
}

void setup() {
  Serial.begin(115200);
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
    
  Serial.print(", Connecting to server.");
    
	// server address, port and URL
  String socketUrl = generateURL();
  //webSocket.begin(IP, Port, socketUrl);
  webSocket.begin(host, port , socketUrl);

	// event handler
	webSocket.onEvent(webSocketEvent);

	// use HTTP Basic Authorization this is optional remove if not needed
	//webSocket.setAuthorization("user", "Password");

	// try ever 5000 again if connection has failed
	webSocket.setReconnectInterval(5000);

}

void loop() {
	webSocket.loop();
  if(Serial.available()>0){
    String mgs = Serial.readStringUntil('\n'); //Serial.readString();
    mgs ="[\"SEND\\ndestination:/app/cloud\\n\\n" + mgs + "\\n\\n\\u0000\"]";
    //Serial.print(mgs);
    webSocket.sendTXT(mgs.c_str(), mgs.length() + 1);
  }
  /*if(webSocket.isConnected()){
    webSocket.sendTXT(sendCMD.c_str(), sendCMD.length() + 1); //send message
    delay(10000); //10 sec
  }*/
}
