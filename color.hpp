#ifndef __COLOR_HPP
#define __COLOR_HPP

#include <cstdint>

class Color
{
public:
	Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
		: data{r,g,b,a}
	{
	}
	Color() : Color(0,0,0,0){}
	uint8_t data[4];
};

#endif //__COLOR_HPP
