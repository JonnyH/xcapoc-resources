#include <vector>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <cfloat>
#include <cmath>

int main(int argc, char **argv)
{
	int padX = 2;
	int padY = 2;
	if (argc != 5)
	{
		std::cerr << "Usage: font2sdf font.dat height width output.sdf\n";
		return EXIT_FAILURE;
	}
	int height = atoi(argv[2]);
	int width = atoi(argv[3]);
	int outWidth = width + (padX * 2);
	int outHeight = height + (padY * 2);
	std::cerr << "Loading " << argv[1] << " size: " << width << " x " << height << "\n";
	std::ifstream file(argv[1]);
	std::ofstream output(argv[4]);

	std::unique_ptr<char[]> glyphData(new char[height*width]);

	while (file.read(glyphData.get(), height*width) && file)
	{
		for (int y = 0; y < outHeight; y++)
		{
			for (int x = 0; x < outWidth; x++)
			{
				float minDistance = FLT_MAX;
				for (int sx = 0; sx < width; sx++)
				{
					for (int sy = 0; sy < height; sy++)
					{
						if (glyphData[sy*width + sx])
						{
							float dx = abs((sx+padX)-x);
							dx *= dx;
							float dy = abs((sy+padY)-y);
							dy *= dy;
							float d = sqrt(dx+dy);
							if (minDistance > d)
								minDistance = d;
						}
					}
				}
				uint8_t outByte = std::max(0.0f, 255.0f - (minDistance * 100.0f));
				//if (glyphData[y*width + x])
				//	outByte = 0;
				//else
				//	outByte = 255;
				output.write((char*)&outByte, 1);
				output.write((char*)&outByte, 1);
				output.write((char*)&outByte, 1);
			}
		}
	}
}
