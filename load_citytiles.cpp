#include "apoclib/city_tile.h"

#include <iostream>
#include <memory>
#include <sstream>


static void print_usage()
{
	std::cout << "Usage: load_citytiles citymap.dat loftemps.dat loftemps.tab output_directory\n";
}

int main(int argc, char **argv)
{
	if (argc != 5)
	{
		print_usage();
		return EXIT_FAILURE;
	}

	std::ifstream cityTileFile(argv[1], std::ios_base::in);
	if (!cityTileFile)
	{
		std::cerr << "Failed to open \"" << argv[1] << "\"\n";
		return EXIT_FAILURE;
	}

	std::string output_dir(argv[4]);

	auto voxelLayers = ApocRes::VoxelLayer::loadLayersFromFile(argv[2], argv[3]);

	int tileCount = 0;
	auto tile = ApocRes::CityMapTile::loadFromFile(cityTileFile, tileCount, voxelLayers);
	while (tile)
	{
		tinyxml2::XMLDocument doc{};
		auto rootElement = doc.NewElement("openapoc");
		doc.InsertFirstChild(rootElement);
		tile->WriteXML(rootElement);
		std::stringstream ss;
		ss << output_dir << "/citymap:" << tileCount << ".xml";

		doc.SaveFile(ss.str().c_str());


		delete tile;
		tileCount++;
		tile = ApocRes::CityMapTile::loadFromFile(cityTileFile, tileCount, voxelLayers);
	}
}
