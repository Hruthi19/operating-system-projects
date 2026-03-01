#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "fs.h"
#include "disk.h"
#include <unistd.h>

int main(int argc, char **argv)
{
		int i;

		if(argc < 2) {
				fprintf(stderr, "usage: ./fs disk_name\n");
				return -1;
		}

		if (access(argv[1], F_OK) != 0) {
				printf("\"%s\" does not exist.\n", argv[1]);
				return 0;
		}
}

