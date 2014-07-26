#pragma once

//cstring required by png++ (strerror)
#include <cstring>
#include <png++/png.hpp>

namespace ApocRes {

class Palette;

class PaletteImage
{
	public:
		PaletteImage(unsigned int sizeX, unsigned int sizeY);
		std::vector<unsigned int> indices;
		png::image<png::rgba_pixel> getPNG(const Palette &pal);
		unsigned int sizeX, sizeY;

		void setPixel(unsigned int x, unsigned int y, unsigned int idx)
		{
			indices[y*this->sizeX + x] = idx;
		}
};

};
