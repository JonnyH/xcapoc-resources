#pragma once

#include <vector>
#include <cstring>
#include <fstream>
#include <png++/png.hpp>

namespace ApocRes {

class Palette
{
	private:
		Palette(unsigned int size = 256);
		std::vector<png::rgba_pixel> pixels;
	public:
		static Palette *loadFromFile(std::ifstream &file);
		png::rgba_pixel getPixel(unsigned int idx) const;
};


}; //namespace ApocRes
