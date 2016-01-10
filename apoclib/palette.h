#pragma once

#include <vector>
#include <cstring>
#include <fstream>
#include <png++/png.hpp>

namespace ApocRes {

class Palette
{
public:
		Palette(unsigned int size = 256);
		std::vector<png::rgba_pixel> pixels;
		static Palette *loadFromFile(std::ifstream &file, bool expandVGA = true);
		static Palette *loadFromMemory(const char* data, unsigned int size, bool expandVGA = true);
		png::rgba_pixel getPixel(unsigned int idx) const;
		void setPixel(unsigned int idx, png::rgba_pixel pix);
};


}; //namespace ApocRes
