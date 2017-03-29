/**/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <FS.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "RelayTimer.h"
#include "settings.h"



#define NUM_OF_RELAYS 4
#define RELAY1_PIN 12
#define RELAY2_PIN 13
#define RELAY3_PIN 14
#define RELAY4_PIN 16
#define RELAY1_ADDRESS 0
#define RELAY2_ADDRESS sizeof(RelayConfig) + RELAY1_ADDRESS
#define RELAY3_ADDRESS sizeof(RelayConfig) + RELAY2_ADDRESS
#define RELAY4_ADDRESS sizeof(RelayConfig) + RELAY3_ADDRESS




const char* ssid = "Zloy_Duh";
const char* password = "123qweasdzxc1234";
const char* host = "esp1";

ESP8266WebServer server(80);
//holds the current upload
File fsUploadFile;

RelayTimer r1(RELAY1_PIN, RELAY1_ADDRESS), r2(RELAY2_PIN, RELAY2_ADDRESS), r3(RELAY3_PIN, RELAY3_ADDRESS), r4(RELAY4_PIN, RELAY4_ADDRESS);

DHT dht(DHTPIN, DHTTYPE,11);

float humidity, temp_f;



#ifdef SYNCPROVIDER_NTP
#include <IPAddress.h>
IPAddress timeServer(NTP_IP_ADDRESS); //NTP server IP Address
const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

#ifdef ARDUINO_ARCH_ESP8266
#include <WiFiUdp.h>
WiFiUDP Udp;
#else
#include <Ethernet.h>
#include <EthernetUdp.h>
EthernetUDP Udp;
#endif 

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
	Serial.println("sending NTP packet...");
	// set all bytes in the buffer to 0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// Initialize values needed to form NTP request
	// (see URL above for details on the packets)
	packetBuffer[0] = 0b11100011;   // LI, Version, Mode
	packetBuffer[1] = 0;     // Stratum, or type of clock
	packetBuffer[2] = 6;     // Polling Interval
	packetBuffer[3] = 0xEC;  // Peer Clock Precision
	// 8 bytes of zero for Root Delay & Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;
	// all NTP fields have been given values, now
	// you can send a packet requesting a timestamp:                 
	Udp.beginPacket(address, 123); //NTP requests are to port 123
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();
}

time_t getNtpTime(){
	unsigned int localPort = 8888;  // local port to listen for UDP packets
	Serial.println("Starting UDP");
	Udp.begin(localPort);
	Serial.print("Local port: ");
	Serial.println(Udp.localPort());
	Serial.println("waiting for sync");

	while (Udp.parsePacket() > 0); // discard any previously received packets
	Serial.println("Transmit NTP Request");
	sendNTPpacket(timeServer);
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500) {
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE) {
			Serial.println("Receive NTP Response");
			Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
			unsigned long secsSince1900;
			// convert four bytes starting at location 40 to a long integer
			secsSince1900 = (unsigned long)packetBuffer[40] << 24;
			secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
			secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
			secsSince1900 |= (unsigned long)packetBuffer[43];
			return secsSince1900 - 2208988800UL;
		}
	}
	Serial.println("No NTP Response :-(");
	return 0; // return 0 if unable to get the time
}

#endif

void gettemperature()
{
	int runs = 0;
	do {
		delay(500);
		temp_f = dht.readTemperature(false);
		humidity = dht.readHumidity();

		if (runs > 0)
			DBG_OUTPUT_PORT.println("\nFailed to read from DHT sensor! \n");
		/*DBG_OUTPUT_PORT.println(temp_f,1);
		DBG_OUTPUT_PORT.println(humidity,1);*/
		runs++;
	} while (isnan(temp_f) && isnan(humidity));
}

on_callback fn_on(){
	DBG_OUTPUT_PORT.println("Callback for ON!");

};

off_callback fn_off(){
	
	DBG_OUTPUT_PORT.println("Callback for OFF!");

};

//format bytes
String formatBytes(size_t bytes){
	if (bytes < 1024){
		return String(bytes) + "B";
	}
	else if (bytes < (1024 * 1024)){
		return String(bytes / 1024.0) + "KB";
	}
	else if (bytes < (1024 * 1024 * 1024)){
		return String(bytes / 1024.0 / 1024.0) + "MB";
	}
	else {
		return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
	}
}

String getContentType(String filename){
	if (server.hasArg("download")) return "application/octet-stream";
	else if (filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".html")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".png")) return "image/png";
	else if (filename.endsWith(".gif")) return "image/gif";
	else if (filename.endsWith(".jpg")) return "image/jpeg";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".xml")) return "text/xml";
	else if (filename.endsWith(".pdf")) return "application/x-pdf";
	else if (filename.endsWith(".zip")) return "application/x-zip";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}

bool handleFileRead(String path){
	DBG_OUTPUT_PORT.println("handleFileRead: " + path);
	if (path.endsWith("/")) path += "index.html";
	String contentType = getContentType(path);
	String pathWithGz = path + ".gz";
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
		if (SPIFFS.exists(pathWithGz))
			path += ".gz";
		File file = SPIFFS.open(path, "r");
		size_t sent = server.streamFile(file, contentType);
		file.close();
		return true;
	}
	return false;
}

void handleFileUpload(){
	if (server.uri() != "/edit") return;
	HTTPUpload& upload = server.upload();
	if (upload.status == UPLOAD_FILE_START){
		String filename = upload.filename;
		if (!filename.startsWith("/")) filename = "/" + filename;
		DBG_OUTPUT_PORT.print("handleFileUpload Name: "); DBG_OUTPUT_PORT.println(filename);
		fsUploadFile = SPIFFS.open(filename, "w");
		filename = String();
	}
	else if (upload.status == UPLOAD_FILE_WRITE){
		DBG_OUTPUT_PORT.print("handleFileUpload Data: "); DBG_OUTPUT_PORT.println(upload.currentSize);
		if (fsUploadFile)
			fsUploadFile.write(upload.buf, upload.currentSize);
	}
	else if (upload.status == UPLOAD_FILE_END){
		if (fsUploadFile)
			fsUploadFile.close();
		DBG_OUTPUT_PORT.print("handleFileUpload Size: "); DBG_OUTPUT_PORT.println(upload.totalSize);
	}
}

void handleFileDelete(){
	if (server.args() == 0) return server.send(500, "text/plain", "BAD ARGS");
	String path = server.arg(0);
	DBG_OUTPUT_PORT.println("handleFileDelete: " + path);
	if (path == "/")
		return server.send(500, "text/plain", "BAD PATH");
	if (!SPIFFS.exists(path))
		return server.send(404, "text/plain", "FileNotFound");
	SPIFFS.remove(path);
	server.send(200, "text/plain", "");
	path = String();
}

void handleFileCreate(){
	if (server.args() == 0)
		return server.send(500, "text/plain", "BAD ARGS");
	String path = server.arg(0);
	DBG_OUTPUT_PORT.println("handleFileCreate: " + path);
	if (path == "/")
		return server.send(500, "text/plain", "BAD PATH");
	if (SPIFFS.exists(path))
		return server.send(500, "text/plain", "FILE EXISTS");
	File file = SPIFFS.open(path, "w");
	if (file)
		file.close();
	else
		return server.send(500, "text/plain", "CREATE FAILED");
	server.send(200, "text/plain", "");
	path = String();
}

void handleFileList() {
	if (!server.hasArg("dir")) { server.send(500, "text/plain", "BAD ARGS"); return; }

	String path = server.arg("dir");
	DBG_OUTPUT_PORT.println("handleFileList: " + path);
	Dir dir = SPIFFS.openDir(path);
	path = String();

	String output = "[";
	while (dir.next()){
		File entry = dir.openFile("r");
		if (output != "[") output += ',';
		bool isDir = false;
		output += "{\"type\":\"";
		output += (isDir) ? "dir" : "file";
		output += "\",\"name\":\"";
		output += String(entry.name()).substring(1);
		output += "\"}";
		entry.close();
	}

	output += "]";
	server.send(200, "text/json", output);
}

void handleArgs(){
#ifdef DEBUG_ENABLED
	for (int i = 0; i < server.args(); i++){
		DBG_OUTPUT_PORT.println("\nServer has args:");
		DBG_OUTPUT_PORT.print(server.argName(i));
		DBG_OUTPUT_PORT.print(" = ");
		DBG_OUTPUT_PORT.print(server.arg(i));
	}
	DBG_OUTPUT_PORT.println();
#endif
	if (server.args() > 0){
		if (server.hasArg("st1")){ r1.on(); }
		else { r1.off(); }
		if (server.hasArg("st2")){ r2.on(); }
		else { r2.off(); }
		if (server.hasArg("st3")){ r3.on(); }
		else { r3.off(); }
		if (server.hasArg("st4")){ r4.on(); }
		else { r4.off(); }
		if (server.hasArg("start1")){ r1.set_start(convertTime(server.arg("start1").c_str())); }
		if (server.hasArg("start2")){ r2.set_start(convertTime(server.arg("start2").c_str())); }
		if (server.hasArg("start3")){ r3.set_start(convertTime(server.arg("start3").c_str())); }
		if (server.hasArg("start4")){ r4.set_start(convertTime(server.arg("start4").c_str())); }
		if (server.hasArg("end1")){ r1.set_end(convertTime(server.arg("end1").c_str())); }
		if (server.hasArg("end2")){ r2.set_end(convertTime(server.arg("end2").c_str())); }
		if (server.hasArg("end3")){ r2.set_end(convertTime(server.arg("end3").c_str())); }
		if (server.hasArg("end4")){ r2.set_end(convertTime(server.arg("end4").c_str())); }

		
		//writeConfig(r1, RELAY1_ADDRESS);
		//writeConfig(r2, RELAY2_ADDRESS);
		//writeConfig(r3, RELAY3_ADDRESS);
		//writeConfig(r4, RELAY4_ADDRESS);
	}

}


String JSONGenerate(){

	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();
	gettemperature();

	root["heap"] = String(ESP.getFreeHeap());
	root["analog"] = String(analogRead(A0));

	JsonObject& gpio = root.createNestedObject("gpio");
	for (int pin = 0; pin <= 15; pin++){
		gpio["gpio" + String(pin)] = digitalRead(pin);
	}

	JsonObject& relay1 = root.createNestedObject("relay1");
	relay1["st1"] = String(r1.state());
	relay1["start1"] = convertTime(r1.get_start());
	relay1["end1"] = convertTime(r1.get_end());
	relay1["name1"] = r1.name();

	JsonObject& relay2 = root.createNestedObject("relay2");
	relay2["st2"] = String(r2.state());
	relay2["start2"] = convertTime(r2.get_start());
	relay2["end2"] = convertTime(r2.get_end());
	relay2["name2"] = r2.name();

	JsonObject& relay3 = root.createNestedObject("relay3");
	relay3["st3"] = String(r3.state());
	relay3["start3"] = convertTime(r3.get_start());
	relay3["end3"] = convertTime(r3.get_end());
	relay3["name3"] = r3.name();

	JsonObject& relay4 = root.createNestedObject("relay4");
	relay4["st4"] = String(r4.state());
	relay4["start4"] = convertTime(r4.get_start());
	relay4["end4"] = convertTime(r4.get_end());
	relay4["name4"] = r4.name();

	root["time"] = String(now());
	/*root["frequency"] = ESP.getCpuFreqMHz();*/
	/*root["temperature"] = dht.readTemperature(false);
	root["humidity"] = dht.readHumidity();*/
	String json = String();
	
	root.printTo(json);
	return json;
}


void setup(void){
#ifdef DEBUG_ENABLED
	DBG_OUTPUT_PORT.begin(115200);
	DBG_OUTPUT_PORT.print("\n");
	DBG_OUTPUT_PORT.setDebugOutput(true);
#endif
	// FS init
	SPIFFS.begin();
	{
		Dir dir = SPIFFS.openDir("/");
		while (dir.next()) {
			String fileName = dir.fileName();
			size_t fileSize = dir.fileSize();
#ifdef DEBUG_ENABLED
			DBG_OUTPUT_PORT.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
#endif
		}

	}

	//Wi-Fi init
	if (String(WiFi.SSID()) != String(ssid)) {
		WiFi.begin(ssid, password);
	}
#ifdef DEBUG_ENABLED
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);

		DBG_OUTPUT_PORT.print(".");

	}
	DBG_OUTPUT_PORT.println("");
	DBG_OUTPUT_PORT.print("Connected! IP address: ");
	DBG_OUTPUT_PORT.println(WiFi.localIP());
#endif
	/*MDNS.begin(host);
	DBG_OUTPUT_PORT.print("Open http://");
	DBG_OUTPUT_PORT.print(host);
	DBG_OUTPUT_PORT.println(".local/edit to see the file browser");*/

#ifdef SYNCPROVIDER_RTC
	setSyncProvider(RTC.get);
#elif defined SYNCPROVIDER_NTP
	//#ifdef ARDUINO_ESP8266_GIT_VER
	setSyncProvider(getNtpTime);
	//#endif
#endif
	// Check time sync
	if (timeStatus() == timeNotSet) {
#ifdef DEBUG_ENABLED
		DBG_OUTPUT_PORT.println("Time is not set!"); now();
	}
	DBG_OUTPUT_PORT.print("Current timestamp: ");
	DBG_OUTPUT_PORT.println(now());
#endif
	// Init relays
	r1.begin();
	r2.begin();
	r3.begin();
	r4.begin();
	//
	r1.rename("Relay 1");
	r2.rename("Relay 2");
	r3.rename("Relay 3");
	r4.rename("Relay 4");


	//SERVER INIT	//list directory
	server.on("/list", HTTP_GET, handleFileList);
	/*
	//load editor
	server.on("/edit", HTTP_GET, [](){
	if (!handleFileRead("/edit.htm")) server.send(404, "text/plain", "FileNotFound");
	});
	//create file
	server.on("/edit", HTTP_PUT, handleFileCreate);
	//delete file
	server.on("/edit", HTTP_DELETE, handleFileDelete);
	//first callback is called after the request has ended with all parsed arguments
	//second callback handles file uploads at that location
	server.on("/edit", HTTP_POST, [](){ server.send(200, "text/plain", ""); }, handleFileUpload);

	//called when the url is not defined here
	//use it to load content from SPIFFS
	*/

	server.onNotFound([](){
		if (!handleFileRead(server.uri()))
			server.send(404, "text/plain", "FileNotFound");
	});

	//get heap status, analog input value and all GPIO statuses in one json call
	server.on("/all", HTTP_GET, [](){

		handleArgs();


		server.send(200, "text/json", JSONGenerate());

	});
	server.begin();
	DBG_OUTPUT_PORT.println("HTTP server started");
}

void loop(void){
	
	r1.process();
	
	r2.process();
	
	server.handleClient();
	
	
	/*r3.process();
	r4.process();*/
}
