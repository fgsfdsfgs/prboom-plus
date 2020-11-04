#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuFiles_Init(void);
void UI_MenuFiles_Update(void);
void UI_MenuFiles_Draw(void);
void UI_MenuFiles_Reload(void);

static struct Option pwads_opts[] =
{
    { OPT_FILE, "Custom file" },
    { OPT_FILE, "Custom file" },
    { OPT_FILE, "Custom file" },
    { OPT_FILE, "Custom file" },
    { OPT_FILE, "Custom file" },
    { OPT_FILE, "Custom file" },
    { OPT_FILE, "Custom file" },
    { OPT_FILE, "Custom file" },
    { OPT_FILE, "Demo" },
    { OPT_FILE, "Override response file" },
    { OPT_BOOLEAN, "Don't load DEHACKED lumps" },
};

struct Menu ui_menu_pwads =
{
    MENU_PWADS,
    "Files",
    "Custom content settings",
    NULL, 0, 0, 0,
    UI_MenuFiles_Init,
    UI_MenuFiles_Update,
    UI_MenuFiles_Draw,
    UI_MenuFiles_Reload,
};

static struct Menu *self = &ui_menu_pwads;

void UI_MenuFiles_Init(void)
{
    UI_MenuFiles_Reload();
}

void UI_MenuFiles_Update(void)
{

}

void UI_MenuFiles_Draw(void)
{

}

void UI_MenuFiles_Reload(void)
{
    for (int i = 0; i < 8; ++i)
    {
        pwads_opts[i].codevar = fs_profiles[ui_profile].files[i];
        pwads_opts[i].file.dir = fs_pwad_dir;
        pwads_opts[i].file.ext[0] = "wad";
        pwads_opts[i].file.ext[1] = "lmp";
        pwads_opts[i].file.ext[2] = "deh";
        pwads_opts[i].file.ext[3] = "bex";
        pwads_opts[i].file.ext[4] =  NULL;
    }

    pwads_opts[8].codevar = fs_profiles[ui_profile].demo;
    pwads_opts[8].file.dir = fs_pwad_dir;
    pwads_opts[8].file.ext[0] = "lmp";
    pwads_opts[8].file.ext[1] = NULL;

    pwads_opts[9].codevar = fs_profiles[ui_profile].rsp;
    pwads_opts[9].file.dir = fs_pwad_dir;
    pwads_opts[9].file.ext[0] = "rsp";
    pwads_opts[9].file.ext[1] = NULL;

    pwads_opts[10].codevar = &fs_profiles[ui_profile].nodeh;

    self->opts = pwads_opts;
    self->numopts = sizeof(pwads_opts) / sizeof(*pwads_opts);
}
