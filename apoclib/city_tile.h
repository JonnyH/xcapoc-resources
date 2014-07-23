#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <tinyxml2.h>
#include <allegro5/allegro.h>

#include "voxel_layer.h"

namespace ApocRes {


class CityMapTile {
private:
public:
	std::string imageName;
	bool ground;
	bool connectionX0;
	bool connectionX1;
	bool connectionY0;
	bool connectionY1;
	bool connectionZ0;
	bool connectionZ1;
	std::vector<VoxelLayer> lofVoxels;

	void WriteXML(tinyxml2::XMLElement *parent);
	static CityMapTile* loadFromFile(ALLEGRO_FILE *file,
		int id,
		std::vector<VoxelLayer> &lofVoxelLayers);

};

}; //namespace ApocRes
