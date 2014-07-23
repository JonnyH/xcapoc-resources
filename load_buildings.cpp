#include "apoclib/building.h"
#include "apoclib/organisation.h"
#include "apoclib/city_tile.h"

#include <iostream>
#include <memory>

static void print_usage()
{
	std::cout << "Usage: load_buildings input.bld citymap.dat loftemps.dat loftemps.tab output.xml\n";
}

int main(int argc, char **argv)
{
	int buildingCount = 0;
	if (argc != 6)
	{
		print_usage();
		return EXIT_FAILURE;
	}
	ALLEGRO_FILE *bldFile = al_fopen(argv[1], "r");
	if (!bldFile)
	{
		std::cerr << "Failed to open \"" << argv[1] << "\"\n";
		return EXIT_FAILURE;
	}
	ALLEGRO_FILE *tileFile = al_fopen(argv[2], "r");
	if (!tileFile)
	{
		std::cerr << "Failed to open \"" << argv[2] << "\"\n";
		return EXIT_FAILURE;
	}
	std::vector<std::unique_ptr<ApocRes::Building> >buildings;

	auto voxelLayers = ApocRes::VoxelLayer::loadLayersFromFile(argv[3], argv[4]);

	auto names =
		ApocRes::Building::getDefaultNames();
	auto orgs =
		ApocRes::Organisation::getDefaultOrganisations();

	auto building = ApocRes::Building::loadFromFile(buildingCount, bldFile, orgs, names);
	while (building)
	{
		buildingCount++;
		buildings.emplace_back(building);
		building = ApocRes::Building::loadFromFile(buildingCount, bldFile, orgs, names);
	}

	std::cout << "Read " << buildings.size() << " buildings\n";

	std::vector<std::unique_ptr<ApocRes::CityMapTile> > tiles;
	int tileCount = 0;

	auto tile = ApocRes::CityMapTile::loadFromFile(tileFile, tileCount, voxelLayers);
	while (tile)
	{
		tileCount++;
		tiles.emplace_back(tile);
		tile = ApocRes::CityMapTile::loadFromFile(tileFile, tileCount, voxelLayers);
	}

	tinyxml2::XMLDocument *doc = new tinyxml2::XMLDocument();

	auto rootElement = doc->NewElement("openapoc");

	doc->InsertFirstChild(rootElement);

	auto buildingsElement = doc->NewElement("buildings");

	rootElement->InsertFirstChild(buildingsElement);

	for (auto &b : buildings)
		b->WriteXML(buildingsElement);
	auto tilesElement = doc->NewElement("buildings");

	rootElement->InsertFirstChild(tilesElement);

	for (auto &t : tiles)
		t->WriteXML(tilesElement);
	doc->SaveFile(argv[5]);

	delete doc;

	al_fclose(tileFile);
	al_fclose(bldFile);

	return EXIT_SUCCESS;
}
