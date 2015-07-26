#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

uint32_t calculate(const char * keyptr, size_t * endpos) {
	uint32_t result = 0;
	uint32_t value = 0;

	size_t i;
	for (i = 0; i < 17; i++) {
		uint32_t x = value ^ (keyptr[i] - '0');
		value = keyptr[i] - '0';

		switch (x) {
			case 1:
			case 2:
				result += (value & 3);
				break;
			case 4:
			case 8:
				switch (value & 0x0C) {
					case 0:
						result *= 2;
						break;
					case 4:
						result -= 4;
						break;
					case 8:
						result += 5;
						break;
					case 12:
						result -= value & 3;
						break;
				}
				break;
			case 16:
				switch (value & 0x10) {
					case 0:
						result = 0;
						break;
					case 16:
						result = 1;
						break;
				}
				break;
			default:
				goto ret;
		}
	}

ret:
	*endpos = i;
	return result;
}

int main() {
	char str[18];
	memset(str, '0', 17);
	str[17] = 0;

	while (true) {
		size_t endpos;

		if (calculate(str, &endpos) == 0x27) {
			printf("Found key: %s\n", str);
		}

		if (endpos > 16) {
			endpos = 16;
		}

		while (true) {
			str[endpos]++;
			if (str[endpos] <= 'O') {
				break;
			}

			if (endpos == 0) {
				printf("keyspace analized\n");
				return 0;
			}

			str[endpos] = '0';
			endpos--;
		}
	}
}