#ifndef __IMAGE_HPP
#define __IMAGE_HPP

#include "color.hpp"
#include "palette.hpp"

#include <memory>
#include <fstream>
#include <cstring>
#include <png++/png.hpp>


class Image
{
public:
	Image(unsigned int width, unsigned int height)
		: width(width), height(height), data(new Color[height*width])
	{}

	void writePNM(std::ofstream &stream);
	void writePNG(std::ofstream &stream);

	unsigned int width;
	unsigned int height;
	std::unique_ptr<Color[]> data;
};

class PaletteImage
{
public:
	PaletteImage(int width, int height)
		:	width(width), height(height), data(new uint8_t[height*width])
	{}
	Image toImage(const Palette &palette);
	void writePNG(std::ofstream &stream, const Palette &palette);
	void drawAt(png::image< png::rgba_pixel > &image, unsigned int xOffset, unsigned int yOffset, const Palette &palette);

	unsigned int width;
	unsigned int height;
	std::unique_ptr<uint8_t[]> data;
};
#endif //__IMAGE_HPP
