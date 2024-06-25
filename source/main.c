#include <grrlib.h>
#include <ogcsys.h>
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sdcard/gcsd.h>
#include <fat.h>
#include "i_main.h"
static int devices[] = {&__io_gcsda, &__io_gcsdb, &__io_gcsd2};

int main(int argc, char **argv) {
	for (int i = 0; i < sizeof(devices) / sizeof(int); i++) {
		fatUnmount("sd:/");
		int isMounted = fatMountSimple("sd", devices[i]);
		if (isMounted) {
			break;
		}
	}
	
	return I_Main(argc, argv);
}
