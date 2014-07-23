#pragma once

#include <vector>
#include <string>
#include <tinyxml2.h>
#include <allegro5/allegro.h>

#include "rect2d.h"
#include "organisation.h"

namespace ApocRes {

class Organisation;

class Building
{
	private:
		Building(int id,
			Organisation &owner,
			std::string name,
			std::string type,
			Rect2d<unsigned short> position);
	public:
		int id;
		Organisation owner;
		std::string name;
		std::string type;
		Rect2d<unsigned short> position;
		void WriteXML(tinyxml2::XMLElement *parent);


		static Building* loadFromFile(int id,
			ALLEGRO_FILE *file,
			std::vector<Organisation> &organisations,
			std::vector<std::string> &names);
		static const std::vector<std::string>& getDefaultNames();

};

}; //namespace ApocRes
