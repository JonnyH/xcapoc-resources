#pragma once

#include <vector>
#include <fstream>
#include <tinyxml2.h>

namespace ApocRes {

class CityMap
{
	private:
		CityMap(std::string name, int x = 100, int y = 100, int z = 10);
	public:
		std::vector<std::vector<std::vector<int> > > tiles; //[x][y][z]
		std::string name;
		int sizeX, sizeY, sizeZ;
		static CityMap *loadFromFile(std::ifstream &file, std::string name);

	void WriteXML(tinyxml2::XMLElement *parent);
};

}; //namespace ApocRes
