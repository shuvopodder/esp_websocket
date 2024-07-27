#include <WiFi.h>  
#include <WebSocketsClient.h>
WebSocketsClient webSocket;

const char *ssid = "..............";
const char *password = "..............";//"..............

const char *host = "192.168.0.3";
const int port = 8080;
const char *url = "/websocket/";

String sendCMD = "[\"SEND\\ndestination:/app/cloud\\n\\nESP\\n\\n\\u0000\"]"; //websocket destination : /app/cloud
String connectCMD = //"[\"CONNECT\\naccept-version:1.1,1.0\\n\\nheart-beat:10000,10000\\n\\n\\u0000\"]";
                    "[\"CONNECT\\naccept-version:1.1,1.0\\nheart-beat:10000,10000\\n\\n\\u0000\"]";
String subscribeCMD = "[\"SUBSCRIBE\\nid:sub-0\\ndestination:/topic/cloud\\n\\n\\u0000\"]"; //subscribe channel : /topic/cloud

//bool state = false;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  Serial.println("Event " + String((char*)payload));

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.printf("Disconnected!\n");
      //state = false;
			break;
		case WStype_CONNECTED:
			Serial.printf("Connected to url: %s\n", payload);
      //state=true;
      //webSocket.sendTXT(connectCMD.c_str(), connectCMD.length()+1);//_connectStomp();  
      //Serial.print("connectCMD");
			break;
		case WStype_TEXT:
    {
      //Serial.printf("Get text: %s\n", payload); 
      String text = (char*) payload;
      if (payload[0] == 'o'){ // && !state) {
        //state = true;
        // on open connection
        webSocket.sendTXT(connectCMD.c_str(), connectCMD.length()+1);//_connectStomp();  
        Serial.print("connectCMD");
      } else if(payload[0] == 'h'){
        Serial.print("Got a HeartBeat!");
      } else if (text.startsWith("a[\"CONNECTED")) {
        // subscribe to /topic/cloud channel
        webSocket.sendTXT(subscribeCMD.c_str(), subscribeCMD.length()+1); 
        Serial.print("Subscribed!");
      } else if(text.startsWith("a[\"MESSAGE")){
        int start = text.indexOf("{"); // last indexof /n
        int end = text.lastIndexOf("}");// last index of /u
        String message = text.substring(start,end+1);
        Serial.print("Got a message!\n");//["MESSAGE\ndestination:/topic/cloud\ncontent-type:text/plain;charset=UTF-8\nsubscription:sub-0\nmessage-id:704719-20\ncontent-length:17\n\n{\"message\":\"Hello ESP\"}\u0000"]
        Serial.println(message);
        
      } else if(payload[0] == 'a'){
        Serial.print("a");
        // subscribe to /topic/cloud channel
        //webSocket.sendTXT(subscribeCMD.c_str(), subscribeCMD.length()+1); 
        //Serial.print("Subscribed!");
      }
			break;
    }
		case WStype_BIN:
			Serial.printf("Get binary length: %u\n", length);
			//hexdump(payload, length);
			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:
      Serial.print("Connection Error!");
      //state = false;
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
  //read from serial
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
