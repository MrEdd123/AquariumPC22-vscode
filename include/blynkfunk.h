// blynkfunk.h

#ifndef _BLYNKFUNK_h
#define _BLYNKFUNK_h

/******* LED Timer Sonne Auf/Untergang *******/

BLYNK_WRITE(V0) 
{
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
	Futterdauer = Futterdauer * 1000;
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

		FutterIndex = 1;
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

}

BLYNK_WRITE(V37 ) 
{
	
	Blynk.virtualWrite(V37, param.asFloat());
	Powerledwert_virtuell = param.asFloat();
	
}

BLYNK_WRITE(V38) 
{

	int i = param.asInt();
	if (i == 1) {
		for (int i = 0; i < NUMLEDS; i++) {
			strip1.SetPixelColor(i, RgbwColor(LEDGruen, LEDRot, LEDBlau, LEDWeiss));
			Powerledwert = Powerledwert_virtuell;
			ledcWrite(PowerledKanal, Powerledwert);
		}	
	}
}



#endif