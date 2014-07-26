#include "palette_image.h"
#include "palette.h"

namespace ApocRes {

PaletteImage::PaletteImage(unsigned int sizeX, unsigned int sizeY)
	: sizeX(sizeX), sizeY(sizeY)
{
	indices.resize(sizeX*sizeY);
}

png::image<png::rgba_pixel>
PaletteImage::getPNG(const Palette &pal)
{
	png::image<png::rgba_pixel> image(this->sizeX, this->sizeY);
	for (unsigned int y = 0; y < this->sizeY; y++)
	{
		for (unsigned int x = 0; x < this->sizeX; x++)
		{
			image[y][x] = pal.getPixel(this->indices[this->sizeX * y + x]);
		}
	}
	return image;
}

}; //namespace ApocRes
