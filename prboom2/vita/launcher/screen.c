#include "screen.h"
#include "input.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <vitasdk.h>

static vita2d_pgf *fnt_main;
static vita2d_texture *fnt_buttons;

static int drawing = 0;

int R_Init(void)
{
    sceAppUtilInit(&(SceAppUtilInitParam){}, &(SceAppUtilBootParam){});
    sceCommonDialogSetConfigParam(&(SceCommonDialogConfigParam){});

    vita2d_init();
    vita2d_set_clear_color(C_BLACK);

    fnt_main = vita2d_load_default_pgf();
    fnt_buttons = vita2d_load_PNG_file("app0:/data/launcher/fnt_buttons.png");
    if (!fnt_main || !fnt_buttons)
    {
        R_Free();
        return -1;
    }

    return 0;
}

void R_Free(void)
{
    vita2d_fini();

    if (fnt_main) vita2d_free_pgf(fnt_main);
    if (fnt_buttons) vita2d_free_texture(fnt_buttons);

    fnt_main = NULL;
    fnt_buttons = NULL;
}

void R_BeginDrawing(void)
{
    if (drawing) R_EndDrawing();
    vita2d_start_drawing();
    drawing = 1;
}

void R_EndDrawing(void)
{
    if (drawing)
    {
        vita2d_end_drawing();
        vita2d_common_dialog_update();
        vita2d_wait_rendering_done();
        vita2d_swap_buffers();
        drawing = 0;
    }
}

void R_Clear(unsigned c)
{
    vita2d_set_clear_color(c);
    vita2d_clear_screen();
}

void R_PrintScaled(int flags, int x, int y, float s, unsigned c, const char *fmt, ...)
{
    static char buf[4096];

    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(buf, sizeof(buf), fmt, argptr);
    va_end(argptr);

    // don't care about performance, it's a fucking menu
    if (flags & P_XCENTER)
        x -= vita2d_pgf_text_width(fnt_main, s, buf) / 2;
    else if (flags & P_ARIGHT)
        x -= vita2d_pgf_text_width(fnt_main, s, buf);

    if (flags & P_YCENTER)
        y -= vita2d_pgf_text_height(fnt_main, s, buf) / 2;
    else if (flags & P_ABOTTOM)
        y -= vita2d_pgf_text_height(fnt_main, s, buf);

    vita2d_pgf_draw_text(fnt_main, x, y, c, s, buf);
}

void R_DrawButton(int x, int y, unsigned c, int button)
{
    if (button == B_SELECT)
    {
        vita2d_draw_texture_tint_part(fnt_buttons, x, y, 42, 42, 42, 21, c);
    }
    else if (button == B_START)
    {
        vita2d_draw_texture_tint_part(fnt_buttons, x, y, 0, 63, 42, 21, c);
    }
    else if (button < 16)
    {
        int tx = 21 * (button % 4);
        int ty = 21 * (button / 4);
        vita2d_draw_texture_tint_part(fnt_buttons, x, y, tx, ty, 21, 21, c);
    }
}

void R_DrawRect(int x, int y, int w, int h, unsigned c)
{
    vita2d_draw_rectangle(x, y, w, h, c);
}

void R_DrawFrame(int x1, int y1, int x2, int y2, int w, unsigned c)
{
    vita2d_draw_rectangle(x1, y1, x2 - x1, w, c);
    vita2d_draw_rectangle(x1, y2 - w, x2 - x1, w, c);
    vita2d_draw_rectangle(x1, y1, w, y2 - y1, c);
    vita2d_draw_rectangle(x2 - w, y1, w, y2 - y1, c);
}

void R_DrawLine(int x1, int y1, int x2, int y2, unsigned c)
{
    vita2d_draw_line(x1, y1, x2, y2, c);
}
