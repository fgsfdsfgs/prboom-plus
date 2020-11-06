#include "utils.h"
#include "files.h"
#include "configs.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_CVARLEN 128
#define MAX_CVARNAME 128

#define CVARF_HEX 0x01

// TODO: actually store types for known cvars somewhere and check them when
//       loading; maybe use m_config.c for this

struct ConfigVar
{
    int type;
    char name[MAX_CVARLEN];
    int flags;
    union
    {
        char sval[MAX_CVARLEN];
        int ival;
        double dval;
    };
};

struct Config
{
    int modified;
    const char *name;
    struct ConfigVar *cvars;
    int numcvars;
};

static struct Config cfg_main = { 0, CONFIG_FILENAME };

static int ConfigLoad(struct Config *cfg)
{
    char buf[512];
    char key[MAX_CVARNAME], value[MAX_CVARLEN];
    snprintf(buf, sizeof(buf), "%s/%s", FS_GetBaseDir(), cfg->name);

    FILE *f = fopen(buf, "r");
    if (!f) return 1;

    while (!feof(f))
    {
        if (fscanf(f, "%79s %127[^\n]\n", key, value) != 2)
            continue;

        if (key[0] == '#')
            continue;

        cfg->cvars = realloc(cfg->cvars, sizeof(struct ConfigVar) * (cfg->numcvars + 1));
        if (!cfg->cvars) { fclose(f); return 1; }

        struct ConfigVar *cvar = cfg->cvars + cfg->numcvars++;

        while (strlen(value) > 0 && !isprint(value[strlen(value)-1]))
            value[strlen(value)-1] = '\0';

        strncpy(cvar->name, key, MAX_CVARNAME);
        cvar->flags = 0;

        if (value[0] == '"' && value[1])
        {
            cvar->type = CVAR_STRING;
            value[strlen(value)-1] = '\0';
            strncpy(cvar->sval, value + 1, MAX_CVARLEN);
        }
        else if (strchr(value, '.'))
        {
            cvar->type = CVAR_DOUBLE;
            sscanf(value, "%lf", &cvar->dval);
        }
        else
        {
            cvar->type = CVAR_INTEGER;
            if (value[0] == '0' && value[1] == 'x')
            {
                sscanf(value + 2, "%x", &cvar->ival);
                cvar->flags |= CVARF_HEX;
            }
            else
            {
                sscanf(value, "%d", &cvar->ival);
            }
        }
    }

    cfg->modified = 0;
    fclose(f);
    return 0;
}

static int ConfigSave(struct Config *cfg)
{
    if (!cfg->modified) return 0;

    static char buf[512];
    snprintf(buf, sizeof(buf), "%s/%s", FS_GetBaseDir(), cfg->name);

    FILE *f = fopen(buf, "w");
    if (!f) return 1;

    for (int i = 0; i < cfg->numcvars; ++i)
    {
        struct ConfigVar *cvar = cfg->cvars + i;

        int wc = fprintf(f, "%s ", cvar->name);
        for (; wc < 30; ++wc) fprintf(f, " ");

        if (cvar->type == CVAR_STRING)
        {
            fprintf(f, "\"%s\"", cvar->sval);
        }
        else if (cvar->type == CVAR_DOUBLE)
        {
            fprintf(f, "%f", (float)cvar->dval);
        }
        else if (cvar->type == CVAR_INTEGER)
        {
            if (cvar->flags & CVARF_HEX)
                fprintf(f, "0x%x", (unsigned)cvar->ival);
            else
                fprintf(f, "%d", cvar->ival);
        }

        fprintf(f, "\n");
    }

    cfg->modified = 0;
    fclose(f);
    return 0;
}

int CFG_Load(void)
{
    return ConfigLoad(&cfg_main);
}

int CFG_Save(void)
{
    return ConfigSave(&cfg_main);
}

void CFG_Free(void)
{
    if (cfg_main.numcvars) free(cfg_main.cvars);
    cfg_main.cvars = NULL;
    cfg_main.numcvars = 0;
}

static struct ConfigVar *FindVar(int profile, const char *name)
{
    struct Config *cfg = &cfg_main;

    for (int i = 0; i < cfg->numcvars; ++i)
    {
        if (!strcmp(cfg->cvars[i].name, name))
            return cfg->cvars + i;
    }

    return NULL;
}

int CFG_ReadVar(int profile, const char *name, void *dst)
{
    struct ConfigVar *cvar = FindVar(profile, name);

    if (!cvar) return 1;

    switch (cvar->type)
    {
        case CVAR_INTEGER:
            *((int *)dst) = cvar->ival;
            break;
        case CVAR_DOUBLE:
            *((double *)dst) = cvar->dval;
            break;
        case CVAR_STRING:
            strncpy((char *)dst, cvar->sval, MAX_CVARLEN);
            break;
        default:
            return 2;
    }

    return 0;
}

int CFG_WriteVar(int profile, const char *name, void *src)
{
    struct ConfigVar *cvar = FindVar(profile, name);

    if (!cvar) return 1;

    int modified = 0;

    switch (cvar->type)
    {
        case CVAR_INTEGER:
            modified = cvar->ival != *((int *)src);
            cvar->ival = *((int *)src);
            break;
        case CVAR_DOUBLE:
            modified = cvar->dval != *((double *)src);
            cvar->dval = *((double *)src);
            break;
        case CVAR_STRING:
            modified = strcmp(cvar->sval, (char *)src);
            strncpy(cvar->sval, (char *)src, MAX_CVARLEN);
            break;
        default:
            return 2;
    }

    if (modified)
        cfg_main.modified = 1;

    return 0;
}

int CFG_VarType(int profile, const char *name)
{
    if (!name) return CVAR_UNKNOWN;
    struct ConfigVar *cvar = FindVar(profile, name);
    if (!cvar) return CVAR_UNKNOWN;
    return cvar->type;
}
