#include "organisation.h"

namespace ApocRes {

static const std::vector<Organisation> _orgs = {
	{"X-COM"},
	{"Alien"},
	{"Government"},
	{"Megapol"},
	{"Cult of Sirius"},
	{"Marsec"},
	{"Superdynamics"},
	{"General Metro"},
	{"Cyberweb"},
	{"Transtellar"},
	{"Solmine"},
	{"Sensovision"},
	{"Lifetree"},
	{"Nutrivend"},
	{"Evonet"},
	{"Sanctuary Clinic"},
	{"Nanotech"},
	{"Energen"},
	{"Synthemesh"},
	{"Gravball League"},
	{"Psyke"},
	{"Diablo"},
	{"Osiron"},
	{"S.E.L.F."},
	{"Mutant Alliance"},
	{"Extropians"},
	{"Technocrats"},
	{"Civilian"},
};

const std::vector<Organisation>&
Organisation::getDefaultOrganisations()
{
	return _orgs;
}

Organisation::Organisation(std::string name)
	: name(name)
{
}

}; //namespace ApocRes
