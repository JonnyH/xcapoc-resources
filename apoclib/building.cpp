#include "organisation.h"
#include "building.h"

#include <cstdint>
#include <iostream>
#include <cassert>

namespace ApocRes {

Building::Building(int id,
	Organisation &owner,
	std::string name,
	std::string type,
	Rect2d<unsigned short> position)
	: id(id), owner(owner), name(name), type(type), position(position)
{
}

//ufo2p.exe 0x14ab46-0x14adcb
static const std::vector<std::string> building_types =
{
	"-",
	"Senate",
	"Police Station",
	"Hospital",
	"School",
	"Rescue Station",
	"Offices",
	"Corporate HQ",
	"Space Port",
	"Sensodrome",
	"Astrodome",
	"Procreation Park",
	"Shopping Mall",
	"Car Park",
	"Apartments",
	"Luxury Apartments",
	"Hotel",
	"Atmosphere Processor",
	"Hydro-Farm",
	"Sewage Works",
	"Water Purifier",
	"Appliances Factory",
	"Arms Factory",
	"Robot Factory",
	"Car Factory",
	"Flyer Factory",
	"Large Flyer Factory",
	"Construction Factory",
	"Slums",
	"Ruins",
	"Warehouse",
	"Space Ship",
	"Power Station",
	"Recyclotorium",
	"Outdoor Parks",
	"People Tubes",
	"Temple of Sirius",
	"X-COM Base",
	"UFOs",
	"Incubator Chamber",
	"Spawning Chamber",
	"Food Chamber",
	"Megapod Chamber",
	"Sleeping Chamber",
	"Organic Factory",
	"Alien Farm",
	"Control Chamber",
	"Maintenance Factory",
	"Dimension Gate Generator",
};
static const std::vector<std::string> _building_names{
	//ufo2p.exe 0x149dd2-0x14ab44
	"Building",
	"The Senate",
	"Judgment Central",
	"Enforcer Academy",
	"Law Control Station",
	"Megastation One",
	"Megastation Two",
	"Phoenix Sanatorium",
	"Nightingale Tower",
	"Iliad Institute",
	"Bosch Institute",
	"Marge Piercy Academy",
	"Rescue One",
	"Rescue Two",
	"Rescue Three",
	"Quadrax Tower",
	"Gygax Memorial Building",
	"Parallax Tower",
	"Tsunami Building",
	"Venus Spires",
	"Raven Reaches",
	"Mahler Building",
	"The Gugarin Institute",
	"Lincoln Tower",
	"The Armageddon Centre",
	"Cyborg Institute",
	"Uhuru Tower",
	"The Karpov Building",
	"Nietzsche Institute",
	"Foucault Tower",
	"Edifice Tower",
	"The Ozone Building",
	"The New Empire Tower",
	"Descartes Towers",
	"Transtellar Spacelines",
	"Galactic Central",
	"Megavision One",
	"Megavision Two",
	"Megavision Three",
	"Megatribe Warriors",
	"Meteor Kings",
	"Dog Star Wanderers",
	"Garden of Delights",
	"The Lineage Foundation",
	"Aldous Huxley Emporium",
	"Hypermart Zone",
	"Acropolis Apartments",
	"Atlantis Apartments",
	"Babylon Apartments",
	"Ptolemy Apartments",
	"Habizone Apartments",
	"Ecozone Apartments",
	"Stellar Apartments",
	"Lone Ranger Apartments",
	"Eden Mansions",
	"Utopia Mansions",
	"Nirvana Mansions",
	"Cyclops Mansions",
	"Cultivator One",
	"Cultivator Two",
	"Cultivator Three",
	"Cityclean One",
	"Cityclean Two",
	"Cityclean Three",
	"Hydrozone One",
	"Hydrozone Two",
	"Hydrozone Three",
	"Appliances One",
	"Appliances Two",
	"Appliances Three",
	"Arms One",
	"Arms Two",
	"Arms Three",
	"Robot One",
	"Robot Two",
	"Robot Three",
	"Car One",
	"Car Two",
	"Car Three",
	"Flyer One",
	"Flyer Two",
	"Flyer Three",
	"Megaflyer One",
	"Megaflyer Two",
	"Megaflyer Three",
	"Construction One",
	"Construction Two",
	"Construction Three",
	"George Orwell Block",
	"Thomas More Tower",
	"Dickens Estate",
	"Oliver Twist Block",
	"Campesino Apartments",
	"Grey Visitor Towers",
	"Scrooge Mansions",
	"Borstal Block",
	"Heavenly Towers",
	"Civic Project",
	"Chronos Block",
	"Necronomicon Mansions",
	"Angel Heart Heights",
	"Lovecraft Block",
	"Bakunin Block",
	"Saturn Block",
	"Hades Block",
	"Neptune Towers",
	"Maze Towers",
	"Grimoire Block",
	"Durruti Block",
	"Blue Doctor Project",
	"Enlightenment Towers",
	"Renaissance Block",
	"Slum City",
	"Warehouse One",
	"Warehouse Two",
	"Warehouse Three",
	"Warehouse Four",
	"Warehouse Five",
	"Warehouse Six",
	"Warehouse Seven",
	"Warehouse Eight",
	"Warehouse Nine",
	"Warehouse Ten",
	"Warehouse Eleven",
	"Warehouse Twelve",
	"Energen Building",
	"Midas Building",
	"Recyclotorium One",
	"Recyclotorium Two",
	"Recyclotorium Three",
	"Temple of the Apocalypse",
	"Temple of the Millenium",
	"Temple of the Visitors",
	"Temple of Humility",
	"Temple of Doom",
	"Temple of Sanity"};

const std::vector<std::string>&
Building::getDefaultNames()
{
	return _building_names;
}

struct bld_file
{
	uint16_t nameIdx;
	uint16_t x0;
	uint16_t x1;
	uint16_t y0;
	uint16_t y1;
	uint16_t zeros1[1];
	uint16_t unknown1;
	uint16_t zeros2[78];
	uint16_t type;
	uint16_t zeros3[5];
	uint16_t unknown3;
	uint16_t zeros4[3];
	uint16_t unknown4;
	uint16_t unknown5;
	uint16_t unknown6;
	uint16_t unknown7;
	uint16_t unknown8;
	uint16_t ownerIdx;
	uint16_t unknown9;
	uint16_t unknown10;
	uint16_t zeros5[10];

};

static_assert(sizeof(bld_file) == 226, "bld_file not 226 bytes");


Building*
Building::loadFromFile(int id,
	std::ifstream &file,
	std::vector<Organisation> &organisations,
	std::vector<std::string> &names)
{
	struct bld_file bld;
	file.read((char*)&bld, sizeof(bld));
	if (!file)
		return nullptr;
	auto name = names[bld.nameIdx];
	auto owner = organisations[bld.ownerIdx];
	std::cout << "Loaded building:\n"
		<< "nameIdx = " << bld.nameIdx << " \"" << name << "\"\n"
		<< "x0 = " << bld.x0 << "\n"
		<< "x1 = " << bld.x1 << "\n"
		<< "y0 = " << bld.y0 << "\n"
		<< "y1 = " << bld.y1 << "\n"
		<< "Owner = " << bld.ownerIdx << " \"" << owner.name << "\"\n"
		<< "Unknown1 = 0x" << std::hex << bld.unknown1 << std::dec << "\n"
		<< "type = 0x" << std::hex << bld.type << std::dec << "\n"
		<< "\t\"" << building_types[bld.type] << "\"\n"
		<< "Unknown3 = 0x" << std::hex << bld.unknown3 << std::dec << "\n"
		<< "Unknown4 = 0x" << std::hex << bld.unknown4 << std::dec << "\n"
		<< "Unknown5 = 0x" << std::hex << bld.unknown5 << std::dec << "\n"
		<< "Unknown6 = 0x" << std::hex << bld.unknown6 << std::dec << "\n"
		<< "Unknown7 = 0x" << std::hex << bld.unknown7 << std::dec << "\n"
		<< "Unknown8 = 0x" << std::hex << bld.unknown8 << std::dec << "\n"
		<< "Unknown9 = 0x" << std::hex << bld.unknown9 << std::dec << "\n"
		<< "Unknown10 = 0x" << std::hex << bld.unknown10 << std::dec << "\n"
		;
	if (bld.zeros1[0])
	{
		std::cout << "zeros1 0x" << std::hex << bld.zeros1[0] << std::dec << "\n";
		std::flush(std::cout);
		assert(0);
	}
	for (int i = 0; i < 78; i++)
	{
		if (bld.zeros2[i])
		{
			std::cout << "zeros2[" << i << "] 0x" << std::hex << bld.zeros2[i] << std::dec << "\n";
			std::flush(std::cout);
			assert(0);
		}
	}
	for (int i = 0; i < 5; i++)
	{
		if (bld.zeros3[i])
		{
			std::cout << "zeros3[" << i << "] 0x" << std::hex << bld.zeros3[i] << std::dec << "\n";
			std::flush(std::cout);
			assert(0);
		}
	}
	for (int i = 0; i < 3; i++)
	{
		if (bld.zeros4[i])
		{
			std::cout << "zeros4[" << i << "] 0x" << std::hex << bld.zeros4[i] << std::dec << "\n";
			std::flush(std::cout);
			assert(0);
		}
	}
	for (int i = 0; i < 10; i++)
	{
		if (bld.zeros5[i])
		{
			std::cout << "zeros5[" << i << "] 0x" << std::hex << bld.zeros5[i] << std::dec << "\n";
			std::flush(std::cout);
			assert(0);
		}
	}
	Rect2d<unsigned short> position = {
		Point2d<unsigned short>{bld.x0, bld.y0},
		Point2d<unsigned short>{bld.x1, bld.y1},
	};

	return new Building(id, owner, name, building_types[bld.type], position);
}

void
Building::WriteXML(tinyxml2::XMLElement *parent)
{
	tinyxml2::XMLElement *element = parent->GetDocument()->NewElement("building");
	parent->InsertEndChild(element);
	element->SetAttribute("name", this->name.c_str());
	element->SetAttribute("owner", this->owner.name.c_str());
	element->SetAttribute("id", this->id);
	element->SetAttribute("type", this->type.c_str());

	tinyxml2::XMLElement *rect = parent->GetDocument()->NewElement("rect");
	element->InsertEndChild(rect);
	rect->SetAttribute("X0", this->position.p0.x);
	rect->SetAttribute("Y0", this->position.p0.y);
	rect->SetAttribute("X1", this->position.p1.x);
	rect->SetAttribute("Y1", this->position.p1.y);
}

}; //namespace ApocRes
