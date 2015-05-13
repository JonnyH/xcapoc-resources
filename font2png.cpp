#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include "apoclib/palette_image.h"
#include "apoclib/palette.h"

int main(int argc, char **argv)
{
	if (argc != 6)
	{
		std::cerr << "Usage: font2png font.dat height width palette.dat outputDirectory\n";
		return EXIT_FAILURE;
	}
	int idx = 0;
	int height = atoi(argv[2]);
	int width = atoi(argv[3]);
	std::string palFileName(argv[4]);
	std::cerr << "Loading " << argv[1] << " size: " << width << " x " << height << "\n";
	std::ifstream file(argv[1]);
	std::ifstream palFile(palFileName);
	assert(palFile);
	std::unique_ptr<ApocRes::Palette> pal (ApocRes::Palette::loadFromFile(palFile));

	std::unique_ptr<char[]> glyphData(new char[height*width]);

	while (file.read(glyphData.get(), height*width) && file)
	{
		ApocRes::PaletteImage img(width, height);
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				img.setPixel(x, y, glyphData[y*width + x]);
			}
		}
		std::stringstream outFileName;
		outFileName << argv[5] << "/" << idx++ << ".png";
		auto png = img.getPNG(*pal);
		png.write(outFileName.str());
	}
}
