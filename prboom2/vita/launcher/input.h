#pragma once

// gamepad buttons in SDL order for config stuff and drawing

enum VitaButton
{
    B_TRIANGLE = 0,
    B_CIRCLE,
    B_CROSS,
    B_SQUARE,
    B_LTRIGGER,
    B_RTRIGGER,
    B_DDOWN,
    B_DLEFT,
    B_DUP,
    B_DRIGHT,
    B_SELECT,
    B_START,
    B_TOUCH1,
    B_TOUCH2,

    // dummy buttons for analogs
    B_LSTICK = 14,
    B_RSTICK = 15,

    B_COUNT
};

int IN_Init(void);
void IN_Update(void);
void IN_Free(void);

int IN_ButtonHeld(int btn);
int IN_ButtonPressed(int btn);
void IN_WaitForButton(int btn);
int IN_GetFirstButton(void);

int IN_ButtonToKey(int btn);
int IN_KeyToButton(int key);
