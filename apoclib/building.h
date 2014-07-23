#pragma once

#include <vector>
#include <string>
#include <tinyxml2.h>
#include <allegro5/allegro.h>

#include "rect2d.h"

namespace ApocRes {

class Organisation;

class Building
{
	private:
		Building(Organisation &owner,
			std::string name,
			Rect2d<unsigned short> position);
	public:
		Organisation &owner;
		std::string name;
		Rect2d<unsigned short> position;


		static Building* loadFromFile(ALLEGRO_FILE *file,
			std::vector<Organisation> &organisations,
			std::vector<std::string> &names);
		static std::vector<std::string> getDefaultNames();

};

}; //namespace ApocRes
