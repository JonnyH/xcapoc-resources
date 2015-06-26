#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <cassert>

#include <cstring>
#include <png++/png.hpp>

//#define LSBFIRST


struct lof_header
{
	uint32_t width;
	uint32_t height;
};

static_assert(sizeof(struct lof_header) == 8, "lof_header wrong size");

int main(int argc, char **argv)
{
	if (argc != 4)
	{
		std::cerr << "Usage: lof2png loftemps.tab loftemps.dat outputDirectory\n";
		return EXIT_FAILURE;
	}
	int idx = 0;
	std::ifstream tabFile(argv[1]);
	std::cerr << "Loaded tabfile \"" << argv[1] << "\n";
	std::ifstream lofFile(argv[2]);
	std::cerr << "Loaded loffile \"" << argv[2] << "\n";
	assert(tabFile);
	assert(lofFile);

	uint32_t offset;

	while (tabFile.read((char*)&offset, 4))
	{
		struct lof_header header;
		std::cerr << "Seeking to offset " << offset * 4 << "\n";
		lofFile.seekg(offset * 4, std::ios::beg);
		assert(lofFile);
		lofFile.read((char*)&header, sizeof(header));
		assert(lofFile);
		png::image<png::rgba_pixel> image(header.width, header.height);
		std::cerr << "Reading idx " << idx << " " << header.width << " x " << header.height << "\n";
		assert(header.width % 8 == 0);
		for (int y = 0; y < header.height; y++)
		{
			//Lines are always aligned to 4 byte boundaries
			for (int x = 0; x < header.width; x+= 32)
			{
				uint32_t bitmask;
				lofFile.read((char*)&bitmask, 4);
				assert(lofFile);
				for (int bit = 0; bit < 32; bit++)
				{
					if (x + bit >= header.width)
						break;
					png::rgba_pixel pix;
#ifdef LSBFIRST
					if (bitmask & 0x1)
						pix = png::rgba_pixel(255,255,255,255);
					else
						pix = png::rgba_pixel(255,0,0,255);
					bitmask >>= 1;
#else
					if (bitmask & 0x80000000)
						pix = png::rgba_pixel(255,255,255,255);
					else
						pix = png::rgba_pixel(255,0,0,255);
					bitmask <<= 1;
#endif
					image[y][x+bit] = pix;
				}
			}
		}
		std::stringstream outFileName;
		outFileName << argv[3] << "/" << idx++ << ".png";
		image.write(outFileName.str());
	}
}
