#include "organisation.h"

namespace ApocRes {

std::vector<Organisation>
Organisation::getDefaultOrganisations()
{
	std::vector<Organisation> orgs;
	std::vector<std::string> names{
		//ufo2p.exe 0x14af10-14b02e
		"X-COM",
		"Alien",
		"Government",
		"Megapol",
		"Cult of Sirius",
		"Marsec",
		"Superdynamics",
		"General Metro",
		"Cyberweb",
		"Transtellar",
		"Solmine",
		"Sensovision",
		"Lifetree",
		"Nutrivend",
		"Evonet",
		"Sanctuary Clinic",
		"Nanotech",
		"Energen",
		"Synthemesh",
		"Gravball League",
		"Psyke",
		"Diablo",
		"Osiron",
		"S.E.L.F.",
		"Mutant Alliance",
		"Extropians",
		"Technocrats",
		"Civilian",
	};

	for (auto name : names)
	{
		orgs.emplace_back(name);
	}
	return orgs;
}

Organisation::Organisation(std::string name)
	: name(name)
{
}

}; //namespace ApocRes
