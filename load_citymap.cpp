#include "apoclib/citymap.h"

#include <iostream>
#include <memory>

static void print_usage()
{
	std::cout << "Usage: load_citymap citymap output.xml\n";
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		print_usage();
		return EXIT_FAILURE;
	}

	std::ifstream citymapFile(argv[1], std::ios_base::in);
	if (!citymapFile)
	{
		std::cerr << "Failed to open \"" << argv[1] << "\"\n";
		return EXIT_FAILURE;
	}

	std::unique_ptr<ApocRes::CityMap> map(ApocRes::CityMap::loadFromFile(citymapFile, std::string(argv[1])));

	tinyxml2::XMLDocument doc;

	auto rootElement = doc.NewElement("openapoc");
	doc.InsertFirstChild(rootElement);

	map->WriteXML(rootElement);

	doc.SaveFile(argv[2]);
}
