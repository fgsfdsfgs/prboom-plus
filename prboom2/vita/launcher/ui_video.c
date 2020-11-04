#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuVideo_Init(void);
void UI_MenuVideo_Update(void);
void UI_MenuVideo_Draw(void);
void UI_MenuVideo_Reload(void);

static const char *vidmode_labels[] =
{
    "Software 8-bit", "Software 16-bit", "Software 32-bit",
    "OpenGL",
};

static const char *vidmode_values[] =
{
    "8-bit", "16-bit", "32-bit",
    "OpenGL",
};

static const char *resolutions[] =
{
    "320x200", "320x240", "480x272", "640x368",
    "640x400", "640x480", "720x408", "960x544"
};

static const char *scale_labels[] =
{
    "Keep aspect", "Integer scaling", "Fit to screen",
    "None", "2x", "3x", "4x"
};
static const char *scale_values[] = { "-2", "-1", "0", "1", "2", "3", "4" };

static const char *glsky_labels[] = { "Auto", "None", "Flat", "Dome" };
static const char *glsky_values[] = { "0", "1", "4", "3" };

static const char *multisampling[] = { "0", "2", "4" };

static struct Option video_opts[] =
{
    {
        OPT_CHOICE,
        "Video mode",
        "videomode", NULL,
        .choice =
        {
            vidmode_labels, vidmode_values,
            4, 3,
        },
    },
    {
        OPT_CHOICE,
        "Software resolution",
        "screen_resolution", NULL,
        .choice =
        {
            resolutions, resolutions,
            8, 1,
        },
    },
    {
        OPT_CHOICE,
        "Software scaling",
        "render_screen_multiply", NULL,
        .choice =
        {
            scale_labels, scale_values,
            7, 1,
        },
    },
    {
        OPT_CHOICE,
        "OpenGL multisampling",
        "render_multisampling", NULL,
        .choice =
        {
            multisampling, multisampling,
            3, 0,
        },
    },
    {
        OPT_CHOICE,
        "OpenGL sky mode",
        "gl_skymode", NULL,
        .choice =
        {
            glsky_labels, glsky_values,
            4, 0,
        },
    },
};

struct Menu ui_menu_video =
{
    MENU_VIDEO,
    "Video",
    "Video settings",
    video_opts, sizeof(video_opts) / sizeof(*video_opts), 0, 0,
    UI_MenuVideo_Init,
    UI_MenuVideo_Update,
    UI_MenuVideo_Draw,
};

static struct Menu *self = &ui_menu_video;

void UI_MenuVideo_Init(void)
{

}

void UI_MenuVideo_Update(void)
{

}

void UI_MenuVideo_Draw(void)
{

}
