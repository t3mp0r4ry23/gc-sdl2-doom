#include <ogc/pad.h>

extern PADStatus gcPads[4];
extern int gamepad;
static int buttons[] = {PAD_BUTTON_LEFT, PAD_BUTTON_RIGHT, PAD_BUTTON_DOWN, PAD_BUTTON_UP, PAD_TRIGGER_Z, PAD_TRIGGER_R, PAD_TRIGGER_L, PAD_BUTTON_A, PAD_BUTTON_B, PAD_BUTTON_X, PAD_BUTTON_Y, PAD_BUTTON_START};

void I_InitGCPad(void);
