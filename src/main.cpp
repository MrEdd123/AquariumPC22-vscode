
//#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <TFT_eSPI.h>
#include <NeoPixelAnimator.h>
#include <TimeAlarms.h>
#include <HTTP_Method.h>
#include <ArduinoOTA.h>
#include "bitmaps.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <BlynkSimpleEsp32.h>
#include <Time.h>
#include <SPI.h>
#include <NeoPixelBrightnessBus.h>
#include <OneWire.h>
#include <DallasTemperature.h>




/***********  NeoPixel Einstellungen   ***********/

#define PIN_STRIPE1			13
#define NUMLEDS				166

NeoPixelBrightnessBus<NeoRgbwFeature, NeoEsp32Rmt0800KbpsMethod> strip1(NUMLEDS, PIN_STRIPE1);

/************ TFT Einstellungen ***************/

TFT_eSPI tft = TFT_eSPI();
#define MAX_GEN_COUNT 500

/************* One Wire (Tempfühler) **********/

#define ONE_WIRE_BUS			26				// Anschlusspin für OneWire			
OneWire oneWire(ONE_WIRE_BUS);					// One Wire Setup
DallasTemperature Tempfueh(&oneWire);			// Pass our oneWire reference to Dallas Temperature.


/**********************************************/


/********** Blynk und WiFi Einstellungen ******/

BlynkTimer Timer;

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "06a15068bcdb4ae89620f5fd2e67c672";
const char* host = "aquarium-webupdate";

/****** BETA Token *****************************/
//char auth[] = "b93c1e342a6c4217b466ec684e61679b";
//const char* host = "aquarium-webupdate-beta";

char ssid[] = "Andre+Janina-EXT";
char pass[] = "sommer12";

bool Connected2Blynk = false;

char serverblynk[] = "blynk-cloud.com";
unsigned int port = 8442;

/*************** WEB Server für OTA **************/

//const char* host = "aquarium-webupdate";

WebServer server(80);
const char* serverIndex = "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
"<input type='file' name='update'>"
"<input type='submit' value='Update'>"
"</form>"
"<div id='prg'>progress: 0%</div>"
"<script>"
"$('form').submit(function(e){"
"e.preventDefault();"
"var form = $('#upload_form')[0];"
"var data = new FormData(form);"
" $.ajax({"
"url: '/update',"
"type: 'POST',"
"data: data,"
"contentType: false,"
"processData:false,"
"xhr: function() {"
"var xhr = new window.XMLHttpRequest();"
"xhr.upload.addEventListener('progress', function(evt) {"
"if (evt.lengthComputable) {"
"var per = evt.loaded / evt.total;"
"$('#prg').html('progress: ' + Math.round(per*100) + '%');"
"}"
"}, false);"
"return xhr;"
"},"
"success:function(d, s) {"
"console.log('success!')"
"},"
"error: function (a, b, c) {"
"}"
"});"
"});"
"</script>";

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

uint8_t FutterStd = 19;
uint8_t FutterMin = 00;

uint8_t maxHell = 250;
uint8_t minHell;
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
uint8_t AblaufBlynk;

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
uint16_t Futtergesch = 0;
uint16_t Futterdauer = 1000;

uint8_t PowerledPin = 16;
uint16_t Powerledfreq = 2000;
uint8_t PowerledKanal = 1;
uint8_t PowerledBit = 8;
uint8_t Powerledwert = 0;
uint8_t Powerledmax = 250;
uint8_t Powerledmin = 0;

uint8_t LEDRot;
uint8_t LEDGruen;
uint8_t LEDBlau;
uint8_t LEDWeiss;


unsigned long currentMillis;
unsigned long previousMillis = 0;

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
int SonAu5[4] = { 186,67,1,0 };
int SonAu6[4] = { 240,120,30,100 };
int SonAu7[4] = { 230,100,200,200 };

// Sonnenuntergang Color Array
//				{ R, G, B, W }
int SonUn1[4] = { 250,200,100,40 };
int SonUn2[4] = { 240,255,30,0 };
int SonUn3[4] = { 186,68,2,0 };
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

uint16_t DurchWait;
uint16_t crossFadeWait;

uint8_t prevR = redVal;
uint8_t prevG = grnVal;
uint8_t prevB = bluVal;
uint8_t prevW = whiteVal;
uint16_t LEDStep = 0;
uint8_t Durchlauf = 1;
uint8_t SonneIndex = 0;

uint16_t StepWert = 1020;

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
const uint32_t syncIntervalAfterFail = 60; // if sync with NTP fails, retry earlier | 60
const uint32_t ntpWaitMax = 900; // maximum time in ms to wait for an answer of NTP Server, most used value 1500 I prefere below one second: 900
uint32_t ntpWaitActual = ntpWaitMax; // optimized/reduced wait time, start with maximum.

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

const int NTP_PACKET_SIZE = 48;		// NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime();
void digitalClockDisplay();
void sendNTPpacket(IPAddress& address);

/***********************************************/

/************** Funktionen *********************/
void PowerLEDplus()
{
	uint16_t PowerLEDFade;
	PowerLEDFade = DurchWait * 50;
	
		if (currentMillis - previousMillis > PowerLEDFade) {
			previousMillis = currentMillis;
			if (Powerledwert < Powerledmax) {
				Powerledwert++;
				ledcWrite(PowerledKanal, Powerledwert);
				}
			else
			{
				Durchlauf++;
			}

			/*Serial.print("PowerLED++ ");
			Serial.println(Powerledwert);	
			Serial.print(DurchWait);
			Serial.print("  ");
			Serial.println(PowerLEDFade);*/
		}
}

void PowerLEDminus()
{
	uint16_t PowerLEDFade;
	PowerLEDFade = DurchWait * 50;
	if (currentMillis - previousMillis > PowerLEDFade) {
		previousMillis = currentMillis;
		if (Powerledwert > 0) {
			Powerledwert--;
			ledcWrite(PowerledKanal, Powerledwert);
		}
		if(Powerledwert == 0) 
		{
			Durchlauf++;
		}
		/*Serial.print("PowerLED-- ");
		Serial.println(Powerledwert);	*/
	}
}


int calculateStep(int prevValue, int endValue) 
{
	int step = endValue - prevValue; // What's the overall gap?
	if (step) {                      // If its non-zero, 
		step = StepWert / step;              //   divide by 1020
	}
	return step;
}

/* The next function is calculateVal. When the loop value, i,
*  reaches the step size appropriate for one of the
*  colors, it increases or decreases the value of that color by 1.
*  (R, G, and B are each calculated separately.)
*/

int calculateVal(int step, int val, int i) 
{

	if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
		if (step > 0) {              //   increment the value if step is positive...
			val += 2;
		}
		else if (step < 0) {         //   ...or decrement it if step is negative
			val -= 2;
		}
	}
	// Defensive driving: make sure val stays in the range 0-255
	if (val > 255) {
		val = 255;
	}
	else if (val < 0) {
		val = 0;
	}
	return val;
}

/* crossFade() converts the percentage colors to a
*  0-255 range, then loops 1020 times, checking to see if
*  the value needs to be updated each time, then writing
*  the color values to the correct pins.
*/


void crossFade(int color[4]) 
{

	int stepR = calculateStep(prevR, (color[0] * 255) / 100);
	int stepG = calculateStep(prevG, (color[1] * 255) / 100);
	int stepB = calculateStep(prevB, (color[2] * 255) / 100);
	int stepW = calculateStep(prevW, (color[3] * 255) / 100);

	do {										// Delay schleife f�r�s Fading

		if (LEDStep <= StepWert) {

			LEDStep++;
			if (LEDStep >= (StepWert + 1)) {
				LEDStep = 0;
				Durchlauf++;
			}

			redVal = calculateVal(stepR, redVal, LEDStep);
			grnVal = calculateVal(stepG, grnVal, LEDStep);
			bluVal = calculateVal(stepB, bluVal, LEDStep);
			whiteVal = calculateVal(stepW, whiteVal, LEDStep);

			for (int i = 0; i < NUMLEDS; i++) {

				strip1.SetPixelColor(i, RgbwColor(grnVal, redVal, bluVal, whiteVal));
			}

			prevR = redVal;						// Update current values for next loop
			prevG = grnVal;
			prevB = bluVal;
			prevW = whiteVal;

			strip1.Show();

			crossFadeWait = DurchWait;
		}
	} while (crossFadeWait = 0);
	crossFadeWait --;


}

void SonneAuf()
{
	ledcWrite(BacklightKanalTFT, BacklightwertTag);
	lcd.print(0, 0, "Sonnenaufgang");
	AblaufI ++;
	
	if (AblaufI >= 100) {
		AblaufX ++;
		AblaufI = 0;
		tft.drawFastVLine(AblaufX, 116, 11, TFT_WHITE);
		
		}

		switch (Durchlauf)
		{
		case 1:
			Serial.println("Case1");
			crossFade(SonAu1);
			break;
		case 2:
			Serial.println("Case2");
			crossFade(SonAu2);
			break;
		case 3:
			Serial.println("Case3");
			crossFade(SonAu3);
			break;
		case 4:
			Serial.println("Case4");
			crossFade(SonAu4);
			break;
		case 5:
			Serial.println("Case5");
			crossFade(SonAu5);
			break;
		case 6:
			Serial.println("Case6");
			crossFade(SonAu6);
			break;
		case 7:
			Serial.println("Case7");
			crossFade(SonAu7);
			break;
		case 8:
			Serial.println("Case8 PowerLED");
			PowerLEDplus();
			break;

		case 9:
			Serial.println("Case9 Ende");
				Durchlauf = 1;
				SonneIndex = 0;
				AblaufX = 1;
				lcd.print(0, 0, "                ");
			
			break;
		}

	}




void SonneUn()
{
	ledcWrite(BacklightKanalTFT, BacklightwertNacht);
	lcd.print(0, 0, "Sonnenuntergang");
	AblaufI ++;
	
	if (AblaufI >= 102) {
		AblaufY --;
		AblaufI = 0;
		tft.drawFastVLine(AblaufY, 116, 11, TFT_BLACK);
	
	}

	switch (Durchlauf)
	{

	case 1:
		PowerLEDminus();
		break;
	case 2:
		crossFade(SonUn1);
		break;
	case 3:
		crossFade(SonUn2);
		break;
	case 4:
		crossFade(SonUn3);
		break;
	case 5:
		crossFade(SonUn4);
		break;
	case 6:
		crossFade(SonUn5);
		break;
	case 7:
		crossFade(SonUn6);
		break;
	case 8:
		crossFade(SonUn7);
		break;
	case 9:
		for (int i = 0; i < 10; i++) {
			
				strip1.SetPixelColor(i, RgbwColor(0, 0, 0, 0));
				delay(10);
				strip1.Show();
		}

		for (int i = 166; i > 17; i--) {

				strip1.SetPixelColor(i, RgbwColor(0, 0, 0, 0));
				delay(10);
				strip1.Show();
		}

		for (int i = 10; i < 17; i++) {

				strip1.SetPixelColor(i, RgbwColor(0, 0, 240, 0));
				delay(10);
				strip1.Show();
		}
		Durchlauf++;
		break;

	case 10:
		Durchlauf = 1;
		SonneIndex = 0; 
		AblaufY = 159;
		lcd.print(0, 0, "                ");
		break;
	}
}

void SonneMitAn()
{
	Serial.println("Sonne Mittag AN");
	lcd.print(0, 0, "Mittagssonne AN");
	strip1.SetBrightness(mittagHell);
	strip1.Show();
	aktHell = mittagHell;
	//PowerLEDminus();

	/*for (int i = Powerledmin; Powerledwert > i; Powerledwert--)
	{
		ledcWrite(PowerledKanal, Powerledwert);
	}*/
	Powerledwert = Powerledmin;
	ledcWrite(PowerledKanal, Powerledwert);
	lcd.print(0, 0, "                ");
	SonneIndex = 0;
}

void SonneMitAus()
{
	Serial.println("Sonne Mittag Aus");
	lcd.print(0, 0, "Mittagssonne AUS");
	strip1.SetBrightness(maxHell);
	strip1.Show();
	aktHell = maxHell;
	//PowerLEDplus();	
	/*for (int i = Powerledmax; Powerledwert < i; Powerledwert++) 
	{
		ledcWrite(PowerledKanal, Powerledwert);
	}*/
	Powerledwert = Powerledmax;
	ledcWrite(PowerledKanal, Powerledwert);
	lcd.print(0, 0, "                ");
	SonneIndex = 0;
}

void SonneNaAus()
{
	lcd.print(0, 0, "Nachtlicht AUS");
	switch (Durchlauf)
	{
	case 1:
		crossFade(Nachtlicht1);
		break;

	case 2:
		Durchlauf = 1;
		SonneIndex = 0;
		lcd.print(0, 0, "                ");
		break;
	}
}

void Heizung() 
{

	//portDISABLE_INTERRUPTS();
	Tempfueh.requestTemperatures();
	IstTemp = (Tempfueh.getTempCByIndex(0));
	//Serial.println(IstTemp);
	//portENABLE_INTERRUPTS();
	lcd.print(0, 1, IstTemp);
	//lcd.print(6, 1, "C");
	//lcd.print(7, 1,(char)178);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.drawFloat(SollTemp, 1, 27, 47, 2);				//SollWert in TFT einblenden
	if (IstTemp > -127)
	{

		Blynk.virtualWrite(V3, IstTemp);					//IstTemp an Blynk senden

		if (IstTemp < SollTemp - Hysterese)
		{
			digitalWrite(heizung, HIGH);
			tft.setTextColor(TFT_RED, TFT_BLACK);
			tft.drawFloat(IstTemp, 1, 90, 33, 4);
			ledheizung.on();
			//Blynk.virtualWrite(V3, IstTemp);			//IstTemp an Blynk senden
		}

		if (IstTemp > SollTemp + Hysterese)
		{
			digitalWrite(heizung, LOW);
			tft.setTextColor(TFT_GREEN, TFT_BLACK);
			tft.drawFloat(IstTemp, 1, 90, 33, 4);
			ledheizung.off();
			//Blynk.virtualWrite(V3, IstTemp);			//IstTemp an Blynk senden
		}

		/******************* Luefter Temperatur *******************/
		if (IstTemp >= LuefTemp)
		{
			digitalWrite(luefter, HIGH);
			ledluefter.on();
		}
		else
		{
			digitalWrite(luefter, LOW);
			ledluefter.off();
		}
	}
}

void Futterautomat()
{
	
	ledcWrite(FutterKanal, Futtergesch);

		delay(Futterdauer);

		ledcWrite(FutterKanal, 0);

	Serial.println("Futterautomat");
	

}

void TFT_Layout() 
{

	/****** Display initalisieren mit Layout*****/

	/******** Leiste Oben ***********/
	tft.init();
	tft.setRotation(TFTRotation);
	tft.fillScreen(TFT_BLACK);
	tft.drawLine(0, 20, 160, 20, TFT_GREEN);
	tft.setTextColor(TFT_WHITE);
	tft.drawBitmap(140, 0, wlan, 20, 20, TFT_RED);

	tft.setTextColor(TFT_RED);
	tft.drawString("0", 3, 2, 2);
	tft.drawString("0", 12, 2, 2);
	tft.drawString(":", 21, 2, 2);
	tft.drawString("0", 25, 2, 2);
	tft.drawString("0", 34, 2, 2);
	tft.drawString(":", 43, 2, 2);
	tft.drawString("0", 48, 2, 2);
	tft.drawString("0", 57, 2, 2);

	/******* Temperatur Fenster ***/
	tft.drawRect(20, 25, 60, 20, TFT_WHITE);
	tft.drawRect(80, 25, 80, 40, TFT_WHITE);
	tft.drawRect(20, 45, 60, 20, TFT_WHITE);

	tft.drawBitmap(0, 25, temp, 21, 40, TFT_WHITE);
	tft.setTextColor(TFT_WHITE);
	tft.drawString("Soll", 37, 31, 1);
	tft.setCursor(63, 50);
	tft.print(char(247));
	tft.print("C");
	tft.setCursor(143, 30);
	tft.print(char(247));
	tft.print("C");

	/******** CO2 Fenster ***********/
	tft.drawBitmap(0, 67, co2s, 21, 22, TFT_WHITE);
	tft.drawRect(20, 67, 60, 22, TFT_WHITE);


	/******** Sonne Fenster *********/
	tft.drawBitmap(0, 90, sonne, 21, 22, TFT_WHITE);
	tft.drawRect(20, 90, 60, 22, TFT_WHITE);

	/******** Futterautomat Fenster *********/
	tft.drawBitmap(80, 67, futter, 21, 22, TFT_WHITE);
	tft.drawRect(100, 67, 60, 22, TFT_WHITE);

	/******** Ablauf Fenster ********/

	tft.drawRect(0, 115, 160, 13, TFT_WHITE);

}

/********************************************/

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
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


// calculates the daylight saving time for middle Europe. Input: Unixtime in UTC (!)
boolean isDayLightSaving(uint32_t local_t)
{
	if (month(local_t) < 3 || month(local_t) > 10) return false; // no DSL in Jan, Feb, Nov, Dez
	if (month(local_t) > 3 && month(local_t) < 10) return true; // DSL in Apr, May, Jun, Jul, Aug, Sep
																// if (month == 3 && (hour + 24 * day) >= (1 + tzHours + 24 * (31 - (5 * year / 4 + 4) % 7)) || month == 10 && (hour + 24 * day) < (1 + tzHours + 24 * (31 - (5 * year / 4 + 1) % 7)));
	if (month(local_t) == 3 && (hour(local_t) + 24 * day(local_t)) >= (1 + 24 * (31 - (5 * year(local_t) / 4 + 4) % 7)) || month(local_t) == 10 && (hour(local_t) + 24 * day(local_t)) < (1 + 24 * (31 - (5 * year(local_t) / 4 + 1) % 7)))
		return true;
	else
		return false;
}

// returns the current date/time as UNIX timestamp, incl. timezone, including daylightsaving
uint32_t nowLocal() 
{
	uint32_t local_t = now();
	if (isDayLightSaving(local_t))
		local_t += 3600 + timeZone * SECS_PER_HOUR;
	else
		local_t += timeZone * SECS_PER_HOUR;
	return local_t;
}


void digitalClockDisplay() 
{

	char readable[20]; // declare a char buffer large enough for your output

	uint32_t local_t = nowLocal(); //f�r Zeitumstellung n�tig

	tft.setTextColor(TFT_GREEN, TFT_BLACK);
	if (hour(local_t) < 10) {
		tft.drawString("0", 3, 2, 2);
		tft.drawNumber(hour(local_t), 12, 2, 2);
	}
	else
		tft.drawNumber(hour(local_t), 3, 2, 2);

	tft.drawString(":", 21, 2, 2);

	if (minute(local_t) < 10) {
		tft.drawString("0", 25, 2, 2);
		tft.drawNumber(minute(local_t), 34, 2, 2);
	}
	else
		tft.drawNumber(minute(local_t), 25, 2, 2);

	tft.drawString(":", 43, 2, 2);

	if (second(local_t) < 10) {
		tft.drawString("0", 48, 2, 2);
		tft.drawNumber(second(), 57, 2, 2);
	}
	else
		tft.drawNumber(second(local_t), 48, 2, 2);

}

void SunTimer() 
{
	tft.setTextSize(1);
	tft.setCursor(24, 92);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.print("Ein");
	tft.setCursor(48, 92);
	if (SoAuStd < 10)
		tft.print("0");
	tft.print(SoAuStd);
	tft.print(":");
	if (SoAuMin < 10)
		tft.print("0");
	tft.print(SoAuMin);

	tft.setCursor(24, 102);
	tft.print("Aus");
	tft.setCursor(48, 102);
	if (SoUnStd < 10)
		tft.print("0");
	tft.print(SoUnStd);
	tft.print(":");
	if (SoUnMin < 10)
		tft.print("0");
	tft.print(SoUnMin);
}

void CO2Timer() 
{
	tft.setTextSize(1);
	tft.setCursor(24, 70);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	tft.print("Ein");
	tft.setCursor(48, 70);
	if (CO2AnStd < 10)
		tft.print("0");
	tft.print(CO2AnStd);
	tft.print(":");
	if (CO2AnMin < 10)
		tft.print("0");
	tft.print(CO2AnMin);

	tft.setCursor(24, 80);
	tft.print("Aus");
	tft.setCursor(48, 80);
	if (CO2AusStd < 10)
		tft.print("0");
	tft.print(CO2AusStd);
	tft.print(":");
	if (CO2AusMin < 10)
		tft.print("0");
	tft.print(CO2AusMin);
}

void FutterTimer() 
{

	tft.setTextSize(1);
	//tft.setCursor(104, 74);
	tft.setTextColor(TFT_WHITE, TFT_BLACK);
	//tft.print("Ein");
	tft.setCursor(120, 74);
	if (FutterStd < 10)
		tft.print("0");
	tft.print(FutterStd);
	tft.print(":");
	if (FutterMin < 10)
		tft.print("0");
	tft.print(FutterMin);
}


void WIFI_TFT() 
{
	int wifi_ctr = 0;
	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		tft.drawBitmap(140, 0, wlan, 20, 20, TFT_RED);
	}
	tft.drawBitmap(140, 0, wlan, 20, 20, TFT_GREEN);

	while (WiFi.status() == WL_CONNECTION_LOST) {

		tft.drawBitmap(140, 0, wlan, 20, 20, TFT_RED);
		WiFi.reconnect();
	}
}


void ProgrammTimer() 
{
	/*************** Timer Programme ******************/

	uint32_t local_t = nowLocal();							//für Zeitumstellung nötig
	Zeit = (minute(local_t)) + (hour(local_t) * 100);		//RTC zeit in vier stellig Umrechnen (z.B. 12:30 nach 1230)

	uint16_t SoTimerAu;
	SoTimerAu = ((SoAuMin)+(SoAuStd) * 100);

	if ((Zeit == SoTimerAu) & (second(local_t) == 0))
	{
		SonneIndex = 1;
	}

	uint16_t SoTimerUn;
	SoTimerUn = ((SoUnMin)+(SoUnStd) * 100);

	if ((Zeit == SoTimerUn) & (second(local_t) == 0))
	{
		SonneIndex = 2;
	}

	uint16_t SoTimerMiAn;
	SoTimerMiAn = ((SoMiAnMin)+(SoMiAnStd) * 100);
	if ((Zeit == SoTimerMiAn) & (second(local_t) == 0))
	{
		SonneIndex = 3;
	}

	uint16_t SoTimerMiAus;
	SoTimerMiAus = ((SoMiAusMin)+(SoMiAusStd) * 100);
	if ((Zeit == SoTimerMiAus) & (second(local_t) == 0))
	{
		SonneIndex = 4;
	}

	uint16_t SoTimerNaAn;
	SoTimerNaAn = ((SoNaMin)+(SoNaStd) * 100);
	if ((Zeit == SoTimerNaAn) & (second(local_t) == 0))
	{
		SonneIndex = 5;
	}

	uint16_t FutterTimer;
	FutterTimer = ((FutterMin)+(FutterStd) * 100);
	if ((Zeit == FutterTimer) & (second(local_t) == 0))
	{
		Futterautomat();
	}

	uint16_t CO2AN;
	uint16_t CO2AUS;
	CO2AN = ((CO2AnMin)+(CO2AnStd) * 100);
	CO2AUS = ((CO2AusMin)+(CO2AusStd) * 100);
	if ((Zeit > (CO2AN - 1)) && (Zeit < CO2AUS))
	{
		digitalWrite(co2, HIGH);
		ledco2.on();
	}
	else
	{
		digitalWrite(co2, LOW);
		ledco2.off();
	}
}


/************************** NTP code ********************************************/

time_t getNtpTime()
{
	IPAddress ntpServerIP; // NTP server's ip address

	while (Udp.parsePacket() > 0); // discard any previously received packets
	Serial.println("Transmit NTP Request");
	// get a random server from the pool
	WiFi.hostByName(ntpServerName, ntpServerIP);
	Serial.print(ntpServerName);
	Serial.print(": ");
	Serial.println(ntpServerIP);
	sendNTPpacket(ntpServerIP);
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500) {
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE) {
			Serial.println("Receive NTP Response");
			tft.setTextColor(TFT_BLACK);
			tft.drawString("Zeit Server :-(", 1, 5);
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
	Serial.println("No NTP Response :-(");
	tft.setTextColor(TFT_RED);
	tft.drawString("0", 3, 2, 2);
	tft.drawString("0", 12, 2, 2);
	tft.drawString(":", 21, 2, 2);
	tft.drawString("0", 25, 2, 2);
	tft.drawString("0", 34, 2, 2);
	tft.drawString(":", 43, 2, 2);
	tft.drawString("0", 48, 2, 2);
	tft.drawString("0", 57, 2, 2);

	if (ntpWaitActual + 200 < ntpWaitMax) ntpWaitActual += 200; // expand wait time if necessary - I'm pretty aware of that this value will hardly reach ntpWaitMax with this simple condition.
	setSyncInterval(syncIntervalAfterFail); // in both cases: no NTP response or no DNS lookup

	return 0; // return 0 if unable to get the time
}



void WIFI_login() 
{

	tft.drawBitmap(140, 0, wlan, 20, 20, TFT_GREEN);
	Serial.println("WiFi Login");
	while (WiFi.status() != WL_CONNECTED && wifi_retry <= 5) {
		wifi_retry++;
		WiFi.persistent(false);   // daten nicht in Flash speichern
		WiFi.mode(WIFI_STA);
		Serial.printf("Connecting to %s ", ssid);
		tft.drawBitmap(140, 0, wlan, 20, 20, TFT_RED);
		WiFi.begin(ssid, pass);
		Blynk.config(auth);
		Blynk.connect();
		while (WiFi.status() != WL_CONNECTED)
		{
			delay(500);
			Serial.print(".");
		}
		
		Serial.println(" connected");
		Serial.print("local IP:");
		Serial.println(WiFi.localIP());
	}

	if (wifi_retry >= 5) {
		wifi_retry == 0;
		Serial.println("\nReboot");
		ESP.restart();
	}

}


void reconnectBlynk() 
{
	WIFI_TFT();

	if (!Blynk.connected()) {
		if (Blynk.connect()) {
			tft.drawBitmap(140, 0, wlan, 20, 20, TFT_GREEN);
		}
		else {
			tft.drawBitmap(140, 0, wlan, 20, 20, TFT_RED);
			delay(2);
			Blynk.begin(auth, ssid, pass);
		}
	}
}
/********************************************************************************/




/******* LED Timer Sonne Auf/Untergang *******/

BLYNK_WRITE(V0) {
	TimeInputParam t(param);

	// Process start time

	if (t.hasStartTime())
	{
		SoAuStd = t.getStartHour();
		SoAuMin = t.getStartMinute();
	}

	if (t.hasStopTime())
	{
		SoUnStd = t.getStopHour();
		SoUnMin = t.getStopMinute();
	}

	SunTimer();
}

/*********** LED Timer Mittagssonne ************/

BLYNK_WRITE(V6) {
	TimeInputParam t(param);

	if (t.hasStartTime())
	{
		SoMiAnStd = t.getStartHour();
		SoMiAnMin = t.getStartMinute();

	}

	if (t.hasStopTime())
	{
		SoMiAusStd = t.getStopHour();
		SoMiAusMin = t.getStopMinute();

	}
}

/************ LED Timer Nachtlicht *****************/

BLYNK_WRITE(V11) {

	TimeInputParam t(param);

	if (t.hasStartTime())
	{
		SoNaStd = t.getStartHour();
		SoNaMin = t.getStartMinute();
	}
}

/**************** CO2 Timer ******************/

BLYNK_WRITE(V4) {
	TimeInputParam t(param);

	// Process start time

	if (t.hasStartTime())
	{
		CO2AnStd = t.getStartHour();
		CO2AnMin = t.getStartMinute();
	}

	if (t.hasStopTime())
	{
		CO2AusStd = t.getStopHour();
		CO2AusMin = t.getStopMinute();
	}

	CO2Timer();
}

/*************** Soll Temperatur ***********/

BLYNK_WRITE(V2) {

	Blynk.virtualWrite(V1, param.asFloat());
	SollTemp = param.asFloat();
}

/******** Luefter **************************/

BLYNK_WRITE(V20) {

	Blynk.virtualWrite(V20, param.asFloat());
	LuefTemp = param.asFloat();
}

/************* Durchlauf Zeit *************/

BLYNK_WRITE(V5) {

	Blynk.virtualWrite(V5, param.asFloat());
	DurchWait = param.asFloat();
}

/******* Futterautomat ********************/

BLYNK_WRITE(V7) {

	TimeInputParam t(param);

	if (t.hasStartTime())
	{
		FutterStd = t.getStartHour();
		FutterMin = t.getStartMinute();
	}

	FutterTimer();
}

BLYNK_WRITE(V34) {

	Blynk.virtualWrite(V34, param.asFloat());
	Futterdauer = param.asFloat();
}

BLYNK_WRITE(V35) {

	Blynk.virtualWrite(V35, param.asFloat());
	Futtergesch = param.asFloat();
}

/********** Temp Hysterese *****************/

BLYNK_WRITE(V8) {

	Blynk.virtualWrite(V8, param.asFloat());
	Hysterese = param.asFloat();
}

/****** Maximale Helligkeit *****************/

BLYNK_WRITE(V9) {

	Blynk.virtualWrite(V9, param.asFloat());
	maxHell = param.asFloat();
}

/******* Mittags Helligkeit *****************/

BLYNK_WRITE(V25) {

	Blynk.virtualWrite(V25, param.asFloat());
	mittagHell = param.asFloat();
}

/******* PowerLED Max Hellighkeit *********/

BLYNK_WRITE(V36) 
{

	Blynk.virtualWrite(V36, param.asFloat());
	Powerledmax = param.asFloat();
}

/******* Aktuelle Helligkeit *****************/

BLYNK_WRITE(V28) 
{

	Blynk.virtualWrite(V28, param.asFloat());
	aktHell = param.asFloat();
}

/******** TFT Helligkeit ********************/

BLYNK_WRITE(V10) 
{

	Blynk.virtualWrite(V10, param.asFloat());
	BacklightwertTag = param.asFloat();
	ledcWrite(BacklightKanalTFT, BacklightwertTag);
}

BLYNK_WRITE(V33) 
{

	Blynk.virtualWrite(V33, param.asFloat());
	BacklightwertNacht = param.asFloat();
	//ledcWrite(BacklightKanalTFT, BacklightwertNacht);
}

/****** TFT Rotation ************************/

BLYNK_WRITE(V19) 
{

	int i = param.asInt();
	if (i == 1) {
		TFTRotation = 3;
		delay(250);
	}
	else
		TFTRotation = 1;

	TFT_Layout();
	
	WIFI_TFT();
	
	CO2Timer();
	
	SunTimer();
	
	FutterTimer();
	
}

/************ Manuelle Funktionen ************/

BLYNK_WRITE(V12) 
{ /*Sonne An*/

	int i = param.asInt();
	if (i == 1) {
		SonneIndex = 1;
		delay(250);
	}
}

BLYNK_WRITE(V14) 
{ /*Sonne Aus*/

	int i = param.asInt();
	if (i == 1) {
		SonneIndex = 2;
		delay(250);
	}
}

BLYNK_WRITE(V13) 
{ /*Sonne Mittag AN*/

	int i = param.asInt();
	if (i == 1) {
		SonneMitAn();
		delay(250);
		
	}
}

BLYNK_WRITE(V21) 
{ /*Sonne Mittag Aus*/

	int i = param.asInt();
	if (i == 1) {
		SonneMitAus();
		delay(250);
		
	}
}

BLYNK_WRITE(V15) 
{ /*Nachlicht Aus*/

	int i = param.asInt();
	if (i == 1) {
		SonneIndex = 5;
		delay(250);
		SonneIndex = 0;
	}
}

BLYNK_WRITE(V16) 
{ /*Futterautomat*/


	int i = param.asInt();
	if (i == 1) {

		Futterautomat();
	}
	
	/*ledcWrite(FutterKanal, Futtergesch);

	delay(Futterdauer);

	ledcWrite(FutterKanal, 0);*/

}

BLYNK_WRITE(V17) 
{ /*Luefter*/

	int i = param.asInt();
	if (i == 1) {
		digitalWrite(luefter, HIGH);
		ledluefter.on();
		delay(250);
	}
}


/*********** System Neustart ************/

BLYNK_WRITE(V18) 
{

	int i = param.asInt();
	if (i == 1) {
		ESP.restart();
		delay(250);
	}
}

/************* LED Farbe Manuell *******/

BLYNK_WRITE(V29) 
{
	Blynk.virtualWrite(V29, param.asFloat());
	LEDRot = param.asFloat();

		/*for (int i = 0; i < NUMLEDS; i++) {

			strip1.SetPixelColor(i, RgbwColor(LEDGruen, LEDRot, LEDBlau, LEDWeiss));
		}*/
}

BLYNK_WRITE(V30) 
{
	Blynk.virtualWrite(V30, param.asFloat());
	LEDBlau = param.asFloat();
	/*
		for (int i = 0; i < NUMLEDS; i++) {

			strip1.SetPixelColor(i, RgbwColor(LEDGruen, LEDRot, LEDBlau, LEDWeiss));
		}*/
}

BLYNK_WRITE(V31) 
{
	Blynk.virtualWrite(V31, param.asFloat());
	LEDGruen = param.asFloat();
	
		/*for (int i = 0; i < NUMLEDS; i++) {

			strip1.SetPixelColor(i, RgbwColor(LEDGruen, LEDRot, LEDBlau, LEDWeiss));
		}*/
	

}

BLYNK_WRITE(V32) 
{
	Blynk.virtualWrite(V32, param.asFloat());
	LEDWeiss = param.asFloat();

		/*for (int i = 0; i < NUMLEDS; i++) {

			strip1.SetPixelColor(i, RgbwColor(LEDGruen, LEDRot, LEDBlau, LEDWeiss));
		}*/
}

BLYNK_WRITE(V37 ) 
{
	//uint8_t Powerledmanu;
	Blynk.virtualWrite(V37, param.asFloat());
	Powerledwert = param.asFloat();
	
}

BLYNK_WRITE(V38) 
{

	int i = param.asInt();
	if (i == 1) {
		for (int i = 0; i < NUMLEDS; i++) {
			strip1.SetPixelColor(i, RgbwColor(LEDGruen, LEDRot, LEDBlau, LEDWeiss));
			ledcWrite(PowerledKanal, Powerledwert);
		}	
	}
}

/*************************************************/



/*************************************************/
void setup() 
{


	Serial.begin(9600);

	/********* TFT Layout setzen ***************/

	TFT_Layout();

	

	pinMode(heizung, OUTPUT);
	pinMode(luefter, OUTPUT);
	pinMode(co2, OUTPUT);


	/**** Alarm Timer setzen für Funktionen ******/ 

	//Alarm.timerRepeat(1, digitalClockDisplay);
	//Alarm.timerRepeat(1, ProgrammTimer);
	//Alarm.timerRepeat(5, Heizung);

	Timer.setInterval(1000, digitalClockDisplay);
	Timer.setInterval(1000, ProgrammTimer);
	Timer.setInterval(5000, Heizung);
	Timer.setInterval(20000, WIFI_login);
	//Alarm.timerRepeat(120, reconnectBlynk);

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
	Tempfueh.setResolution(12);			// 0.25 Grad Genauigkeit

	/******** TFT Backlight *******************/

	ledcSetup(BacklightKanalTFT, BacklightFrequenz, BacklightBit);	//ledcSetup(Kanal, Frequenz, Bit);
	ledcAttachPin(BacklightPin, BacklightKanalTFT);					//ledcAttachPin(Pin, Kanal);

	/******** Futterautomat *******************/

	ledcSetup(FutterKanal, Futterfreq, FutterRes);
	ledcAttachPin(FutterPin, FutterKanal);

	/******* PowerLED ************************/

	ledcSetup(PowerledKanal, Powerledfreq, PowerledBit);
	ledcAttachPin(PowerledPin, PowerledKanal);

	
	///******** Blynk Verbinden / WIFI Verbinden **********/
	WIFI_login();

	//Blynk.begin(auth, ssid, pass);

	Blynk.syncAll();								  // Werte aus Blynk Cloud laden


	/*************** WEB Server für OTA *******/

	if (WiFi.waitForConnectResult() == WL_CONNECTED) {
		MDNS.begin(host);
		server.on("/", HTTP_GET, []() {
			server.sendHeader("Connection", "close");
			server.send(200, "text/html", serverIndex);
			});
		server.on("/update", HTTP_POST, []() {
			server.sendHeader("Connection", "close");
			server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "Update OK Neustart!!");
			ESP.restart();
			}, []() {
				HTTPUpload& upload = server.upload();
				if (upload.status == UPLOAD_FILE_START) {
					Serial.setDebugOutput(true);
					Serial.printf("Update: %s\n", upload.filename.c_str());
					if (!Update.begin()) { //start with max available size
						Update.printError(Serial);
					}
				}
				else if (upload.status == UPLOAD_FILE_WRITE) {
					if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
						Update.printError(Serial);
					}
				}
				else if (upload.status == UPLOAD_FILE_END) {
					if (Update.end(true)) { //true to set the size to the current progress
						Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
					}
					else {
						Update.printError(Serial);
					}
					Serial.setDebugOutput(false);
				}
			});
		server.begin();
		MDNS.addService("http", "tcp", 80);

		Serial.printf("Ready! Open http://%s.local in your browser\n", host);
	}
	else {
		Serial.println("WiFi Failed");
	}


	/******** Time Server Satr und setzen *****/

	Udp.begin(localPort);
	setSyncProvider(getNtpTime);
	setSyncInterval(300);
	digitalClockDisplay();

}

time_t prevDisplay = 0;						// when the digital clock was displayed

void loop() 
{

	Blynk.run();
	Timer.run();
	server.handleClient();

	strip1.SetBrightness(aktHell);
	strip1.Show();

	currentMillis = millis();

	
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

}




