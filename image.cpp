/*
 * =====================================================================================
 *
 *       Filename:  image.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  10/10/13 14:54:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Jonathan Hamilton (jh), jtrhamilton@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include "image.hpp"
#include <cstring> //Required for strerror() used in png++ header
#include <png++/png.hpp>

void
Image::writePNM(std::ofstream &stream)
{
	stream << "P3\n";
	stream << width << " " << height <<"\n";
	stream << "255\n";
	for (unsigned int row = 0; row < height; row++)
	{
		for (unsigned int col = 0; col < width; col++)
		{
			//No alpha support, so ignore that for now...
			unsigned int offset = row*width + col;
			stream << (unsigned int)data[offset].data[0] << " "
				   << (unsigned int)data[offset].data[1] << " "
				   << (unsigned int)data[offset].data[2] << " ";
		}
		stream << "\n";
	}
	stream.flush();
}

void
Image::writePNG(std::ofstream &stream)
{
	png::image< png::rgba_pixel > image(this->width, this->height);
	for (unsigned int y = 0; y < this->height; y++)
	{
		for (unsigned int x = 0; x < this->width; x++)
		{
			unsigned int offset = y*this->width + x;
			image[y][x] = png::rgba_pixel(data[offset].data[0],
			                              data[offset].data[1],
										  data[offset].data[2],
										  data[offset].data[3]);
		}
	}

	image.write_stream(stream);

}

void
PaletteImage::writePNG(std::ofstream &stream, const Palette &palette)
{
	png::image < png::index_pixel > image(this->width, this->height);
	png::palette pal(256);
	for (unsigned int i = 0; i < 256; i++)
	{
		pal[i] = png::color(palette.palette[i].data[0],
		                   palette.palette[i].data[1],
						   palette.palette[i].data[2]);
						   //FIXME: PNGs apparently support a palette
						   //with alpha, but png++ does not?
						   //palette.palette[i].data[3]);
	}
	image.set_palette(pal);
	for (unsigned int y = 0; y < this->height; y++)
	{
		for (unsigned int x = 0; x < this->width; x++)
		{
			unsigned int offset = y*this->width + x;
			image[y][x] = png::index_pixel(data[offset]);
		}
	}
	image.write_stream(stream);
}

Image
PaletteImage::toImage(const Palette &palette)
{
	Image image(width, height);
	for (unsigned int i = 0; i < width * height; i++)
	{
		image.data[i] = palette.palette[this->data[i]];
	}
	return image;
}

void
PaletteImage::drawAt(png::image< png::rgba_pixel > &image, unsigned int xOffset, unsigned int yOffset, const Palette &palette)
{
	for (unsigned int y = 0 ; y < this->height; y++)
	{
		for (unsigned int x = 0; x < this->width; x++)
		{
			unsigned int srcOffset = y*this->width + x;
			unsigned int index = this->data[srcOffset];
			if (index == 0)
				continue;
			Color c = palette.palette[index];
			image[y+yOffset][x+xOffset] = png::rgba_pixel(c.data[0], c.data[1],
			                                              c.data[2], c.data[3]);
		}
	}
}
