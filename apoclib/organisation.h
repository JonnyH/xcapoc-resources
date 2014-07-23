#pragma once

#include <string>
#include <vector>

namespace ApocRes {

class Organisation
{
	public:
		Organisation(std::string name);
		std::string name;
		const static std::vector<Organisation>& getDefaultOrganisations();
};

}; //namespace ApocRes
