#include "utils.h"
#include "input.h"

#include <vitasdk.h>

static SceCtrlData pad, pad_old;
static int touch[2], touch_old[2];
static SceTouchData touch_tmp;

int IN_Init(void)
{
    sceTouchSetSamplingState(SCE_TOUCH_PORT_FRONT, 1);
    sceTouchSetSamplingState(SCE_TOUCH_PORT_BACK, 1);
    sceCtrlPeekBufferPositive(0, &pad, 1);
    return 0;
}

void IN_Update(void)
{
    pad_old = pad;
    touch_old[0] = touch[0];
    touch_old[1] = touch[1];

    sceCtrlPeekBufferPositive(0, &pad, 1);

    for (int i = 0; i < 2; ++i)
    {
        sceTouchPeek(i, &touch_tmp, 1);
        touch[i] = (touch_tmp.reportNum > 0);
    }
}

void IN_Free(void)
{
}

static inline int TranslateButton(int btn)
{
    static const int bmap[16] =
    {
        SCE_CTRL_TRIANGLE,
        SCE_CTRL_CIRCLE,
        SCE_CTRL_CROSS,
        SCE_CTRL_SQUARE,
        SCE_CTRL_LTRIGGER,
        SCE_CTRL_RTRIGGER,
        SCE_CTRL_DOWN,
        SCE_CTRL_LEFT,
        SCE_CTRL_UP,
        SCE_CTRL_RIGHT,
        SCE_CTRL_SELECT,
        SCE_CTRL_START,
    };

   if (btn < 0 || btn > B_START) return -1;
   return bmap[btn];
}

int IN_ButtonPressed(int btn)
{
    if (btn == B_TOUCH1 || btn == B_TOUCH2)
    {
        int i = btn - B_TOUCH1;
        return touch[i] && !touch_old[i];
    }

    btn = TranslateButton(btn);
    return (pad.buttons & btn) && !(pad_old.buttons & btn);
}

int IN_ButtonHeld(int btn)
{
    if (btn == B_TOUCH1 || btn == B_TOUCH2)
    {
        int i = btn - B_TOUCH1;
        return touch[i];
    }

    btn = TranslateButton(btn);
    return (pad.buttons & btn);
}

void IN_WaitForButton(int btn)
{
    do
    {
        IN_Update();
    }
    while (!IN_ButtonPressed(btn));
}

int IN_GetFirstButton(void)
{
    while (1)
    {
        IN_Update();
        for (int b = B_TRIANGLE; b <= B_TOUCH2; ++b)
        {
            if (IN_ButtonPressed(b))
                return b;
        }
    }

    return -1;
}

static const int keymap[B_COUNT] =
{
    56, // B_TRIANGLE | LALT
    14, // B_CIRCLE   | BKSP
    28, // B_CROSS    | RETURN
    57, // B_SQUARE   | SPACE
    54, // B_LTRIGGER | LSHIFT
    29, // B_RTRIGGER | LCTRL
    80, // B_DDOWN    | DOWN
    75, // B_DLEFT    | LEFT
    72, // B_DUP      | UP
    77, // B_DRIGHT   | RIGHT
    83, // B_SELECT   | DELETE
    1,  // B_START    | ESCAPE
    15, // B_TOUCH1   | TAB
    73, // B_TOUCH2   | PGUP
    0,  // B_LSTICK   | -/-
    0,  // B_RSTICK   | -/-
};

int IN_ButtonToKey(int btn)
{
    if (btn < 0 || btn >= B_COUNT) return 0;
    return keymap[btn];
}

int IN_KeyToButton(int key)
{
    if (key <= 0 || key > 255) return -1;
    for (int i = 0; i < B_COUNT; ++i)
        if (keymap[i] == key)
            return i;
    return -1;
}
