﻿
//#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <NeoPixelAnimator.h>
#include "bitmaps.h"
#include <ESPmDNS.h>
#include <BlynkSimpleEsp32_SSL.h>
#include <Time.h>
#include <SPI.h>
#include <NeoPixelBrightnessBus.h>
#include <OneWire.h>
#include <DS18B20.h>
#include <Preferences.h>
#include <PageBuilder.h>

/*********** EEPROM Speichern ********************/

Preferences preferences; 

/********** Auto Connect *************************/
#include <WiFi.h>
//#include <WiFiClientSecure.h>
#include <WebServer.h>
#include <EEPROM.h> 
#include <AutoConnect.h>
#define EXTERNAL_SWITCH_PIN 21

/*** Extra Menue für Blynk Token Eingabe ***/

static const char addonJson[] PROGMEM = R"raw(
{
  "title": "Blynk Token",
  "uri": "/blynktoken",
  "menu": true,
  "element": [
    {
      "name": "blyto",
      "type": "ACInput",
      "label": "Hier Blynk Token eingeben"
    },
    {
      "name": "send",
      "type": "ACSubmit",
      "value": "Save",
      "uri": "/blyto"
    }
  ]
}
)raw";

WebServer Server;
AutoConnect portal(Server);
AutoConnectConfig config;
String BlynkTokenEE;
char BlynkToken[34];

	// fuer umrechnung string zu char noetig
	
	//**************************************
void blytoOn() {

  // Retrieve the value of a input-box named "feels"
  String token = Server.arg("blyto");
	//int str_len = token.length() + 1; 
	//char BlynkToken[str_len];

  // Echo back the value
  String echo = "<html><p style=\"color:blue;font-family:verdana;font-size:300%;\">" + token + String(" and a bold world!</p></html>");
  Server.send(200, "text/html", echo);
  Serial.println (token);
	
	// Umrechnung string zu char 
	token.toCharArray(BlynkToken, 35);
	BlynkTokenEE = token;
	preferences.putString("BlyTo", BlynkTokenEE);
	//*************************************
	Serial.println (BlynkToken);

}

/***********  NeoPixel Einstellungen   ***********/

#define PIN_STRIPE1			13
#define NUMLEDS				166

NeoPixelBrightnessBus<NeoGrbwFeature, NeoEsp32Rmt0800KbpsMethod> strip1(NUMLEDS, PIN_STRIPE1);

/************ TFT Einstellungen ***************/

TFT_eSPI tft = TFT_eSPI();
#define MAX_GEN_COUNT 500

/************* One Wire (Tempfühler) **********/

#define ONE_WIRE_BUS			26				// Anschlusspin für OneWire			
OneWire oneWire(ONE_WIRE_BUS);
DS18B20 Tempfueh(&oneWire);

/********** Blynk und WiFi Einstellungen ******/

BlynkTimer Timer;

//char auth[] = "06a15068bcdb4ae89620f5fd2e67c672";

/****** BETA Token *****************************/

//char auth[] = "HI89YVOp5X0dR6ycdXnP6WHd3XT4gmQv";

char serverblynk[] = "blynk-cloud.com";
unsigned int port = 8442;

/******* Variablen *******************************/

uint8_t TFTRotation = 3;

uint8_t wifi_retry = 0;

uint8_t SoAuStd = 10;
uint8_t SoAuMin = 00;
uint8_t SoAuZei;
uint8_t SoUnStd = 20;
uint8_t SoUnMin = 00;
uint8_t SoUnZei;
uint8_t SoNaStd = 22;
uint8_t SoNaMin = 00;

uint8_t SoMiAnStd = 13;
uint8_t SoMiAnMin = 00;
uint8_t SoMiAusStd = 15;
uint8_t SoMiAusMin = 00;

uint8_t FutterStd = 00;
uint8_t FutterMin = 00;

uint8_t maxHell = 250;
uint8_t minHell = 0;
uint8_t mittagHell = 5;
uint8_t aktHell = 200;

float_t SollTemp = 25.0;
float_t IstTemp;
float_t LuefTemp = 30.0;
float_t Hysterese = 0.30;

uint8_t CO2AnStd = 11;
uint8_t CO2AnMin = 00;
uint8_t CO2AusStd = 18;
uint8_t CO2AusMin = 00;

uint16_t Zeit;

uint8_t AblaufX = 0;
uint8_t AblaufY = 159;
uint8_t AblaufI = 0;

uint8_t BacklightPin = 22;
uint16_t BacklightFrequenz = 500;
uint8_t BacklightKanalTFT = 2;
uint8_t BacklightBit = 8;
uint8_t BacklightwertTag = 100;
uint8_t BacklightwertNacht = 100;

uint8_t FutterPin = 14;
uint16_t Futterfreq = 250;
uint8_t FutterKanal = 0;
uint8_t FutterRes = 8;
uint16_t Futtergesch = 180;
uint16_t Futterdauer = 2000;

uint8_t PowerledPin = 16;
uint16_t Powerledfreq = 2000;
uint8_t PowerledKanal = 1;
uint8_t PowerledBit = 8;
uint8_t Powerledwert = 0;
uint8_t Powerledwert_virtuell = 0;
uint8_t Powerledmax = 250;
uint8_t Powerledmin = 0;

uint8_t LEDRot;
uint8_t LEDGruen;
uint8_t LEDBlau;
uint8_t LEDWeiss;

unsigned long PowerLEDMillis = 0;
unsigned long FutterMillis = 0;

/**************** Pin Belegung *******************/

#define heizung				12      //Pin Relais Heizung
#define luefter				25      //Pin Lüfter
#define co2					27		//Pin Relais CO2 Abschaltung


/**************** NeoPixel Init ******************/

// Sonnenaufgang Color Array
//				{ R, G , B, W }
int SonAu1[4] = { 30,0,0,0 };
int SonAu2[4] = { 150,5,0,0 };
int SonAu3[4] = { 157,13,0,0 };
int SonAu4[4] = { 163,21,1,0 };
int SonAu5[4] = { 200,30,60,0 };
int SonAu6[4] = { 240,50,100,100 };
int SonAu7[4] = { 230,60,240,200 };

// Sonnenuntergang Color Array
//				{ R, G, B, W }
int SonUn1[4] = { 250,60,100,40 };
int SonUn2[4] = { 240,50,60,0 };
int SonUn3[4] = { 200,30,40,0 };
int SonUn4[4] = { 150,20,0,0 };
int SonUn5[4] = { 50, 14,1, 0 };
int SonUn6[4] = { 0, 0, 6, 0 };
int SonUn7[4] = { 0, 0, 4, 0 };

//Nachtlicht AUS Color Array
//					 { R, G, B, W }
int Nachtlicht1[4] = { 0, 0, 0, 0 };

// Set initial color
uint8_t redVal = 0;
uint8_t grnVal = 0;
uint8_t bluVal = 0;
uint8_t whiteVal = 0;

uint16_t DurchWait = 180;
unsigned long CrossLEDMillis = 0;

uint8_t prevR = redVal;
uint8_t prevG = grnVal;
uint8_t prevB = bluVal;
uint8_t prevW = whiteVal;
uint16_t LEDStep = 0;
uint8_t Durchlauf = 1;
uint8_t SonneIndex = 0;
uint8_t FutterIndex = 0;

uint16_t StepWert = 256;

/******** BLYNK FUNKTIONEN  ********************/

WidgetLCD lcd(V127);
WidgetLED ledluefter(V22);
WidgetLED ledheizung(V23);
WidgetLED ledco2(V24);
WidgetLED led(V25);


/***** NTP Server abrufen für Local Time ********/

static const char ntpServerName[] = "de.pool.ntp.org";
const int timeZone = 1;

const uint32_t syncIntervalMax = 300; // could be the same value like the syncInterval in time.cpp | 300
const uint32_t syncIntervalAfterFail = 30; // if sync with NTP fails, retry earlier | 60
const uint32_t ntpWaitMax = 1500; // maximum time in ms to wait for an answer of NTP Server, most used value 1500 I prefere below one second: 900
uint32_t ntpWaitActual = ntpWaitMax; // optimized/reduced wait time, start with maximum.

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

const int NTP_PACKET_SIZE = 48;		// NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

/************** Funktionen *********************/

#include <ProgFunk.h>
#include <tft.h>
#include <timer.h>
#include <blynkfunk.h>


/************************** NTP code ********************************************/

time_t getNtpTime()
{
	IPAddress ntpServerIP; // NTP server's ip address

	while (Udp.parsePacket() > 0); // discard any previously received packets
	//Serial.println("Transmit NTP Request");
	// get a random server from the pool
	WiFi.hostByName(ntpServerName, ntpServerIP);
	//Serial.print(ntpServerName);
	//Serial.print(": ");
	//Serial.println(ntpServerIP);
	sendNTPpacket(ntpServerIP);
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500) {
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE) {
			//Serial.println("Receive NTP Response");
			Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
			unsigned long secsSince1900;
			// convert four bytes starting at location 40 to a long integer
			secsSince1900 = (unsigned long)packetBuffer[40] << 24;
			secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
			secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
			secsSince1900 |= (unsigned long)packetBuffer[43];
			setSyncInterval(syncIntervalMax);
			return secsSince1900 - 2208988800UL; // Important: Systemtime = UTC!!!

		}
	}
	//Serial.println("No NTP Response :-(");
	tft.setTextColor(TFT_BLACK);
	tft.drawString("           ", 3, 2, 2);
	tft.setTextColor(TFT_RED);
	tft.drawString("8", 3, 2, 2);
	tft.drawString("8", 12, 2, 2);
	tft.drawString(":", 21, 2, 2);
	tft.drawString("8", 25, 2, 2);
	tft.drawString("8", 34, 2, 2);
	tft.drawString(":", 43, 2, 2);
	tft.drawString("8", 48, 2, 2);
	tft.drawString("8", 57, 2, 2);

	if (ntpWaitActual + 200 < ntpWaitMax) ntpWaitActual += 200; // expand wait time if necessary - I'm pretty aware of that this value will hardly reach ntpWaitMax with this simple condition.
	setSyncInterval(syncIntervalAfterFail); // in both cases: no NTP response or no DNS lookup

	return 0; // return 0 if unable to get the time
}

void WIFI_login() 
{
	tft.drawBitmap(140, 0, wlan, 20, 20, TFT_GREEN);
	Serial.println("WiFi Login");
	BlynkTokenEE.toCharArray(BlynkToken, 35);
	Serial.print ("BlynkToken: ");
	Serial.println (BlynkToken);
	Blynk.config(BlynkToken);
	Blynk.connect();
	/*while (WiFi.status() != WL_CONNECTED && wifi_retry <= 10) {
		wifi_retry++;
		tft.drawBitmap(140, 0, wlan, 20, 20, TFT_RED);
		//WiFi.persistent(false);   // daten nicht in Flash speichern
		//WiFi.disconnect();
      	//WiFi.mode(WIFI_OFF);
      	//WiFi.mode(WIFI_STA);
		//WiFi.begin(ssid, pass);
		Blynk.config(BlynkToken);
		Blynk.connect();
		//Blynk.syncAll();		
	}

	if (wifi_retry >= 11) {
		wifi_retry = 0;
		Serial.println("\nReboot");
		ESP.restart();
	}*/
}
/********************************************************************************/


void setup() 
{

	Serial.begin(115200);

	/************ TFT Layout setzen ***************/  

	TFT_Layout();

	/************** EEPROM auslesen ***************/

	preferences.begin("aq", false);                         //Ordner az anlegen/verwenden

  	//preferences.clear();                                  //Alle "Dateien" unter dem Ordner aq löschen

 	//preferences.remove("aq");                          	// EEPROM Datei "StartS" löschen
	SoAuStd = preferences.getUInt("StartS", 0); 			
	SoAuMin = preferences.getUInt("StartM", 0);
	SoUnStd = preferences.getUInt("StopS", 0);
	SoUnMin = preferences.getUInt("StopM", 0);
	BacklightwertNacht = preferences.getUInt("BackLN", 0);
	BacklightwertTag = preferences.getUInt("BackLT", 0);
	TFTRotation = preferences.getInt("TFTR", 0);
	FutterStd = preferences.getUInt("FuttS", 0);
	FutterMin = preferences.getUInt("FuttM", 0);
	DurchWait = preferences.getUInt("Wait", 0);
	Hysterese = preferences.getFloat("Hyst", 0);
	maxHell = preferences.getUInt("MaxH", 0);
	mittagHell = preferences.getUInt("MitH", 0);
	Powerledmax = preferences.getUInt("PowH", 0);
	Futterdauer = preferences.getUInt("FutD", 0);
	Futtergesch = preferences.getUInt("FutG", 0);
	SollTemp = preferences.getUInt("SollT", 0);
	LuefTemp = preferences.getUInt("LueT", 0);
	CO2AnStd = preferences.getUInt("COSS", 0);
	CO2AnMin = preferences.getUInt("COSM", 0);
	CO2AusStd = preferences.getUInt("COAS", 0);
	CO2AusMin = preferences.getUInt("COAM", 0);
	SoNaStd = preferences.getUInt("NaLS", 0);
	SoNaMin = preferences.getUInt("NaLM", 0);
	SoMiAusStd = preferences.getUInt("MiAuLS", 0);
	SoMiAusMin = preferences.getUInt("MIAuLM", 0);
	SoMiAnStd = preferences.getUInt("MiAnLS", 0);
	SoMiAnMin = preferences.getUInt("MIAnLM", 0);
	BlynkTokenEE = preferences.getString("BlyTo");

	Serial.print ("Blynk EE: ");
	Serial.println (BlynkTokenEE);

	pinMode(2, OUTPUT);
	
	/******* AutoConnect initialisieren *********/

	Server.on("/blyto", blytoOn);  // Register /feels handler  // Register /feels handler
  	portal.load(addonJson);           // Load a custom Web page
	config.ota = AC_OTA_BUILTIN;
	config.autoReconnect = true;
	config.ticker = true;
	config.tickerPort = 2;
	config.tickerOn = HIGH;
  	portal.config(config);
  	portal.begin();

	

	/***** Blynk Verbinden / WIFI Verbinden *******/

	WIFI_login();

	/*********** GPIO´s definieren ****************/

	pinMode(heizung, OUTPUT);
	pinMode(luefter, OUTPUT);
	pinMode(co2, OUTPUT);


	


	CO2Timer();
	SunTimer();
	FutterTimer();
	

	/**** Alarm Timer setzen für Funktionen ******/ 

	//Timer.setInterval(1000, digitalClockDisplay);
	Timer.setInterval(1000, ProgrammTimer);
	Timer.setInterval(1000, Heizung);

	/******* Blynk LCD löschen ******************/

	lcd.clear();

	/*********** Neopixel Starten ***************/

	strip1.Begin();
	strip1.ClearTo(0);
	strip1.SetPixelColor(10, RgbwColor(0, 0, 200, 0));
	strip1.SetBrightness(250);
	aktHell = maxHell;
	strip1.Show();

	/************ Tempfuehler *******************/

	Tempfueh.begin();
	Tempfueh.setResolution(10);			// 0.25 Grad Genauigkeit

	/******** TFT Backlight *******************/

	ledcSetup(BacklightKanalTFT, BacklightFrequenz, BacklightBit);	//ledcSetup(Kanal, Frequenz, Bit);
	ledcAttachPin(BacklightPin, BacklightKanalTFT);					//ledcAttachPin(Pin, Kanal);
	ledcWrite(BacklightKanalTFT, BacklightwertTag);

	/******** Futterautomat *******************/

	ledcSetup(FutterKanal, Futterfreq, FutterRes);
	ledcAttachPin(FutterPin, FutterKanal);

	/******* PowerLED ************************/

	ledcSetup(PowerledKanal, Powerledfreq, PowerledBit);
	ledcAttachPin(PowerledPin, PowerledKanal);

	/******** Time Server Satr und setzen *****/

	Udp.begin(localPort);
	setSyncProvider(getNtpTime);
	setSyncInterval(300);
	digitalClockDisplay();

}

void loop() 
{
	
	//WIFI_login();
	Blynk.run();
	Timer.run();
	portal.handleClient();

	strip1.SetBrightness(aktHell);
	strip1.Show();


	/************* Uhr im Display aktualisieren ********/

	if (second(nowLocal()) == 00)
	{
		digitalClockDisplay();
	}
	
	
	/******** Schalter für Beleuchtung ********/

	switch (SonneIndex) {

	case 1:
		SonneAuf();
		break;
	case 2:
		SonneUn();
		break;
	case 3:
		SonneMitAn();
		break;
	case 4:
		SonneMitAus();
		break;
	case 5:
		SonneNaAus();
		break;
	}

	switch (FutterIndex)
	{

	case 1:
		Futterautomat();
		break;
	
	}

}