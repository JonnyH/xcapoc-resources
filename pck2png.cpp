#include "apoclib/palette.h"
#include "apoclib/palette_image.h"
#include "apoclib/pck.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <map>
#include <string>
#include <getopt.h>

using namespace ApocRes;

std::map<std::string, PCK::PCKType> pckTypes
{
	{"RLEA", PCK::PCKType::RLEA},
	{"RLEB", PCK::PCKType::RLEB},
	{"BLK", PCK::PCKType::BLK},
	{"SHADOW", PCK::PCKType::SHADOW},
};

static void print_usage()
{
	std::cout << "Usage: pck2png -f format -p palette.dat -i image.pck -t image.tab -o output_dir\n";
	std::cout << "Supported formats: ";
	for (auto &t : pckTypes)
		std::cout << "\"" << t.first << "\" ";
	std::cout << "\n";
}

int main(int argc, char **argv)
{
	unsigned int maxX = 0;
	unsigned int maxY = 0;

	std::string formatStr;
	std::string paletteStr;
	std::string pckfileStr;
	std::string tabfileStr;
	std::string outdirStr;

	int c;

	while ((c = getopt(argc, argv, "f:p:i:t:o:")) != -1)
	{
		switch (c)
		{
			case 'f':
				if (formatStr != "")
				{
					std::cerr << "Specified multiple formats\n";
					print_usage();
					return EXIT_FAILURE;
				}
				formatStr = optarg;
				break;
			case 'p':
				if (paletteStr != "")
				{
					std::cerr << "Specified multiple palettes\n";
					print_usage();
					return EXIT_FAILURE;
				}
				paletteStr = optarg;
				break;
			case 'i':
				if (pckfileStr != "")
				{
					std::cerr << "Specified multiple pck images\n";
					print_usage();
					return EXIT_FAILURE;
				}
				pckfileStr = optarg;
				break;
			case 't':
				if (tabfileStr != "")
				{
					std::cerr << "Specified multiple tab files\n";
					print_usage();
					return EXIT_FAILURE;
				}
				tabfileStr = optarg;
				break;
			case 'o':
				if (outdirStr != "")
				{
					std::cerr << "Specified multiple output directories\n";
					print_usage();
					return EXIT_FAILURE;
				}
				outdirStr = optarg;
				break;
			case '?':
			default:
				std::cerr << "Failed to parse options \"" << optopt << "\n";
				print_usage();
				return EXIT_FAILURE;
		}
	}
	if (formatStr == "")
	{
		std::cerr << "Must supply format\n";
		print_usage();
		return EXIT_FAILURE;
	}
	if (paletteStr == "")
	{
		std::cerr << "Must supply palette file\n";
		print_usage();
		return EXIT_FAILURE;
	}
	if (pckfileStr == "")
	{
		std::cerr << "Must supply PCK file\n";
		print_usage();
		return EXIT_FAILURE;
	}
	if (tabfileStr == "")
	{
		std::cerr << "Must supply TAB file\n";
		print_usage();
		return EXIT_FAILURE;
	}
	if (outdirStr == "")
	{
		std::cerr << "Must supply output directory\n";
		print_usage();
		return EXIT_FAILURE;
	}
	if (pckTypes.find(formatStr) == pckTypes.end())
	{
		std::cerr << "Unknown format \"" << formatStr << "\"\n";
		print_usage();
		return EXIT_FAILURE;
	}

	PCK::PCKType format = pckTypes[formatStr];

	std::ifstream paletteFile(paletteStr, std::ios_base::in);
	if (!paletteFile)
	{
		std::cerr << "Failed to open \"" << paletteStr << "\"\n";
		return EXIT_FAILURE;
	}

	std::unique_ptr<Palette> pal (Palette::loadFromFile(paletteFile));

	std::ifstream pckFile(pckfileStr, std::ios_base::in);
	if (!pckFile)
	{
		std::cerr << "Failed to open \"" << pckfileStr << "\"\n";
		return EXIT_FAILURE;
	}

	std::ifstream tabFile(tabfileStr, std::ios_base::in);
	if (!tabFile)
	{
		std::cerr << "Failed to open \"" << tabfileStr << "\"\n";
		return EXIT_FAILURE;
	}

	unsigned int i = 0;
	uint32_t tabOffset;

	while (tabFile.read((char*)&tabOffset, sizeof(tabOffset)))
	{
		if (format != PCK::PCKType::SHADOW)
			tabOffset *= 4;
		pckFile.seekg(tabOffset, std::ios::beg);
		if (!pckFile)
		{
			std::cerr << "Failed to seek to " << tabOffset << " at tab index " << i << "\n";
			return EXIT_FAILURE;
		}
		PaletteImage image = PCK::load(pckFile, format);
		std::cout << "Loaded image " << i << "\n";
		if (image.sizeX > maxX)
			maxX = image.sizeX;
		if (image.sizeY > maxY)
			maxY = image.sizeY;
		std::stringstream ss;
		ss << outdirStr << "/" << i << ".png";
		auto pngImage = image.getPNG(*pal);
		pngImage.write(ss.str());
		i++;
	}
	std::cout << "Max X:" << maxX << "\nMax Y:" << maxY << "\n";
}
