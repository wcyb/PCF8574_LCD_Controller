/*
 Name:		PCF8574LCDController.h
 Created:	18/02/2019 11:01:42 PM
 Author:	Wojciech Cybowski (github.com/wcyb)
 License:	GPL v2
 Editor:	http://www.visualmicro.com
*/

#ifndef _PCF8574LCDController_h
#define _PCF8574LCDController_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <Wire.h>

#define PCF8574_DEFAULT_ADDRESS 0x20
#define PCF8574_ALTERNATE_DEFAULT_ADDRESS 0x27
#define PCF8574A_DEFAULT_ADDRESS 0x38
#define PCF8574A_ALTERNATE_DEFAULT_ADDRESS 0x3F

template<typename Enumerator>
uint8_t getEnumValue(const Enumerator& enumType)
{
	return static_cast<uint8_t>(enumType);
}

class PCF8574LCDController final
{
public:
	enum class LCDDriverType : uint8_t { HD44780, SPLC780D, KS0066 };

	/// <summary>
	/// Clears display.
	/// </summary>
	void clearDisplay(void) { writeDataI2c(false, getEnumValue(HD44780CommandsBase::clearDisplay)); }

	/// <summary>
	/// Moves cursor to home position (first char in first line).
	/// </summary>
	void moveToHome(void) { writeDataI2c(false, getEnumValue(HD44780CommandsBase::returnHome)); }

	/// <summary>
	/// Disables display (clear without delete).
	/// </summary>
	void disableDisplay(void) { writeDataI2c(false, setDisplayControl(getEnumValue(HD44780CommandsDisplayControl::setDisplayOff))); }

	/// <summary>
	/// Enables display.
	/// </summary>
	void enableDisplay(void) { writeDataI2c(false, setDisplayControl(getEnumValue(HD44780CommandsDisplayControl::setDisplayOn))); }

	/// <summary>
	/// Disables displaying of cursor.
	/// </summary>
	void disableCursor(void) { writeDataI2c(false, setDisplayControl(getEnumValue(HD44780CommandsDisplayControl::setCursorOff))); }

	/// <summary>
	/// Enables displaying of cursor.
	/// </summary>
	void enableCursor(void) { writeDataI2c(false, setDisplayControl(getEnumValue(HD44780CommandsDisplayControl::setCursorOn))); }

	/// <summary>
	/// Disables blinking of last entered char.
	/// </summary>
	void disableBlinking(void) { writeDataI2c(false, setDisplayControl(getEnumValue(HD44780CommandsDisplayControl::setCursorBlinkingOff))); }

	/// <summary>
	/// Enables blinking of last entered char.
	/// </summary>
	void enableBlinking(void) { writeDataI2c(false, setDisplayControl(getEnumValue(HD44780CommandsDisplayControl::setCursorBlinkingOn))); }

	/// <summary>
	/// Sets displaying of entered data right-to-left.
	/// </summary>
	void displayToLeft(void) { writeDataI2c(false, setEntryMode(getEnumValue(HD44780CommandsEntryMode::moveCursorRightWhenSendingData))); }

	/// <summary>
	/// Sets displaying of entered data left-to-right.
	/// </summary>
	void displayToRight(void) { writeDataI2c(false, setEntryMode(getEnumValue(HD44780CommandsEntryMode::moveCursorLeftWhenSendingData))); }

	/// <summary>
	/// Enables display shift on data write.
	/// </summary>
	void enableDisplayShiftOnDataWrite(void) { writeDataI2c(false, setEntryMode(getEnumValue(HD44780CommandsEntryMode::shiftDisplayWhenSendingData))); }

	/// <summary>
	/// Disables display shift on data write.
	/// </summary>
	void disableDisplayShiftOnDataWrite(void) { writeDataI2c(false, setEntryMode(getEnumValue(HD44780CommandsEntryMode::dontShiftDisplayWhenSendingData))); }

	/// <summary>
	/// Shifts cursor one char to left.
	/// </summary>
	void shiftCursorToLeft(void) { writeDataI2c(false, setCursorOrDisplayShift(getEnumValue(HD44780CommandsCursorDisplayShift::moveCursorLeft))); }

	/// <summary>
	/// Shifts cursor one char to right.
	/// </summary>
	void shiftCursorToRight(void) { writeDataI2c(false, setCursorOrDisplayShift(getEnumValue(HD44780CommandsCursorDisplayShift::moveCursorRight))); }

	/// <summary>
	/// Shifts display one char to left.
	/// </summary>
	void shiftDisplayToLeft(void) { writeDataI2c(false, setCursorOrDisplayShift(getEnumValue(HD44780CommandsCursorDisplayShift::shiftDisplayLeft))); }

	/// <summary>
	/// Shifts display one char to right.
	/// </summary>
	void shiftDisplayToRight(void) { writeDataI2c(false, setCursorOrDisplayShift(getEnumValue(HD44780CommandsCursorDisplayShift::shiftDisplayRight))); }

	/// <summary>
	/// Sets cursor to given position.
	/// </summary>
	/// <param name="x">Char position</param>
	/// <param name="y">Line (from 0 to 3)</param>
	void setCursorToPosition(const uint8_t& x, const uint8_t& y) { if (y > 3) return; writeDataI2c(false, (m_lineOffset[y] + x) | getEnumValue(HD44780CommandsReadWriteData::setDDRAMAddress)); }

	/// <summary>
	/// Writes one byte of data to DDRAM.
	/// </summary>
	/// <param name="address">Address in DDRAM</param>
	/// <param name="data">Data to write</param>
	void writeDataToDDRAM(const uint8_t& address, const uint8_t& data) { writeDataI2c(false, address | getEnumValue(HD44780CommandsReadWriteData::setDDRAMAddress)); writeDataI2c(true, data); }

	/// <summary>
	/// Reads one byte from DDRAM.
	/// </summary>
	/// <param name="address">Address in DDRAM</param>
	/// <returns>Readed byte</returns>
	uint8_t readDataFromDDRAM(const uint8_t& address);

	/// <summary>
	/// Sends data to display.
	/// </summary>
	/// <param name="dataToDisplay">String to display</param>
	void displayData(const char dataToDisplay[]) { while (*dataToDisplay) { writeDataToDDRAM(*dataToDisplay++); } }

	/// <summary>
	/// Sends data to display.
	/// </summary>
	/// <param name="dataToDisplay">Char to display</param>
	void displayData(const char& dataToDisplay) { writeDataToDDRAM(dataToDisplay); }

	/// <summary>
	/// Writes given char of size 5x8 to CGRAM. After that, given char may be displayed as other standard chars.
	/// </summary>
	/// <param name="charPosition">Char position (from 0 to 7)</param>
	/// <param name="data">New char data array</param>
	void write5x8CharDataToCGRAM(const uint8_t& charPosition, const uint8_t data[]);

	/// <summary>
	/// Writes given char of size 5x10 to CGRAM. After that, given char may be displayed as other standard chars.
	/// </summary>
	/// <param name="charPosition">Char position (from 0 to 3)</param>
	/// <param name="data">New char data array</param>
	void write5x10CharDataToCGRAM(const uint8_t& charPosition, const uint8_t data[]);

	/// <summary>
	/// Erases line by writing blanks on whole line. After that it will set cursor to beginning of chosen line.
	/// </summary>
	/// <param name="lineNumber">Line to erase</param>
	void eraseLine(const uint8_t& lineNumber);

	/// <summary>
	/// Enables backlight.
	/// </summary>
	/// <returns>True if changed successfuly</returns>
	bool enableBacklight(void);

	/// <summary>
	/// Disables backlight.
	/// </summary>
	/// <returns>True if changed successfuly</returns>
	bool disableBacklight(void);

	/// <summary>
	/// Returns cursor moving direction and status of display shifting when entering data.
	/// </summary>
	/// <returns>Byte formatted as in controller documentation</returns>
	uint8_t getCurrentEntryMode(void) { return m_entryMode; }

	/// <summary>
	/// Returns status (enabled/disabled) of display, cursor and blinking.
	/// </summary>
	/// <returns>Byte formatted as in controller documentation</returns>
	uint8_t getCurrentDisplayControl(void) { return m_displayControl; }

	/// <summary>
	/// Returns setting of cursor and display shift.
	/// </summary>
	/// <returns>Byte formatted as in controller documentation</returns>
	uint8_t getCurrentCursorOrDisplayShift(void) { return m_cursorOrDisplayShift; }

	/// <summary>
	/// Returns status of interface data lenght, display lines and font.
	/// </summary>
	/// <returns>Byte formatted as in controller documentation</returns>
	uint8_t getCurrentFunctionSet(void) { return m_functionSet; }

	/// <summary>
	/// Constructor for PCF8574 LCD Controller.
	/// </summary>
	/// <param name="pcf8574Address">I2C address of PCF8574</param>
	/// <param name="sizeX">Chars in line</param>
	/// <param name="sizeY">Lines count</param>
	/// <param name="font">Font type selection. 0 = 5x8, 1 = 5x10</param>
	PCF8574LCDController(
		const uint8_t& pcf8574Address, const uint8_t& sizeX, const uint8_t& sizeY, 
		const uint8_t& font = 0, LCDDriverType driverType = LCDDriverType::HD44780) :
		m_pcfAddress(pcf8574Address), m_displaySizeX(sizeX), m_displaySizeY(sizeY),
		m_displayFont(font) 
	{
		if (driverType == LCDDriverType::HD44780 || driverType == LCDDriverType::SPLC780D) initializeDisplayHD44780();
		else initializeDisplayKS0066();
	}

	PCF8574LCDController(const PCF8574LCDController&) = delete;
	PCF8574LCDController& operator=(const PCF8574LCDController&) = delete;

private:
	/// <summary>
	/// Number of chars in line.
	/// </summary>
	const uint8_t m_displaySizeX;

	/// <summary>
	/// Number of lines.
	/// </summary>
	const uint8_t m_displaySizeY;

	/// <summary>
	/// Address offset for each display line.
	/// </summary>
	const uint8_t m_lineOffset[4] = { 0x00, 0x40, m_displaySizeX, 0x40 + m_displaySizeX };

	/// <summary>
	/// Display font type.
	/// </summary>
	const uint8_t m_displayFont;

	/// <summary>
	/// State of backlight.
	/// </summary>
	bool m_backlightEnabled = true;

	/// <summary>
	/// I2C address of PCF8574.
	/// </summary>
	const uint8_t m_pcfAddress;

private:
	/// <summary>
	/// Function set for entry mode.
	/// </summary>
	uint8_t m_entryMode = 0x06;//default: cursor move right and no shift of display

	/// <summary>
	/// Function set for display control.
	/// </summary>
	uint8_t m_displayControl = 0x0C;//default: display on, no cursor, no blinking

	/// <summary>
	/// Function set for cursor and display shift.
	/// </summary>
	uint8_t m_cursorOrDisplayShift = 0x10;//default: moves cursor without display shift

	/// <summary>
	/// Function set for LCD controller.
	/// </summary>
	uint8_t m_functionSet = 0x28;//default: 4bit interface, two lines

private:
	/// <summary>
	/// Initialization of LCD controller.
	/// </summary>
	void initializeDisplayHD44780(void);

	/// <summary>
	/// Initialization of LCD controller.
	/// </summary>
	void initializeDisplayKS0066(void);

	/// <summary>
	/// Sets how data will be processed by display when entered (shift data, left or right shift).
	/// </summary>
	/// <param name="newState">New function set</param>
	/// <returns>Data describing new state</returns>
	uint8_t setEntryMode(const uint8_t& newState) { if (newState > 0x10) return m_entryMode &= newState; else return m_entryMode |= newState; }//| 0x04

	/// <summary>
	/// Sets how data will be displayed (cursor, blinking).
	/// </summary>
	/// <param name="newState">New function set</param>
	/// <returns>Data describing new state</returns>
	uint8_t setDisplayControl(const uint8_t& newState) { if (newState > 0x10) return m_displayControl &= newState; else return m_displayControl |= newState; }//| 0x08

	/// <summary>
	/// Moves cursor of shifts displayed data.
	/// </summary>
	/// <param name="newState">New function set</param>
	/// <returns>Data describing new state</returns>
	uint8_t setCursorOrDisplayShift(const uint8_t& newState) { m_cursorOrDisplayShift = 0x00; return (m_cursorOrDisplayShift |= newState) | 0x10; }//| 0x10

	/// <summary>
	/// Sets data displaying parameters (font, number of lines).
	/// </summary>
	/// <param name="newState">New function set</param>
	/// <returns>Data describing new state</returns>
	uint8_t setFunctionSet(const uint8_t& newState) { if (newState > 0x10) return m_functionSet &= newState; else return m_functionSet |= newState; }//| 0x20

	/// <summary>
	/// Reads busy flag from display controller.
	/// </summary>
	/// <returns>True if busy</returns>
	bool readBusyFlag(void) { return readDataI2c(false) & 0x80; }

	/// <summary>
	/// Reads current address from address counter.
	/// </summary>
	/// <returns>Address counter value</returns>
	uint8_t readAddressCounter(void) { return readDataI2c(false) & 0x7F; }

	/// <summary>
	/// Reads one byte from CGRAM.
	/// </summary>
	/// <param name="address">Address in CGRAM</param>
	/// <returns>Readed byte</returns>
	uint8_t readDataFromCGRAM(const uint8_t& address);

	/// <summary>
	/// Writes one byte of data to CGRAM.
	/// </summary>
	/// <param name="address">Position of data in CGRAM</param>
	/// <param name="data">Data to write</param>
	void writeDataToCGRAM(const uint8_t& address, const uint8_t& data);

	/// <summary>
	/// Writes one byte of data to DDRAM on position pointed by current value of address counter.
	/// </summary>
	/// <param name="data">Data to write</param>
	void writeDataToDDRAM(const uint8_t& data) { writeDataI2c(true, data); }

private:
	/// <summary>
	/// Writes one byte to LCD at current address counter.
	/// </summary>
	/// <param name="rs">Register select. False = instruction register, true = data register</param>
	/// <param name="data">Byte to write</param>
	void writeDataI2c(const bool& rs, const uint8_t& data);

	/// <summary>
	/// Writes half byte to LCD at current address counter.
	/// </summary>
	/// <param name="rs">Register select. False = instruction register, true = data register</param>
	/// <param name="data">Byte to write. Only upper half will be used</param>
	void writeNibbleDataI2c(const bool& rs, const uint8_t& data);

	/// <summary>
	/// Reads one byte from LCD at current address counter.
	/// </summary>
	/// <param name="rs">Register select. False = instruction register, true = data register</param>
	/// <returns>Readed byte</returns>
	uint8_t readDataI2c(const bool& rs);

private:
	enum class HD44780CommandsBase : uint8_t {
		clearDisplay = 0x01,
		returnHome = 0x02
	};

	enum class HD44780CommandsEntryMode : uint8_t {
		shiftDisplayWhenSendingData = 0x01,
		dontShiftDisplayWhenSendingData = 0xFE,
		moveCursorLeftWhenSendingData = 0x02,
		moveCursorRightWhenSendingData = 0xFD
	};

	enum class HD44780CommandsDisplayControl : uint8_t {
		setCursorBlinkingOn = 0x01,
		setCursorBlinkingOff = 0xFE,
		setCursorOn = 0x02,
		setCursorOff = 0xFD,
		setDisplayOn = 0x04,
		setDisplayOff = 0xFB
	};

	enum class HD44780CommandsCursorDisplayShift : uint8_t {
		moveCursorLeft = 0x00,
		moveCursorRight = 0x04,
		shiftDisplayLeft = 0x08,
		shiftDisplayRight = 0x0C
	};

	enum class HD44780CommandsFunctionSet : uint8_t {
		setFont5x10 = 0x04,
		setFont5x8 = 0xFB,
		setTwoLinesDisplay = 0x08,
		setOneLineDisplay = 0xF7,
		set8bitInterface = 0x10,
		set4bitInterface = 0xEF
	};

	enum class HD44780CommandsReadWriteData : uint8_t {
		setCGRAMAddress = 0x40,
		setDDRAMAddress = 0x80
	};
};

#endif
