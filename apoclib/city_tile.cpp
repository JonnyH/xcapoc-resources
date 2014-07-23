#include "city_tile.h"
#include "voxel_layer.h"

#include <cstdint>
#include <sstream>

namespace ApocRes {

struct citymap_dat_file
{
	uint16_t unknown1;
	uint16_t unknown2;
	uint16_t unknown3;
	uint16_t unknown4;
	uint16_t unknown5;
	uint16_t unknown6;
	uint16_t unknown7;
	uint16_t unknown8;
	uint8_t voxelIdx[16];
	uint8_t ground;
	uint8_t connectionY0;
	uint8_t connectionX0;
	uint8_t connectionY1;
	uint8_t connectionX1;
	uint8_t connectionZ0;
	uint8_t connectionZ1;
	uint8_t unknown9[13];

} __attribute__((packed));

static_assert(sizeof(struct citymap_dat_file) == 52, "citymap_dat_file not 52 bytes");

CityMapTile*
CityMapTile::loadFromFile(ALLEGRO_FILE *file, int id, std::vector<VoxelLayer> &lofVoxelLayers)
{
	struct citymap_dat_file dat;
	int ret = al_fread(file, &dat, sizeof(dat));
	if (ret != sizeof(dat))
		return nullptr;
	CityMapTile *tile = new CityMapTile();
	std::stringstream ss;

	ss << "ufodata/city:" << id;

	tile->imageName = ss.str();
	for (int i = 0; i < 16; i++)
		tile->lofVoxels.push_back(lofVoxelLayers[dat.voxelIdx[i]]);
	tile->ground = dat.ground;
	tile->connectionX0 = dat.connectionX0;
	tile->connectionX1 = dat.connectionX1;
	tile->connectionY0 = dat.connectionY0;
	tile->connectionY1 = dat.connectionY1;
	tile->connectionZ0 = dat.connectionZ0;
	tile->connectionZ1 = dat.connectionZ1;
	return tile;
}

void
CityMapTile::WriteXML(tinyxml2::XMLElement *parent)
{
	tinyxml2::XMLElement *element = parent->GetDocument()->NewElement("maptile");
	parent->InsertEndChild(element);

	element->SetAttribute("tile", this->imageName.c_str());
	element->SetAttribute("ground", this->ground);
	element->SetAttribute("connectionx0", this->connectionX0);
	element->SetAttribute("connectionx1", this->connectionX1);
	element->SetAttribute("connectiony0", this->connectionY0);
	element->SetAttribute("connectiony1", this->connectionY1);
	element->SetAttribute("connectionz0", this->connectionZ0);
	element->SetAttribute("connectionz1", this->connectionZ1);

	auto lofelement = parent->GetDocument()->NewElement("lof");
	element->InsertEndChild(lofelement);
	for (int z = 0; z < this->lofVoxels.size(); z++)
	{
		auto voxelPlane = parent->GetDocument()->NewElement("lof_plane");
		lofelement->InsertEndChild(voxelPlane);

		for (int y = 0; y < this->lofVoxels[z].bitmap.size(); y++)
		{
			std::stringstream ss;
			auto voxelRow = parent->GetDocument()->NewElement("lof_row");
			voxelPlane->InsertEndChild(voxelRow);
			for (int x = 0; x < this->lofVoxels[z].bitmap[y].size(); x++)
			{
				ss << (this->lofVoxels[z].bitmap[y][x] ? "1" : "0");
			}
			voxelRow->SetText(ss.str().c_str());
		}
	}
}

}; //namespace ApocRes
