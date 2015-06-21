#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* If set, reduces the range to 6-bits (divides pcx stored values by 4) */
#define VGA_RANGE

void print_usage(void)
{
	fprintf(stderr, "Usage: pcx2pal input.pcx output.pal\n");
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		print_usage();
		return EXIT_FAILURE;
	}
	FILE *fp = fopen(argv[1], "rb");
	if (!fp)
	{
		fprintf(stderr, "Failed to optn input file: \"%s\"\n", argv[1]);
		return EXIT_FAILURE;
	}
	if (fseek(fp, -768, SEEK_END))
	{
		fprintf(stderr, "Failed to seek to palette section\n");
		return EXIT_FAILURE;
	}
	FILE *pal_out = fopen(argv[2], "wb");
	if (!pal_out)
	{
		fprintf(stderr, "Failed to open output file \"%s\"\n", argv[2]);
		return EXIT_FAILURE;
	}
	int x;
	for (x = 0; x < 256; x++)
	{
		uint8_t b = 0;
		fread(&b, 1, 1, fp);
#ifdef VGA_RANGE
		if (b%4)
			fprintf(stderr, "idx %d: R value %d isn't 6-bit?\n", x, (int)b);
		b /= 4;
#endif
		fwrite(&b, 1, 1, pal_out);
		fread(&b, 1, 1, fp);
#ifdef VGA_RANGE
		if (b%4)
			fprintf(stderr, "idx %d: G value %d isn't 6-bit?\n", x, (int)b);
		b /= 4;
#endif
		fwrite(&b, 1, 1, pal_out);
		fread(&b, 1, 1, fp);
#ifdef VGA_RANGE
		if (b%4)
			fprintf(stderr, "idx %d: B value %d isn't 6-bit?\n", x, (int)b);
		b /= 4;
#endif
		fwrite(&b, 1, 1, pal_out);
	}
}
