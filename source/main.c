#include <ogcsys.h>
#include <gccore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sdcard/gcsd.h>
#include <fat.h>
#include "i_main.h"

static int devices[] = {&__io_gcsda, &__io_gcsdb, &__io_gcsd2};
static void *xfb = NULL;
GXRModeObj *rmode;

int wadCount = 0;
int done = 0;
int cursorLine = 0;
char **customargv;
char wads[32][PATH_MAX];
char *sdName;

int main(int argc, char **argv) {
	VIDEO_Init();
	
	rmode = VIDEO_GetPreferredMode(NULL);
	
	PAD_Init();

	
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
		
	VIDEO_Configure(rmode);
		
	VIDEO_SetNextFramebuffer(xfb);
	
	VIDEO_SetBlack(FALSE);
	VIDEO_Flush();
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();
	
	
	console_init(xfb,64,64,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*2);

	customargv[0] = "";
	customargv[1] = "-i";

	for (int i = 0; i < sizeof(devices) / sizeof(int); i++) {
		fatUnmount("sd:/");
		int isMounted = fatMountSimple("sd", devices[i]);
		if (isMounted) {
			break;
		}
	}

	fatGetVolumeLabel("sd", sdName);
	printf("WADs found on %s:\n\n", sdName);
	printf("\x1b[1B\x1b[999D\x1b[s");

	DIR *list_dir;
	struct dirent *list_item;

	list_dir = opendir("sd:/sdl2-doom/wads");
	if (list_dir != NULL) {
		int i = 0;
		while ((list_item = readdir(list_dir)) && i < 32) {
			list_item = readdir(list_dir);
			strcpy(wads[i], list_item->d_name);
			i++;
		}
		wadCount = i;
	}

	while (!done) {
		PAD_ScanPads();
		for (int line = 0; line < wadCount; line++) {
			printf("%c %s", line == cursorLine ? '>' : ' ', wads[line]);
		}

		if ((PAD_ButtonsDown(0) & PAD_BUTTON_DOWN) || PAD_StickY(0) > 10) {
			if (cursorLine < 31) {
				cursorLine++;
			} else {
				cursorLine = 0;
			}
		}
		if ((PAD_ButtonsDown(0) & PAD_BUTTON_UP) || PAD_StickY(0) < -10) {
			if (cursorLine > 0) {
				cursorLine--;
			} else {
				cursorLine = 31;
			}
		}
		if (PAD_ButtonsDown(0) & PAD_BUTTON_A) {
			strcpy(customargv[2], wads[cursorLine]);
			done = 1;
		}
		VIDEO_WaitVSync();
	}
	xfb = NULL;
	return I_Main(3, customargv);
}
