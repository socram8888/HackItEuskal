// gcc -Wall aribdec.c -o aribdec -laribb24 -I/usr/local/include/ -L/usr/local/lib -lpng && ./aribdec

#include <aribb24/aribb24.h>
#include <aribb24/decoder.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

int main() {
	FILE * f = fopen("decode.bin", "rb");
	unsigned char buf[512];
	size_t insize = fread(buf, 1, sizeof(buf), f);
	fclose(f);

	arib_instance_t * inst = arib_instance_new(NULL);
	arib_decoder_t * dec = arib_get_decoder(inst);
	arib_initialize_decoder(dec);

	char unicode[512];
	size_t decsize = arib_decode_buffer(dec, buf, insize, unicode, sizeof(unicode));
	printf("decsize: %d\n", decsize);

	f = fopen("unicode.bin", "wb");
	fwrite(unicode, 1, decsize, f);
	fclose(f);

	return 0;
}