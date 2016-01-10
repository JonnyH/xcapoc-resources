#include "palette.h"
#include <cstdint>
#include <memory>

namespace ApocRes {

Palette::Palette(unsigned int size)
	:pixels(size)
{
}

png::rgba_pixel
Palette::getPixel(unsigned int idx) const
{
	if (idx >= this->pixels.size())
		return png::rgba_pixel(0,0,0,0);
	return this->pixels[idx];
}

void
Palette::setPixel(unsigned int idx, png::rgba_pixel pix)
{
	if (idx >= this->pixels.size())
		return;
	this->pixels[idx] = pix;
}

Palette*
Palette::loadFromMemory(const char *data, unsigned int length, bool expandVGA)
{
	unsigned int entries = length / 3;
	Palette *p = new Palette(entries);

	const char *pos = data;

	for (unsigned int i = 0; i < entries; i++)
	{
		uint8_t r, g, b, a;
		r = g = b = 0;
		if (i == 0)
			a = 0;
		else
			a = 255;
		r = *pos++;
		g = *pos++;
		b = *pos++;
		if (expandVGA)
		{
			r <<= 2;
			g <<= 2;
			b <<= 2;
		}
		p->pixels[i] = png::rgba_pixel(r,g,b,a);
	}

	return p;
}

Palette*
Palette::loadFromFile(std::ifstream &file, bool expandVGA)
{
	file.seekg(0, std::ios::end);
	auto size = file.tellg();
	file.seekg(0, std::ios::beg);
	std::unique_ptr<char[]> data(new char[size]);
	file.read((char*)data.get(), size);
	return Palette::loadFromMemory(data.get(), size, expandVGA);
}

}; //namespace ApocRes
