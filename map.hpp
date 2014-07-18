#ifndef __MAP_HPP
#define __MAP_HPP

#include <fstream>
#include <memory>
#include <cstdint>

//FIXME: Best place for tile sizes?
#define FLOOR_TILE_HEIGHT 24
#define FLOOR_TILE_WIDTH 48

//FIXME: Order of floor/left/right/feature guessed
typedef struct
{
	uint8_t floorIdx;
	uint8_t leftIdx;
	uint8_t rightIdx;
	uint8_t featureIdx;
} smp_unit_t;

static_assert(sizeof(smp_unit_t) == 4, "sizeof(smp_unit_t) != 4");

class Map
{
public:
	Map(std::ifstream &inputSDT, std::ifstream &inputSMP);
	unsigned int x;
	unsigned int y;
	unsigned int z;

	static const unsigned int XChunkSize = 24;
	static const unsigned int YChunkSize = 24;
	static const unsigned int ZChunkSize = 5;
	smp_unit_t get(unsigned int x, unsigned int y, unsigned int z);
private:
	std::unique_ptr<smp_unit_t[]> data;
};

#endif //__MAP_HPP
