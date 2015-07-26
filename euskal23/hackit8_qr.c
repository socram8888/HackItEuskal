
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "qrencode-3.4.4/qrspec.h"
#include "qrencode-3.4.4/rscode.h"

const uint8_t RS_MASK[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x0F, 0xFF };
const uint8_t RS_DATA[] = { 0x9E, 0x12, 0x9C, 0x51, 0x89, 0x6E, 0x00, 0x00, 0x81, 0x08, 0xD6, 0x17, 0x80, 0x07, 0x13 };

const uint8_t DATA_PFIX[] = { 0x71, 0xA4, 0x34, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F, 0x2F, 0x77, 0x61, 0x61, 0x2E, 0x61, 0x69, 0x2F, 0x65, 0x34, 0x42, 0x20, 0x41, 0x6E, 0x64, 0x20, 0x6E, 0x6F, 0x77, 0x20, 0x74, 0x68, 0x69, 0x73, 0x3A, 0x20, 0x68, 0x74, 0x74, 0x70, 0x73, 0x3A, 0x2F, 0x2F, 0x77, 0x61, 0x61, 0x2E, 0x61, 0x69, 0x2F };


int countbits(uint32_t val) {
	int set = 0;
	while (val) {
		if (val & 1) {
			set++;
		}
		val = val >> 1;
	}
	return set;
}

void main() {
	int eccSpec[5];
	QRspec_getEccSpec(3, QR_ECLEVEL_L, eccSpec);

	int dl = QRspec_rsDataCodes1(eccSpec);
	int el = QRspec_rsEccCodes1(eccSpec);

	RS * rs = init_rs(8, 0x11d, 0, 1, el, 255 - dl - el);
	
	uint8_t qrData[128];
	memcpy(qrData, DATA_PFIX, sizeof(DATA_PFIX));
	memset(qrData + 51, '0', 4);
	qrData[55] = 0;

	while (true) {
		uint8_t eccData[128];
		encode_rs_char(rs, qrData, eccData);

		size_t i;
		unsigned int diff = 0;
		for (i = 0; i < sizeof(RS_DATA); i++) {
			diff += countbits(RS_DATA[i] ^ (RS_MASK[i] & eccData[i]));
		}

		if (diff < 3) {
			printf("FOUND: %s\n", qrData + 3);
		}

		i = 0;
		while (true) {
			qrData[51 + i]++;
			if (qrData[51 + i] == '9' + 1) {
				qrData[51 + i] = 'A';
			} else if (qrData[51 + i] == 'Z' + 1) {
				qrData[51 + i] = 'a';
			} else if (qrData[51 + i] == 'z' + 1) {
				qrData[51 + i] = '0';
				i++;
				if (i == 4) {
					printf("End of keyspace\n");
					return 0;
				}
			} else {
				break;
			}
		}
	}

	return 0;
}
