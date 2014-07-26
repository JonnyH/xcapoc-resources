#pragma once

#include "rect2d.h"
#include "organisation.h"

#include <vector>
#include <string>
#include <fstream>
#include <tinyxml2.h>

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
			std::ifstream &file,
			std::vector<Organisation> &organisations,
			std::vector<std::string> &names);
		static const std::vector<std::string>& getDefaultNames();

};

}; //namespace ApocRes
