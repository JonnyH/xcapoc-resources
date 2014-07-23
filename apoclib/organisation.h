#pragma once

#include <string>
#include <vector>

namespace ApocRes {

class Organisation
{
	public:
		Organisation(std::string name);
		std::string name;
		static std::vector<Organisation> getDefaultOrganisations();
};

}; //namespace ApocRes
