#pragma once

#define MAX_FNAME    256
#define MAX_FILES    8
#define MAX_PROFILES 32

struct FileList
{
    char **files;
    int numfiles;
};

enum NetMode
{
    NETMODE_NONE = 0,
    NETMODE_FAKENET,
    NETMODE_CLIENT,
};

struct Profile
{
    char name[MAX_FNAME];
    char iwad[MAX_FNAME];

    int episodic;

    int present;

    char files[MAX_FILES][MAX_FNAME];
    char demo[MAX_FNAME];
    char rsp[MAX_FNAME];

    char joinaddr[MAX_FNAME];
    int netmode;

    int nodeh;
    int complevel;
    int warp;
    char skill[2];
    char monsters[2];
    int record;
    int logfile;
};

extern struct Profile fs_profiles[MAX_PROFILES];

extern char fs_pwad_dir[MAX_FNAME];
extern char fs_temp_dir[MAX_FNAME];
extern char fs_iwad_dir[MAX_FNAME];

int FS_Init(void);
void FS_Free(void);
char *FS_Error(void);

const char *FS_GetBaseDir(void);

int FS_ProfileAvailable(int profile);

int FS_ListDir(struct FileList *flist, const char *path, const char *ext);
void FS_FreeFileList(struct FileList *flist);

int FS_LoadProfiles(void);
int FS_SaveProfiles(void);

void FS_ExecGame(int profile);
