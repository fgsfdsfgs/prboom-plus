#pragma once

#include <stdlib.h>
#include <string.h>

#define MAX_CHOICES 8
#define MAX_STROPT 128

enum Menus
{
    MENU_MAIN,
    MENU_VIDEO,
    MENU_AUDIO,
    MENU_INPUT,
    MENU_PWADS,
    MENU_MISC,
    MENU_NET,

    MENU_COUNT
};

enum OptionTypes
{
    OPT_BOOLEAN,
    OPT_CHOICE,
    OPT_INTEGER,
    OPT_DOUBLE,
    OPT_STRING,
    OPT_BUTTON,
    OPT_CALLBACK,
    OPT_FILE,
};

struct Option
{
    int type;
    const char *name;
    const char *cfgvar;
    void *codevar;

    union
    {
        int boolean;

        struct
        {
            const char **names;
            const char **values;
            int count;
            int val;
        } choice;

        struct
        {
            double min;
            double max;
            double step;
            double val;
        } dnum;

        struct
        {
            int min;
            int max;
            int step;
            int val;
        } inum;

        char string[MAX_STROPT];

        int button;

        void (*cb)(int);

        struct
        {
            const char *dir;
            const char *ext[6];
            char val[MAX_STROPT];
        } file;
    };
};

struct Menu
{
    int id;
    const char *tabname;
    const char *title;

    struct Option *opts;
    int numopts;
    int sel;
    int scroll;

    void (*init)(void);
    void (*update)(void);
    void (*draw)(void);
    void (*reload)(void);
};

extern int ui_current_menu;
extern int ui_profile;

int UI_Init(void);
int UI_Update(void);
void UI_Draw(void);
void UI_Redraw(void);
void UI_Free(void);

void UI_ReloadOptions(void);
void UI_SaveOptions(void);
