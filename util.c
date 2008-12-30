#include "util.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>


FILE* dex_fopen(char* name, char* mode) {
	FILE* fo;
	if(!strcmp("-", name)) {
		if(!strcmp("w", mode)) {
			fo = stdout;
		} else {
			fo = stdin;
		}
	} else {
		fo = fopen(name, mode);
	}
	if(fo == NULL) {		
    fprintf(stderr, "Cannot open file %s, error %d, %s\n", name, errno, strerror(errno));
		exit(1);
	}
	return fo;	
}
