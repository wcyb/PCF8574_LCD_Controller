/*
 Name:		PCF8574LCDController.cpp
 Created:	18/02/2019 11:01:42 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
 Editor:	http://www.visualmicro.com
*/

#include "PCF8574LCDController.h"

uint8_t PCF8574LCDController::readDataFromDDRAM(const uint8_t& address)
{
	uint8_t DDRAMaddr = readAddressCounter();//save cursor position

	writeDataI2c(false, address | getEnumValue(HD44780CommandsReadWriteData::setDDRAMAddress));
	uint8_t data = readDataI2c(true);

	writeDataI2c(false, DDRAMaddr | getEnumValue(HD44780CommandsReadWriteData::setDDRAMAddress));//restore cursor position
	return data;
}

void PCF8574LCDController::write5x8CharDataToCGRAM(const uint8_t& charPosition, const uint8_t data[])
{
	uint8_t DDRAMaddr = readAddressCounter();//save cursor position

	writeDataI2c(false, ((charPosition & 0x07) << 3) | getEnumValue(HD44780CommandsReadWriteData::setCGRAMAddress));//set starting CGRAM address
	for (uint8_t i = 0; i < 8; i++) writeDataI2c(true, data[i]);

	writeDataI2c(false, DDRAMaddr | getEnumValue(HD44780CommandsReadWriteData::setDDRAMAddress));//restore cursor position
}

void PCF8574LCDController::write5x10CharDataToCGRAM(const uint8_t& charPosition, const uint8_t data[])
{
	uint8_t DDRAMaddr = readAddressCounter();//save cursor position

	writeDataI2c(false, ((charPosition & 0x06) << 3) | getEnumValue(HD44780CommandsReadWriteData::setCGRAMAddress));//set starting CGRAM address
	for (uint8_t i = 0; i < 11; i++) writeDataI2c(true, data[i]);

	writeDataI2c(false, DDRAMaddr | getEnumValue(HD44780CommandsReadWriteData::setDDRAMAddress));//restore cursor position
}

void PCF8574LCDController::eraseLine(const uint8_t& lineNumber)
{
	setCursorToPosition(static_cast<uint8_t>(0), lineNumber);
	for (uint8_t i = 0; i < m_displaySizeX; i++) displayData(' ');
	setCursorToPosition(static_cast<uint8_t>(0), lineNumber);
}

bool PCF8574LCDController::enableBacklight(void)
{
	uint8_t lcdState = 0;
	Wire.requestFrom(m_pcfAddress, static_cast<uint8_t>(1));
	if (Wire.available() != 1) return false;
	lcdState = Wire.read();

	lcdState |= 0x08;

	Wire.beginTransmission(m_pcfAddress);
	Wire.write(lcdState);
	Wire.endTransmission();

	m_backlightEnabled = true;
	return true;
}

bool PCF8574LCDController::disableBacklight(void)
{
	uint8_t lcdState = 0;
	Wire.requestFrom(m_pcfAddress, static_cast<uint8_t>(1));
	if (Wire.available() != 1) return false;
	lcdState = Wire.read();

	lcdState &= 0xF7;

	Wire.beginTransmission(m_pcfAddress);
	Wire.write(lcdState);
	Wire.endTransmission();

	m_backlightEnabled = false;
	return true;
}

void PCF8574LCDController::initializeDisplayHD44780(void)
{
	unsigned long startTime = millis();
	
	while (millis() - startTime < 60UL) { asm("nop"); }//wait > 40ms after VDD > 2,7V or > 15ms after VDD > 4,5V
	writeNibbleDataI2c(false, setFunctionSet(getEnumValue(HD44780CommandsFunctionSet::set8bitInterface)));//send nibble with interface setted to 8bit

	startTime = millis();
	while (millis() - startTime < 6UL) { asm("nop"); }//wait > 4,1ms
	writeNibbleDataI2c(false, m_functionSet);//send last setting again
	
	startTime = millis();
	while (millis() - startTime < 2UL) { asm("nop"); }//wait > 100us
	writeNibbleDataI2c(false, m_functionSet);//send last setting again
	
	writeNibbleDataI2c(false, setFunctionSet(getEnumValue(HD44780CommandsFunctionSet::set4bitInterface)));//set interface to 4bit

	if (m_displayFont == 1) m_functionSet ^= 0x04;//choose 5x10 font
	if (m_displaySizeY == 1) m_functionSet ^= 0x08;//choose 1 line display
	writeDataI2c(false, m_functionSet);//set number of lines and font

	writeDataI2c(false, 0x08);//disable cursor, cursor blinking, and display

	clearDisplay();

	writeDataI2c(false, m_entryMode);//set cursor moving right and no display shift

	writeDataI2c(false, setDisplayControl(getEnumValue(HD44780CommandsDisplayControl::setDisplayOn)));
}

void PCF8574LCDController::initializeDisplayKS0066(void)
{
	unsigned long startTime = millis();

	while (millis() - startTime < 40UL) { asm("nop"); }//wait > 30ms after VDD > 4,5V
	writeNibbleDataI2c(false, m_functionSet);//send nibble with interface setted to 4bit

	if (m_displayFont == 1) m_functionSet ^= 0x04;//choose 5x10 font
	if (m_displaySizeY == 1) m_functionSet ^= 0x08;//choose 1 line display
	writeDataI2c(false, m_functionSet);//set number of lines and font

	startTime = millis();
	while (millis() - startTime < 2UL) { asm("nop"); }//wait > 39us
	writeDataI2c(false, 0x08);//disable cursor, cursor blinking, and display

	startTime = millis();
	while (millis() - startTime < 2UL) { asm("nop"); }//wait > 39us

	clearDisplay();

	startTime = millis();
	while (millis() - startTime < 3UL) { asm("nop"); }//wait > 1,5ms

	writeDataI2c(false, m_entryMode);//set cursor moving right and no display shift

	writeDataI2c(false, setDisplayControl(getEnumValue(HD44780CommandsDisplayControl::setDisplayOn)));
}

uint8_t PCF8574LCDController::readDataFromCGRAM(const uint8_t& address)
{
	uint8_t DDRAMaddr = readAddressCounter();//save cursor position

	writeDataI2c(false, address | getEnumValue(HD44780CommandsReadWriteData::setCGRAMAddress));//set address of CGRAM to read
	uint8_t data = readDataI2c(true);//read data from CGRAM

	writeDataI2c(false, DDRAMaddr | getEnumValue(HD44780CommandsReadWriteData::setDDRAMAddress));//restore cursor position
	return data;//return readed data
}

void PCF8574LCDController::writeDataToCGRAM(const uint8_t& address, const uint8_t& data)
{
	uint8_t DDRAMaddr = readAddressCounter();//save cursor position

	writeDataI2c(false, address | getEnumValue(HD44780CommandsReadWriteData::setCGRAMAddress));//set CGRAM position
	writeDataI2c(true, data);//write data to given position

	writeDataI2c(false, DDRAMaddr | getEnumValue(HD44780CommandsReadWriteData::setDDRAMAddress));//restore cursor position
}

void PCF8574LCDController::writeDataI2c(const bool& registerSelect, const uint8_t& dataToWrite)
{
	while (readBusyFlag()) { asm("nop"); }
	writeNibbleDataI2c(registerSelect, dataToWrite);
	writeNibbleDataI2c(registerSelect, dataToWrite << 4);
}

void PCF8574LCDController::writeNibbleDataI2c(const bool& registerSelect, const uint8_t& data)
{
	uint8_t writeParams = 0;
	if (registerSelect) writeParams |= 0x01;
	if (m_backlightEnabled) writeParams |= 0x08;

	for (uint8_t i = 0; i < 3; i++)
	{
		Wire.beginTransmission(m_pcfAddress);
		Wire.write((data & 0xF0) | writeParams);
		Wire.endTransmission();

		writeParams ^= 0x04;//change state of E line
	}
}

uint8_t PCF8574LCDController::readDataI2c(const bool& registerSelect)
{
	uint8_t readParams = 0xF2;//set high state for data lines as weak pullup
	uint8_t readedData = 0;

	if (registerSelect) readParams |= 0x01;
	if (m_backlightEnabled) readParams |= 0x08;

	for (uint8_t i = 0;i < 5; i++)
	{
		Wire.beginTransmission(m_pcfAddress);
		Wire.write(readParams);
		Wire.endTransmission();

		readParams ^= 0x04;//change state of E line

		if (i == 1) 
		{
			Wire.requestFrom(m_pcfAddress, static_cast<uint8_t>(1));
			if (Wire.available() != 1) return 0x00;
			readedData = Wire.read() & 0xF0;
		}
		else if (i == 3)
		{
			Wire.requestFrom(m_pcfAddress, static_cast<uint8_t>(1));
			if (Wire.available() != 1) return 0x00;
			readedData |= (Wire.read() & 0xF0) >> 4;
		}
	}

	return readedData;
}