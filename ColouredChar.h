#pragma once
#include <iostream>

enum class Color : int
{
	Black = 1,
	White = 2,
	Green = 3,
	Red = 4,
	Blue = 5,
	Yellow = 6
};

std::string stringVal(const Color& color);

std::ostream& operator<<(std::ostream& os, const Color& color);

static const char defaultChar = 'a';
static const char terminator = '\0';
static const Color defaultColor = Color::Black;

class ColouredChar
{
private:

	Color _color;
	char _ch;

public:

	ColouredChar(char ch = defaultChar, Color color = defaultColor);
	ColouredChar(const ColouredChar& ch);
	ColouredChar(ColouredChar&& ch);
	~ColouredChar();

	ColouredChar& operator=(const ColouredChar& ch);
	ColouredChar& operator=(ColouredChar&& ch);

	void setChar(const char ch);
	void setColor(const Color color);

	char getChar() const;
	Color getColor() const;

	operator int() const;

	static ColouredChar strTerminator();
};

bool operator==(const ColouredChar& left, const ColouredChar& right);
bool operator!=(const ColouredChar& left, const ColouredChar& right);

//in order comparison color doesn't matter, comparing only chars
bool operator<(const ColouredChar& left, const ColouredChar& right);
bool operator<=(const ColouredChar& left, const ColouredChar& right);
bool operator>(const ColouredChar& left, const ColouredChar& right);
bool operator>=(const ColouredChar& left, const ColouredChar& right);

std::ostream& operator<<(std::ostream& os, const ColouredChar& ch);
