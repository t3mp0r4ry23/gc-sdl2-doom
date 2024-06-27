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
static GXRModeObj *rmode = NULL;

int iwadsCount = 0;
int pwadsCount = 0;
int selectedPWADsCount = 0;
int done = 0;
int cursorLine = 0;
int customargc = 0;
char **customargv;

int fileCount(char *path) {
	int count = 0;
	DIR *count_dir;
	struct dirent *count_ent;

	count_dir = opendir(path);
	if (count_dir != NULL) {
		while (count_ent = readdir(count_dir)) {
			if (count_ent->d_type == DT_REG) {
				count++;
			}
		}
		closedir(count_dir);
	} else {
		return 0;
	}

	return count;
}

void getFileList(char *path, char **file_list) {
	DIR *list_dir;
	struct dirent *list_item;
	int list_entry = 0;

	list_dir = opendir(path);
	if (list_dir != NULL) {
		while (list_item = readdir(list_dir)) {
			if (list_item->d_type == DT_REG) {
				strcpy(file_list[list_entry], list_item->d_name);
			}
		}
		closedir(list_dir);
	}
}

void menuInit() {
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
	
	
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
}

int sdInit() {
	for (int i = 0; i < sizeof(devices) / sizeof(int); i++) {
		fatUnmount("sd:/");
		int isMounted = fatMountSimple("sd", devices[i]);
		if (isMounted) {
			return 0;
		}
	}
	return 1;
}

int main(int argc, char **argv) {
	menuInit();
	if (sdInit()) {
		printf("\nERROR:\nCould not mount SD Card.\n\nPress A to exit.\n");
		while (1) {
			PAD_ScanPads();
			if (PAD_ButtonsDown(0) & PAD_BUTTON_A)
				return 1;
		}
	}

	char **iwads;
	iwadsCount = fileCount("sd:/sdl2-doom/iwads");
	iwads = malloc(sizeof(char*) * iwadsCount);
	for (int iwadFile = 0; iwadFile < iwadsCount; iwadFile++)
		iwads[iwadFile] = malloc(PATH_MAX + 1);
	getFileList("sd:/sdl2-doom/iwads", iwads);
	int selectedIWAD = 0;
	char **pwads;
	pwadsCount = fileCount("sd:/sdl2-doom/pwads");
	pwads = malloc(sizeof(char*) * pwadsCount);
	for (int pwadFile = 0; pwadFile < pwadsCount; pwadFile++)
		pwads[pwadFile] = malloc(PATH_MAX + 1);
	getFileList("sd:/sdl2-doom/pwads", pwads);
	int *selectedPWADs;
	selectedPWADs = malloc(sizeof(int) * pwadsCount);

	if (!iwadsCount) {
		printf("\nERROR:\nNo IWADs found.\nPlease place one or more IWADS in a folder at [SD ROOT]/sdl2-doom/iwads/\n\nPress A to exit.\n");
		while (1) {
			PAD_ScanPads();
			if (PAD_ButtonsDown(0) & PAD_BUTTON_A)
				return 1;
		}
	}

	printf("\nPress A to select an IWAD\n\n");
	printf("\x1b[1B\x1b[999D\x1b[s");
	printf("pause for debug\n");
	printf("iwadsCount is %d\n", iwadsCount);
	sleep(5);
	printf("iwads[0] is %s\n", iwads[0]);
	printf("iwads[1] is %s\n", iwads[1]);
	sleep(5);
	printf("strlen(iwads[0]) is %d\n", strlen(iwads[0]));
	sleep(5);
	for (int iterator = 0; iterator < 5; iterator++)
		printf("here's a printf with conditionals and escape codes inside a for loop %c%stest\x1b[40;0m\x1b[37;0m %s\n", iterator == 3 ? '0' : ' ', 1 ? "\x1b[47;0m\x1b[30;0m" : "", iwads[1]);
	sleep(5);
	while (!done) {
		PAD_ScanPads();
		for (int line = 0; line < iwadsCount; line++) {
			printf("%c %s%s\x1b[40;0m\x1b[37;0m\n", line == cursorLine ? '>' : ' ', line == cursorLine ? "\x1b[47;0m\x1b[30;0m" : "", iwads[line]);
		}

		if ((PAD_ButtonsDown(0) & PAD_BUTTON_DOWN) || PAD_StickY(0) > 10) {
			if (cursorLine < iwadsCount - 1) {
				cursorLine++;
			} else {
				cursorLine = 0;
			}
		}
		if ((PAD_ButtonsDown(0) & PAD_BUTTON_UP) || PAD_StickY(0) < -10) {
			if (cursorLine > 0) {
				cursorLine--;
			} else {
				cursorLine = iwadsCount - 1;
			}
		}
		if (PAD_ButtonsDown(0) & PAD_BUTTON_A) {
			selectedIWAD = cursorLine;
			done = 1;
		}
		VIDEO_WaitVSync();
		printf("\x1b[u\x1b[0J");
	}

	if (pwadsCount) {
		printf("\x1b[2J");
		done = 0;
		cursorLine = 0;

		printf("\nPress A to select PWADs. Press START to finish selection.\n\n");
		printf("\x1b[1B\x1b[999D\x1b[s");
		while (!done) {
			PAD_ScanPads();
			for (int line = 0; line < pwadsCount; line++) {
				printf("%c %s%s%s\x1b[40;0m\x1b[37;0m\n", line == cursorLine ? '>' : ' ', selectedPWADs[line] ? "\x1b[42;0m" : "", line == cursorLine ? "\x1b[47;0m\x1b[30;0m" : "  ", pwads[line]);
			}

			if ((PAD_ButtonsDown(0) & PAD_BUTTON_DOWN) || PAD_StickY(0) > 10) {
				if (cursorLine < pwadsCount - 1) {
					cursorLine++;
				} else {
					cursorLine = 0;
				}
			}
			if ((PAD_ButtonsDown(0) & PAD_BUTTON_UP) || PAD_StickY(0) < -10) {
				if (cursorLine > 0) {
					cursorLine--;
				} else {
					cursorLine = pwadsCount -1;
				}
			}
			if (PAD_ButtonsDown(0) & PAD_BUTTON_START) {
				done = 1;
			}
			if (PAD_ButtonsDown(0) & PAD_BUTTON_A) {
				if (selectedPWADs[cursorLine]) {
					selectedPWADs[cursorLine] = 0;
					selectedPWADsCount--;
				} else {
					selectedPWADs[cursorLine] = 1;
					selectedPWADsCount++;
				}
			}
			VIDEO_WaitVSync();
			printf("\x1b[u\x1b[0J");
		}
	}

	customargv = malloc(sizeof(char*) * (3 + selectedPWADsCount + (selectedPWADsCount ? 1 : 0)));
	customargv[0] = '\0';
	strcpy(customargv[1], "-iwad");
	sprintf(customargv[2], "%s/%s", "sd:/sdl2-doom/iwads", iwads[selectedIWAD]);
	for (int iwadFile = 0; iwadFile < iwadsCount; iwadFile++)
		free(iwads[iwadFile]);
	free(iwads);
	if (selectedPWADsCount) {
		customargc++;
		strcpy(customargv[3], "-file");
		for (int pwad = 0; pwad < pwadsCount; pwad++) {
			if (selectedPWADs[pwad]) {
				sprintf(customargv[customargc], "%s/%s", "sd:/sdl2-doom/pwads", pwads[pwad]);
				customargc++;
			}
		}
	}
	for (int pwadFile = 0; pwadFile < pwadsCount; pwadFile++)
		free(pwads[pwadFile]);
	free(pwads);

	return I_Main(customargc, customargv);
}
