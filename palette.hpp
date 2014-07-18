#ifndef __PALETTE_HPP
#define __PALETTE_HPP

#include "color.hpp"

#include <string>

class Palette
{
public:
	Palette(const std::string fileName);
	Palette();
	Color palette[256];
};

#endif //__PALETTE_HPP
