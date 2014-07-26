#include "citymap.h"
#include <sstream>

namespace ApocRes {

CityMap::CityMap(std::string name, int sizeX, int sizeY, int sizeZ)
	: name(name), sizeX(sizeX), sizeY(sizeY), sizeZ(sizeZ)
{
	this->tiles.resize(sizeX);
	for (int x = 0; x < sizeX; x++)
	{
		this->tiles[x].resize(sizeY);
		for (int y = 0; y < sizeY; y++)
		{
			this->tiles[x][y].resize(sizeZ);
		}
	}
}

void
CityMap::WriteXML(tinyxml2::XMLElement *parent)
{
	auto cityElement = parent->GetDocument()->NewElement("city");
	parent->InsertEndChild(cityElement);
	cityElement->SetAttribute("name", this->name.c_str());
	cityElement->SetAttribute("sizeX", this->sizeX);
	cityElement->SetAttribute("sizeY", this->sizeY);
	cityElement->SetAttribute("sizeZ", this->sizeZ);
	for (int x = 0; x < sizeX ;x++)
	{
		for (int y = 0; y < sizeY; y++)
		{
			for (int z = 0; z < sizeZ; z++)
			{
				int tileID = tiles[x][y][z];
				if (tileID == 0)
					continue;
				auto tileElement = parent->GetDocument()->NewElement("tile");
				cityElement->InsertEndChild(tileElement);
				std::stringstream ss;
				ss << "city/citymap:" << tileID;
				tileElement->SetAttribute("x", x);
				tileElement->SetAttribute("y", y);
				tileElement->SetAttribute("z", z);
				tileElement->SetText(ss.str().c_str());

			}
		}
	}
}

CityMap*
CityMap::loadFromFile(std::ifstream &file, std::string name)
{
	CityMap *map = new CityMap(name);
	for (int z = 0; z < map->sizeZ; z++)
	{
		for (int y = 0; y < map->sizeY; y++)
		{
			for (int x = 0; x < map->sizeX; x++)
			{
				uint16_t tileID;
				file.read((char*)&tileID, sizeof(tileID));
				map->tiles[x][y][z] = tileID;
			}
		}
	}
	return map;
}

}; //namespace ApocRes
