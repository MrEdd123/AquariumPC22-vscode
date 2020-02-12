// Funktionen.h

#ifndef _FUNKTIONEN_h
#define _FUNKTIONEN_h


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



#endif