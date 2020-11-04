#pragma once

#define CONFIG_FILENAME "prboom-plus.cfg"

enum ConfigVarTypes
{
    CVAR_UNKNOWN,
    CVAR_INTEGER,
    CVAR_DOUBLE,
    CVAR_STRING,
};

void CFG_Free(void);
int CFG_Load(void);
int CFG_Save(void);

int CFG_ReadVar(int profile, const char *name, void *dst);
int CFG_WriteVar(int profile, const char *name, void *src);
int CFG_VarType(int profile, const char *name);
