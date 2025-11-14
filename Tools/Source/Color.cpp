#include <Tools/pch.h>
#include <Tools/Header/Color.h>

const Color Color::Black(0, 0, 0, 1);
const Color Color::White(1, 1, 1, 1);
const Color Color::Red(1, 0, 0, 1);
const Color Color::Green(0, 1, 0, 1);
const Color Color::Blue(0, 0, 1, 1);
const Color Color::Yellow(1, 1, 0, 1);
const Color Color::Magenta(1, 0, 1, 1);
const Color Color::Cyan(0, 1, 1, 1);
const Color Color::Transparent(0, 0, 0, 0);

Color::Color() : r(0), g(0), b(0), a(1)
{
}

Color::Color(float red, float green, float blue, float alpha) : r(red), g(green), b(blue), a(alpha)
{
}

Color& Color::operator=(const Color& other)
{
	if (this != &other)
	{
		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
	}
	return *this;
}