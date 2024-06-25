#include <grrlib.h>
#include <ogcsys.h>
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sdcard/gcsd.h>
#include <fat.h>
#include "i_main.h"
#include "font_png.h"

static int devices[] = {&__io_gcsda, &__io_gcsdb, &__io_gcsd2};

int main(int argc, char **argv) {
	GRRLIB_Init();
	PAD_Init();
	int wadNum = 0;
	int done = 0;
	int wadCount = 0;
	char **myargv;
	char wads[32][PATH_MAX];
	myargv[0] = "";
	myargv[1] = "-i";

	//load font
	GRRLIB_texImg *tex_font = GRRLIB_LoadTexture(font_png);
	GRRLIB_InitTileSet(tex_font, 8, 8, 32);

	for (int i = 0; i < sizeof(devices) / sizeof(int); i++) {
		fatUnmount("sd:/");
		int isMounted = fatMountSimple("sd", devices[i]);
		if (isMounted) {
			break;
		}
	}

	char sdMessage[32];
	char sdName[16];
	fatGetVolumeLabel("sd", sdName);
	sprintf(sdMessage, "WADs found on %s:", sdName);

	DIR *list_dir;
	struct dirent *list_item;

	list_dir = opendir("sd:/sdl2-doom/wads");
	/*if (list_dir != NULL) {
		int i = 0;
		while (i < 32) {
			list_item = readdir(list_dir);
			strcpy(wads[i], list_item->d_name);
			i++;
		}
		wadCount = i;
	}*/

	while (!done) {
		PAD_ScanPads();
	
		GRRLIB_SetBackgroundColour(0x1e, 0x1e, 0x2e, 0xFF);
		GRRLIB_Printf(64, 64, tex_font, 0xcdd6f4ff, 2, sdMessage);

		/*for (int y = 0; y < 32; y++) {
			GRRLIB_Printf(64, 128 + y * 32, tex_font, 0xcdd6f4ff, 2, wads[y]);
		}*/
		//selection code goes here

		GRRLIB_Render();
		if (PAD_ButtonsDown(0) & PAD_BUTTON_START) {
			done = 1;
		}
	}

	GRRLIB_Exit();
	
	return I_Main(3, argv);
}
