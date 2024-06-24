#include <SDL2/SDL.h>
#include <sdcard/gcsd.h>
#include <fat.h>
#include "i_main.h"

int main(int argc, char **argv) {
	fatMountSimple("sdb", &__io_gcsdb);
	return I_Main(argc, argv);
}
