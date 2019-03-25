/*
 Name:		PCF8574LCDControllerDemo.ino
 Created:	18/02/2019 11:03:26 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
*/

#include "PCF8574LCDController.h"

PCF8574LCDController* display = nullptr;

const uint8_t exampleCustomChar5x8[] = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x0 };
// the setup function runs once when you press reset or power the board
void setup() {
	Wire.setClock(100000);//for PCF8574 100khz clock is max
	Wire.begin();
	Serial.begin(115200);
	display = new PCF8574LCDController(PCF8574_DEFAULT_ADDRESS, 20, 4, 0);//address of PCF8754(A), number of chars in line, number of lines, font (0 = 5x8, 1 = 5x10), optional LCD controller declaration
	display->write5x8CharDataToCGRAM(0, exampleCustomChar5x8);//write example custom char to LCD CGRAM
	Serial.println("...PCF8574 LCD Controller Demo started...");
}

// the loop function runs over and over again until power down or reset
void loop() {
	if (!(millis() % 1000))//do this every second
	{
		display->setCursorToPosition(0, 0);

		display->displayData(static_cast<char>(0));//use custom char number 0
		display->displayData("LCD TEST");
		display->displayData(static_cast<char>(0));

		display->setCursorToPosition(0, 1);//set cursor to second line
		display->displayData("RANDOM CHAR: ");
		display->displayData(static_cast<uint8_t>(random(8, 256)));

		display->setCursorToPosition(0, 2);
		display->displayData("RUNTIME: ");
		display->displayData(String(millis() / 1000).c_str());

		display->setCursorToPosition(0, 3);
		display->displayData(static_cast<char>(0));//use custom char number 0
		display->displayData("LCD TEST");
		display->displayData(static_cast<char>(0));
	}
}
