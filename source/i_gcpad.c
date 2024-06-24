//code for handling the gamecube controllers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "i_gcpad.h"

void I_InitGCPad(void) {
	//init pads
	PAD_Init();

	//get pad info
	PAD_ScanPads();
	PAD_Read(gcPads);

	for (int i = 0; i < PAD_CHANMAX; i++) {
		if (!gcPads[i].err) {
			printf("I_InitGCPad: Found gamepad on port %d\n", i + 1);
			gamepad = i;
			break;
		} else {
			continue;
		}
	}
}
