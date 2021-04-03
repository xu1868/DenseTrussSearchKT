#include "Utility.h"

FILE *open_file(const char *file_name, const char *mode) {
	FILE *fin = fopen(file_name, mode);
	if(fin == NULL) {
		printf("Can not open file: %s\n", file_name);
		exit(1);
	}

	return fin;
}

