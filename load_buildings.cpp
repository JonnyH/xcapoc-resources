#include "apoclib/building.h"
#include "apoclib/organisation.h"

#include <iostream>
#include <memory>

static void print_usage()
{
	std::cout << "Usage: load_buildings input.bld output.xml\n";
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		print_usage();
		return EXIT_FAILURE;
	}
	ALLEGRO_FILE *file = al_fopen(argv[1], "r");
	if (!file)
	{
		std::cerr << "Failed to open \"" << argv[1] << "\"\n";
		return EXIT_FAILURE;
	}
	std::vector<std::unique_ptr<ApocRes::Building> >buildings;

	auto names =
		ApocRes::Building::getDefaultNames();
	auto orgs =
		ApocRes::Organisation::getDefaultOrganisations();

	auto building = ApocRes::Building::loadFromFile(file, orgs, names);
	while (building)
	{
		buildings.emplace_back(building);
		building = ApocRes::Building::loadFromFile(file, orgs, names);
	}

	std::cout << "Read " << buildings.size() << " buildings\n";


	al_fclose(file);

	return EXIT_SUCCESS;
}
