#include "apoclib/palette.h"
#include "apoclib/palette_image.h"
#include "apoclib/pck.h"

#include <iostream>
#include <memory>
#include <sstream>

static void print_usage()
{
	std::cout << "Usage: load_citymap palette.dat image.pck image.tab output_dir\n";
}

int main(int argc, char **argv)
{
	if (argc != 5)
	{
		print_usage();
		return EXIT_FAILURE;
	}

	std::ifstream paletteFile(argv[1], std::ios_base::in);
	if (!paletteFile)
	{
		std::cerr << "Failed to open \"" << argv[1] << "\"\n";
		return EXIT_FAILURE;
	}

	std::ifstream pckFile(argv[2], std::ios_base::in);
	if (!pckFile)
	{
		std::cerr << "Failed to open \"" << argv[2] << "\"\n";
		return EXIT_FAILURE;
	}

	std::ifstream tabFile(argv[3], std::ios_base::in);
	if (!tabFile)
	{
		std::cerr << "Failed to open \"" << argv[3] << "\"\n";
		return EXIT_FAILURE;
	}

	std::unique_ptr<ApocRes::Palette> pal(ApocRes::Palette::loadFromFile(paletteFile));

	std::unique_ptr<ApocRes::PCK> pck(ApocRes::PCK::loadFromFile(pckFile, tabFile));

	for (unsigned int i = 0; i < pck->images.size(); i++)
	{
		auto &image = pck->images[i];
		std::stringstream ss;
		ss << argv[4] << "/" << i << ".png";
		auto pngImage = image.getPNG(*pal);
		pngImage.write(ss.str());
	}

}
