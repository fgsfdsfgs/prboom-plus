#include "utils.h"
#include "files.h"
#include "input.h"
#include "screen.h"
#include "configs.h"
#include "ui.h"

void UI_MenuNet_Init(void);
void UI_MenuNet_Update(void);
void UI_MenuNet_Draw(void);
void UI_MenuNet_Reload(void);

static void DoJoinNet(int);

static struct Option net_opts[] =
{
    { OPT_STRING, "Game address" },
    { OPT_CALLBACK, "Connect to address", .cb = DoJoinNet },
};

struct Menu ui_menu_net =
{
    MENU_PWADS,
    "Net",
    "Network game settings",
    NULL, 0, 0, 0,
    UI_MenuNet_Init,
    UI_MenuNet_Update,
    UI_MenuNet_Draw,
    UI_MenuNet_Reload,
};

static struct Menu *self = &ui_menu_net;

void UI_MenuNet_Init(void)
{
    UI_MenuNet_Reload();
    // HACK: dynamic vars don't get loaded properly for Doom because it's
    //       selected on start, so got to do this stupid shit
    strncpy(net_opts[0].string, fs_profiles[ui_profile].joinaddr, MAX_STROPT);
}

void UI_MenuNet_Update(void)
{

}

void UI_MenuNet_Draw(void)
{

}

void UI_MenuNet_Reload(void)
{
    net_opts[0].codevar = fs_profiles[ui_profile].joinaddr;

    self->opts = net_opts;
    self->numopts = sizeof(net_opts) / sizeof(*net_opts);
}

static void DoJoinNet(int arg)
{
    fs_profiles[ui_profile].netmode = NETMODE_CLIENT;
    UI_SaveOptions();
    CFG_Save();
    FS_SaveProfiles();
    FS_ExecGame(ui_profile);
}
