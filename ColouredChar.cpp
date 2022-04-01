#include "ColouredChar.h"

ColouredChar::ColouredChar(char ch, Color color) : _ch(ch), _color(color) {}

ColouredChar::ColouredChar(const ColouredChar& ch) : _ch(ch._ch), _color(ch._color) {}

ColouredChar::ColouredChar(ColouredChar&& ch) : _ch(ch._ch), _color(ch._color) {}

ColouredChar::~ColouredChar() {}

ColouredChar& ColouredChar::operator=(const ColouredChar& ch)
{
	if (this != &ch)
	{
		this->_ch = ch._ch;
		this->_color = ch._color;
	}
	return *this;
}

ColouredChar& ColouredChar::operator=(ColouredChar&& ch)
{
	if (this != &ch)
	{
		this->_ch = ch._ch;
		this->_color = ch._color;
	}
	return *this;
}

void ColouredChar::setChar(const char ch)
{
	this->_ch = ch;
}

void ColouredChar::setColor(const Color color)
{
	this->_color = color;
}

char ColouredChar::getChar() const
{
	return _ch;
}

Color ColouredChar::getColor() const
{
	return _color;
}

ColouredChar::operator int() const
{
	return static_cast<int>(_ch);
}

ColouredChar ColouredChar::strTerminator()
{
	return {terminator, defaultColor};
}

bool operator==(const ColouredChar& left, const ColouredChar& right)
{
	return (left.getChar() == right.getChar()) && (left.getColor() == right.getColor());
}

bool operator!=(const ColouredChar& left, const ColouredChar& right)
{
	return !(left == right);
}

//in order comparison color doesn't matter, comparing only chars
bool operator<(const ColouredChar& left, const ColouredChar& right)
{
	return left.getChar() < right.getChar();
}

bool operator<=(const ColouredChar& left, const ColouredChar& right)
{
	return (left == right) || (left < right);
}

bool operator>(const ColouredChar& left, const ColouredChar& right)
{
	return left.getChar() > right.getChar();
}

bool operator>=(const ColouredChar& left, const ColouredChar& right)
{
	return (left == right) || (left > right);
}

std::ostream& operator<<(std::ostream& os, const ColouredChar& ch)
{
	os << '(' << ch.getChar() << ", " << ch.getColor() << ')';
	return os;
}

std::string stringVal(const Color& color)
{

	switch (color)
	{
		case Color::Black: return { "Black" };
		case Color::White: return { "White" };
		case Color::Green: return { "Green" };
		case Color::Red: return { "Red" };
		case Color::Blue: return { "Blue" };
		case Color::Yellow: return { "Yellow" };
		default: return "Unknown";
	}
}

std::ostream& operator<<(std::ostream& os, const Color& color)
{
	os << stringVal(color);
	return os;
}

