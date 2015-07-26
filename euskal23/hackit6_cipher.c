#include <stdio.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

#define IMGW 311
#define IMGH 47
#define BCOUNT (IMGW * IMGH * 3)

bool testknown(uint8_t * srvb, uint16_t srvkey, uint8_t * clib, uint16_t clikey, size_t pos, size_t end) {
	uint16_t * srvs = (uint16_t *) srvb;
	uint16_t * clis = (uint16_t *) clib;
	pos /= 2;
	end = (end + 1) / 2;

	while (pos < end) {
		uint16_t srvmt = htons(srvs[pos]) - srvkey * (pos + 1);
		uint16_t climt = htons(clis[pos]) - clikey * (pos + 1);

		if (srvmt != climt) {
			//printf("FAILED at %04X %04X %04X\n", pos * 2, srvmt, climt);
			return false;
		}

		pos++;
	}
	return true;
}

void create_same_map(uint8_t * srvb, uint16_t srvkey, uint8_t * clib, uint16_t clikey) {
	uint16_t * srvs = (uint16_t *) srvb;
	uint16_t * clis = (uint16_t *) clib;

	size_t pos;
	int col = 0;
	for (pos = 0; pos < (BCOUNT + 1) / 2; pos++) {
		uint16_t srvmt = htons(srvs[pos]) - srvkey * (pos + 1);
		uint16_t climt = htons(clis[pos]) - clikey * (pos + 1);

		if (srvmt != climt) {
			printf(" ");
		} else {
			printf("#");
		}
		col += 2;
		if (col > IMGW * 3) {
			printf("\n");
			col = 0;
		}
	}
}

int main() {
	uint8_t srvbin[BCOUNT];
	FILE * srvf = fopen("server_img_bytes.bin", "rb");
	assert(fread(srvbin, sizeof(srvbin), 1, srvf));
	fclose(srvf);

	uint8_t clibin[BCOUNT];
	FILE * clif = fopen("client_img_bytes.bin", "rb");
	assert(fread(clibin, sizeof(clibin), 1, clif));
	fclose(clif);
	
	uint32_t srvkey = 0;
	while (srvkey < 65536) {
		uint32_t clikey = 0;
		while (clikey < 65536) {
			#define TK(start, end) testknown(srvbin, srvkey, clibin, clikey, start, end)
			// Estos rangos están siempre a cero en el plaintext ya que son parte del texto fijo que se imprime
			bool valid =
				TK(0x123C, 0x1259) &&
				TK(0x15F0, 0x12FD) &&
				TK(0x3330, 0x333F) &&
				TK(0x3fd0, 0x3fe3) &&
				TK(0X4200, 0x420d);
			#undef TK
			if (valid) {
				printf("Found twin keys: client %d server %d\n", clikey, srvkey);
				create_same_map(srvbin, srvkey, clibin, clikey);
				return 0;
			}

			clikey++;
		}
		srvkey++;
	}
	printf("Unable to find twin keys\n");
	return 1;
}