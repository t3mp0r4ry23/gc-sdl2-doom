#include <grrlib.h>
#include <ogcsys.h>
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sdcard/gcsd.h>
#include <fat.h>
#include "i_main.h"
int isMounted;

int main(int argc, char **argv) {
	fatInitDefault();	
	return I_Main(argc, argv);
}
