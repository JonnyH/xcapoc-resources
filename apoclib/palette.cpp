#include "palette.h"
#include <cstdint>

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

Palette*
Palette::loadFromFile(std::ifstream &file)
{
	file.seekg(0, std::ios::end);
	unsigned int entries = file.tellg()/3;
	file.seekg(0, std::ios::beg);
	Palette *p = new Palette(entries);

	for (unsigned int i = 0; i < entries; i++)
	{
		uint8_t r, g, b, a;
		r = g = b = 0;
		if (i == 0)
			a = 0;
		else
			a = 255;
		file.read((char*)&r, 1);
		r <<= 2;
		file.read((char*)&g, 1);
		g <<= 2;
		file.read((char*)&b, 1);
		b <<= 2;
		p->pixels[i] = png::rgba_pixel(r,g,b,a);
	}

	return p;
}

}; //namespace ApocRes
