#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "ui.h"

void UI_MenuMisc_Init(void);
void UI_MenuMisc_Update(void);
void UI_MenuMisc_Draw(void);
void UI_MenuMisc_Reload(void);

static const char *mon_labels[] = 
{
    "Default",
    "Disabled",
    "Fast",
    "Respawning",
    "Fast and Respawning",
};

static const char *mon_values[] = 
{
    "0", "1", "2", "4", "6",
};

static const char *skill_labels[] =
{
    "Default",
    "I'm too young to die",
    "Hey, not too rough",
    "Hurt me plenty",
    "Ultra Violence",
    "Nightmare!"
};

static const char *skill_values[] =
{
    "0", "1", "2", "3", "4", "5"
};

static struct Option misc_opts[] =
{
    { OPT_CHOICE, "Monsters", .choice = { mon_labels, mon_values, 5, 0 } },
    { OPT_BOOLEAN, "Record demo" },
    { OPT_CHOICE, "Skill", .choice = { skill_labels, skill_values, 6, 0 } },
    { OPT_INTEGER, "Starting map", .inum = { 0, 99, 1, 0 } },
    { OPT_INTEGER, "Compatibility level", .inum = { -1, 17, 1, -1 } },
    { OPT_BOOLEAN, "Debug logging" },
};

struct Menu ui_menu_misc =
{
    MENU_PWADS,
    "Misc",
    "Other game settings",
    NULL, 0, 0, 0,
    UI_MenuMisc_Init,
    UI_MenuMisc_Update,
    UI_MenuMisc_Draw,
    UI_MenuMisc_Reload,
};

static struct Menu *self = &ui_menu_misc;

void UI_MenuMisc_Init(void)
{
    UI_MenuMisc_Reload();
}

void UI_MenuMisc_Update(void)
{

}

void UI_MenuMisc_Draw(void)
{

}

void UI_MenuMisc_Reload(void)
{
    misc_opts[0].codevar = fs_profiles[ui_profile].monsters;
    misc_opts[1].codevar = &fs_profiles[ui_profile].record;
    misc_opts[2].codevar = &fs_profiles[ui_profile].skill;
    misc_opts[3].codevar = &fs_profiles[ui_profile].warp;
    misc_opts[4].codevar = &fs_profiles[ui_profile].complevel;
    misc_opts[5].codevar = &fs_profiles[ui_profile].logfile;

    self->opts = misc_opts;
    self->numopts = sizeof(misc_opts) / sizeof(*misc_opts);
}
