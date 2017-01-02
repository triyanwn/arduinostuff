// ===============================
// ATR v5
// © 2016 Triyan W. Nugroho
// ===============================

#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <virtuabotixRTC.h>

// relay pins
#define relay1      0
#define relay2      1
#define relay3      2
#define relay4      3
#define relay5      10
#define relay6      11
#define relay7      12
#define relay8      13

// define pins of RTC module
#define rtcVCC      A5
#define rtcGND      A4
#define rtcSCLK     A3
#define rtcIO       A2
#define rtcCE       A1

// creation of RTC object
virtuabotixRTC myRTC(rtcSCLK, rtcIO, rtcCE);

// keypad definitions
#define btnRIGHT    0
#define btnUP       1
#define btnDOWN     2
#define btnLEFT     3
#define btnSELECT   4
#define btnNONE     5

// mode definitions
#define modeSETUP   1
#define modeNORMAL  2

int mode = 1; // set default mode

// EEPROM adrressing
#define adr1ON    2
#define adr1OF    4
#define adr2ON    6
#define adr2OF    8
#define adr3ON    10
#define adr3OF    12
#define adr4ON    14
#define adr4OF    16
#define adr5ON    18
#define adr5OF    20
#define adr6ON    22
#define adr6OF    24
#define adr7ON    26
#define adr7OF    28
#define adr8ON    30
#define adr8OF    32

int eepromMin = 0;
int eepromHour = 0;

int eepromHourON = 0;
int eepromHourOF = 0;
int eepromMinON = 0;
int eepromMinOF = 0;

// pins used on LCD Keypad Shield
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
int lcd_key     = 0;
int adc_key_in  = 0;

void setup() {
	// inisialisasi serial utk debugging
	//	Serial.begin(9600);

	// jadikan semua digital pin sbg output
	for (int i = 0; i <= 13; i++) {
		pinMode(i, OUTPUT);
	}

	// activate RTC module
	pinMode(rtcVCC,  OUTPUT);
	pinMode(rtcGND,  OUTPUT);
	pinMode(rtcSCLK, OUTPUT);
	pinMode(rtcIO,   OUTPUT);
	pinMode(rtcCE,   OUTPUT);
	digitalWrite(rtcVCC, HIGH);
	digitalWrite(rtcGND, LOW);

	delay(500);

	// lcd initialization
	lcd.begin(16, 2);
}

void loop() {

	lcd.setCursor(4, 0);

	// tampilkan jam:menit:detik
	displayTime();

	// set & tampilkan relay berdasarkan data eeprom
	lcd.setCursor(0, 1); 
	lcd.print("1");
	relayAction(adr1ON, adr1OF, 1, relay1);
	lcd.setCursor(2, 1); 
	lcd.print("2");
	relayAction(adr2ON, adr2OF, 3, relay2);
	lcd.setCursor(4, 1); 
	lcd.print("3");
	relayAction(adr3ON, adr3OF, 5, relay3);
	lcd.setCursor(6, 1); 
	lcd.print("4");
	relayAction(adr4ON, adr4OF, 7, relay4);
	lcd.setCursor(8, 1); 
	lcd.print("5");
	relayAction(adr5ON, adr5OF, 9, relay5);
	lcd.setCursor(10, 1); 
	lcd.print("6");
	relayAction(adr6ON, adr6OF, 11, relay6);
	lcd.setCursor(12, 1); 
	lcd.print("7");
	relayAction(adr7ON, adr7OF, 13, relay7);
	lcd.setCursor(14, 1); 
	lcd.print("8");
	relayAction(adr8ON, adr8OF, 15, relay8);

	// jika user menekan tombol SELECT, masuk menu setting

	if (read_LCD_buttons() == btnSELECT) {
		while (read_LCD_buttons() == btnSELECT);
		lcd.clear();
		lcd.setCursor(0, 0);
		lcd.print("UP. TIME SETTING");
		lcd.setCursor(0, 1);
		lcd.print("DN. PIN SETTING");
		while (read_LCD_buttons() == btnNONE);
		if (read_LCD_buttons() == btnUP) {
			while (read_LCD_buttons() == btnUP);
			setRTC();   // jika user menekan tombol UP, masuk menu setting RTC
		}
		else if (read_LCD_buttons() == btnDOWN) {
			while (read_LCD_buttons() == btnDOWN);
			setPin();   // jika user menekan tombol DOWN, masuk menu setting PIN
		}
	}
}

void relayAction(int adrON, int adrOF, int pos, int pin) {
	myRTC.updateTime();
	int MinToday = (myRTC.hours * 60) + myRTC.minutes;
	int MinEprON = (EEPROM.read(adrON) * 60) + EEPROM.read(adrON + 1);
	int MinEprOF = (EEPROM.read(adrOF) * 60) + EEPROM.read(adrOF + 1);

	lcd.setCursor(pos, 1);
	if (MinEprON == MinEprOF) {  // jika sama berarti tidak dipake
		lcd.print(" ");
		digitalWrite(pin, LOW);
	}
	else if (MinEprON < MinEprOF) { // kondisi ON terjadi di hari yg sama
		if ((MinEprON <= MinToday) && (MinEprOF > MinToday)) {
			lcd.print("*");
			digitalWrite(pin, LOW);
		}
		else {
			lcd.print("-");
			digitalWrite(pin, HIGH);
		}
	}
	else if (MinEprON > MinEprOF) {  // kondisi ON terjadi sampai besoknya
		if ((MinEprON >= MinToday) || (MinEprOF < MinToday)) {
			lcd.print("*");
			digitalWrite(pin, LOW);
		}
		else {
			lcd.print("-");
			digitalWrite(pin, HIGH);
		}
	}
}

/* ================================================== */
/* SETUP Functions                                    */
/* ================================================== */

void setRTC() {

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("TIME SETTING");
	delay(1000);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("CURRENT ");
	setupShowValue(myRTC.hours, myRTC.minutes, 0);

	lcd.setCursor(0, 1);
	lcd.print("NEW ");

	myRTC.updateTime();
	setupShowValue(myRTC.hours, myRTC.minutes, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValueSetRTC(myRTC.hours, myRTC.minutes, 1);
	lcd.setCursor(0,0);
	lcd.print("SETTINGS SAVED   ");
	delay(1000);
	lcd.clear();
}

void setPin() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PIN SETTING");
	delay(1000);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PIN 1 ON");
	eepromHour = EEPROM.read(adr1ON);
	eepromMin = EEPROM.read(adr1ON + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 0);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr1ON, 0);

	lcd.setCursor(0, 1);
	lcd.print("PIN 1 OFF");
	eepromHour = EEPROM.read(adr1OF);
	eepromMin = EEPROM.read(adr1OF + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr1OF, 1);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PIN 2 ON");
	eepromHour = EEPROM.read(adr2ON);
	eepromMin = EEPROM.read(adr2ON + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 0);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr2ON, 0);

	lcd.setCursor(0, 1);
	lcd.print("PIN 2 OFF");
	eepromHour = EEPROM.read(adr2OF);
	eepromMin = EEPROM.read(adr2OF + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr2OF, 1);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PIN 3 ON");
	eepromHour = EEPROM.read(adr3ON);
	eepromMin = EEPROM.read(adr3ON + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 0);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr3ON, 0);

	lcd.setCursor(0, 1);
	lcd.print("PIN 3 OFF");
	eepromHour = EEPROM.read(adr3OF);
	eepromMin = EEPROM.read(adr3OF + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr3OF, 1);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PIN 4 ON");
	eepromHour = EEPROM.read(adr4ON);
	eepromMin = EEPROM.read(adr4ON + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 0);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr4ON, 0);

	lcd.setCursor(0, 1);
	lcd.print("PIN 4 OFF");
	eepromHour = EEPROM.read(adr4OF);
	eepromMin = EEPROM.read(adr4OF + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr4OF, 1);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PIN 5 ON");
	eepromHour = EEPROM.read(adr5ON);
	eepromMin = EEPROM.read(adr5ON + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 0);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr5ON, 0);

	lcd.setCursor(0, 1);
	lcd.print("PIN 5 OFF");
	eepromHour = EEPROM.read(adr5OF);
	eepromMin = EEPROM.read(adr5OF + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr5OF, 1);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PIN 6 ON");
	eepromHour = EEPROM.read(adr6ON);
	eepromMin = EEPROM.read(adr6ON + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 0);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr6ON, 0);

	lcd.setCursor(0, 1);
	lcd.print("PIN 6 OFF");
	eepromHour = EEPROM.read(adr6OF);
	eepromMin = EEPROM.read(adr6OF + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr6OF, 1);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PIN 7 ON");
	eepromHour = EEPROM.read(adr7ON);
	eepromMin = EEPROM.read(adr7ON + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 0);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr7ON, 0);

	lcd.setCursor(0, 1);
	lcd.print("PIN 7 OFF");
	eepromHour = EEPROM.read(adr7OF);
	eepromMin = EEPROM.read(adr7OF + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr7OF, 1);

	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("PIN 8 ON");
	eepromHour = EEPROM.read(adr8ON);
	eepromMin = EEPROM.read(adr8ON + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 0);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr8ON, 0);

	lcd.setCursor(0, 1);
	lcd.print("PIN 8 OFF");
	eepromHour = EEPROM.read(adr8OF);
	eepromMin = EEPROM.read(adr8OF + 1);
	if (eepromHour >= 24) eepromHour = 0;
	if (eepromMin  >= 60) eepromMin  = 0;
	setupShowValue(eepromHour, eepromMin, 1);
	while (read_LCD_buttons() == btnNONE);
	setupChooseValue(eepromHour, eepromMin, adr8OF, 1);
	lcd.clear();
}

void setupChooseValue(int HourNew, int MinNew, byte Address, byte Pos) {
	while (read_LCD_buttons() != btnSELECT) {
		if (read_LCD_buttons() == btnRIGHT) {
			if (HourNew < 23) {
				HourNew++;
			}
		}
		else if (read_LCD_buttons() == btnLEFT) {
			if (HourNew > 0) {
				HourNew--;
			}
		}
		else if (read_LCD_buttons() == btnUP) {
			if (MinNew < 59) {
				MinNew++;
			}
		}
		else if (read_LCD_buttons() == btnDOWN) {
			if (MinNew > 0) {
				MinNew--;
			}
		}
		setupShowValue(HourNew, MinNew, Pos);
		delay(150);
	}
	while (read_LCD_buttons() != btnNONE);  // tunggu sampai tombol rilis
	EEPROM.write(Address, HourNew);
	EEPROM.write(Address + 1, MinNew);
	delay(150);
}

void setupChooseValueSetRTC(int HourNew, int MinNew, byte Pos) {
	while (read_LCD_buttons() != btnSELECT) {
		if (read_LCD_buttons() == btnRIGHT) {
			if (HourNew < 23) {
				HourNew++;
			}
		}
		else if (read_LCD_buttons() == btnLEFT) {
			if (HourNew > 0) {
				HourNew--;
			}
		}
		else if (read_LCD_buttons() == btnUP) {
			if (MinNew < 59) {
				MinNew++;
			}
		}
		else if (read_LCD_buttons() == btnDOWN) {
			if (MinNew > 0) {
				MinNew--;
			}
		}
		setupShowValue(HourNew, MinNew, Pos);
		delay(150);
	}
	while (read_LCD_buttons() != btnNONE);  // tunggu sampai tombol rilis
	myRTC.setDS1302Time(00, MinNew, HourNew, 6, 10, 1, 2014);
	delay(150);
}

void setupShowValue(int Hour, int Min, int Pos) {
	lcd.setCursor(11, Pos);
	print2digits(Hour);
	lcd.print(":");
	print2digits(Min);
}

/* ================================================== */
/* LCD Functions                                      */
/* ================================================== */

int read_LCD_buttons()
{
	adc_key_in = analogRead(0);       // read the value from the sensor

	if (adc_key_in > 1000) return btnNONE; 
	if (adc_key_in < 50)   return btnRIGHT;  
	if (adc_key_in < 150)  return btnUP; 
	if (adc_key_in < 300)  return btnDOWN; 
	if (adc_key_in < 500)  return btnLEFT; 
	if (adc_key_in < 850)  return btnSELECT;  
	return btnNONE;                // when all others fail, return this.
}

void eeprom_write_int(int p_address, int p_value) {
	byte lowByte = ((p_value >> 0) & 0xFF);
	byte highByte = ((p_value >> 8) & 0xFF);

	EEPROM.write(p_address, lowByte);
	EEPROM.write(p_address + 1, highByte);
}

unsigned int eeprom_read_int(int p_address) {
	byte lowByte = EEPROM.read(p_address);
	byte highByte = EEPROM.read(p_address + 1);

	return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
}

void displayTime() {
	myRTC.updateTime();
	print2digits(myRTC.hours);
	lcd.print(":");
	print2digits(myRTC.minutes);
	lcd.print(":");
	print2digits(myRTC.seconds);
}

void print2digits(int number) {
	if (number >= 0 && number < 10)
		lcd.print('0');
	lcd.print(number, DEC);
}



