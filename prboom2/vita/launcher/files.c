#include "utils.h"
#include "files.h"
#include "net.h"

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <vitasdk.h>

#define mkdir(x, y) sceIoMkdir(x, y)

char fs_base_dir[MAX_FNAME];
char fs_pwad_dir[MAX_FNAME];
char fs_temp_dir[MAX_FNAME];
char fs_iwad_dir[MAX_FNAME];

static char fs_error[4096];

// default profiles
struct Profile fs_profiles[MAX_PROFILES] =
{
    // doom1-based
    { "Doom (Shareware)", "doom1.wad", 1 },
    { "Doom", "doom.wad", 1 },
    { "FreeDM", "freedm.wad", 1 },
    { "FreeDoom: Phase 1", "freedoom1.wad", 1 },
    { "Chex Quest", "chex.wad", 1 },
    // doom2-based
    { "Doom II", "doom2.wad", 0 },
    { "Final Doom: TNT Evilution", "tnt.wad", 0 },
    { "Final Doom: The Plutonia Experiment", "plutonia.wad", 0 },
    { "FreeDoom: Phase 2", "freedoom2.wad", 0 },
    { "Hacx", "hacx.wad", 0 },
};

static void SetError(const char *fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    vsnprintf(fs_error, sizeof(fs_error), fmt, argptr);
    va_end(argptr);
}

static int CheckForProfile(int g)
{
    char buf[MAX_FNAME];
    snprintf(buf, sizeof(buf), "%s/%s", fs_iwad_dir, fs_profiles[g].iwad);
    return fexists(buf);
}

const char *FS_GetBaseDir(void)
{
    const char *drives[] = { "uma0", "imc0", "ux0" };
    const char *path = "/data/prboom";

    if (fs_base_dir[0] == 0)
    {
        // check if a prboom folder exists on one of the drives
        // default to the last one (ux0)
        for (unsigned int i = 0; i < sizeof(drives) / sizeof(*drives); ++i)
        {
          snprintf(fs_base_dir, sizeof(fs_base_dir), "%s:%s", drives[i], path);
          DIR *dir = opendir(fs_base_dir);
          if (dir)
          {
            closedir(dir);
            break;
          }
        }
    }

    return fs_base_dir;
}

int FS_Init(void)
{
    snprintf(fs_pwad_dir, sizeof(fs_pwad_dir), "%s/pwads", FS_GetBaseDir());
    snprintf(fs_iwad_dir, sizeof(fs_iwad_dir), "%s/iwads", FS_GetBaseDir());
    snprintf(fs_temp_dir, sizeof(fs_temp_dir), "%s/tmp", FS_GetBaseDir());

    mkdir(FS_GetBaseDir(), 0755);
    mkdir(fs_pwad_dir, 0755);
    mkdir(fs_temp_dir, 0755);

    int numgames = 0;
    for (int i = 0; i < MAX_PROFILES; ++i)
    {
        if (fs_profiles[i].iwad == NULL) break;
        int present = CheckForProfile(i);
        fs_profiles[i].present = present;
        fs_profiles[i].monsters[0] = '0';
        fs_profiles[i].complevel = -1;
        snprintf(fs_profiles[i].joinaddr, MAX_FNAME, "%s:5030", net_my_ip);
        numgames += present;
    }

    if (!numgames)
    {
        SetError("No supported IWADs were found in the `iwads` directory.\n" \
                 "Make sure you have installed at least one game properly.");
        return -1;
    }

    return 0;
}

void FS_Free(void)
{

} 

char *FS_Error(void)
{
    return fs_error;
}

int FS_ProfileAvailable(int profile)
{
    if (profile < 0 || profile >= MAX_PROFILES) return 0;
    return fs_profiles[profile].present;
}

int FS_ListDir(struct FileList *flist, const char *path, const char *ext)
{
    static char buf[512];

    DIR *dp = opendir(path);
    if (!dp) return -1;

    struct dirent *ep;
    while (ep = readdir(dp))
    {
        if (!ep->d_name || ep->d_name[0] == '.')
            continue;

        char *fname = ep->d_name;
        snprintf(buf, sizeof(buf), "%s/%s", path, fname);

        if (!isdir(buf) && !strcasecmp(fext(fname), ext))
        {
            flist->files = realloc(flist->files, (flist->numfiles + 1) * sizeof(char*));
            if (!flist->files) return -2;
            flist->files[flist->numfiles] = strdup(fname);
            flist->numfiles++;
        }
    }

    closedir(dp);

    return flist->numfiles;
}

void FS_FreeFileList(struct FileList *flist)
{
    for (int i = 0; i < flist->numfiles; ++i)
      free(flist->files[i]);

    free(flist->files);
    flist->files = NULL;
    flist->numfiles = 0;
}

static void WriteResponseFile(int profile, const char *fname)
{
    struct Profile *g = fs_profiles + profile;

    FILE *f = fopen(fname, "w");
    if (!f) I_Error("Could not create file\n%s", fname);

    if (g->netmode)
    {
        if (g->netmode == NETMODE_CLIENT)
            fprintf(f, "-net %s\n", g->joinaddr);
        else
            fprintf(f, "-solo-net\n");
    }

    fprintf(f, "-iwad %s\n", g->iwad);

    const char *pwads[MAX_FILES];
    int numpwads = 0;
    const char *dehs[MAX_FILES];
    int numdehs = 0;

    for (int i = 0; i < MAX_FILES; ++i)
    {
        if (!g->files[i][0]) continue;
        if (!fextcmp(g->files[i], "deh") || !fextcmp(g->files[i], "bex"))
            dehs[numdehs++] = g->files[i];
        else
            pwads[numpwads++] = g->files[i];
    }

    if (numpwads)
    {
        fprintf(f, "-file");
        for (int i = 0; i < numpwads; ++i)
            fprintf(f, " %s", pwads[i]);
        fprintf(f, "\n");
    }

    if (numdehs)
    {
        fprintf(f, "-deh");
        for (int i = 0; i < numdehs; ++i)
            fprintf(f, " %s", dehs[i]);
        fprintf(f, "\n");
    }

    if (g->nodeh)
        fprintf(f, "-nodeh\n");

    if (g->skill)
        fprintf(f, "-skill %d\n", g->skill);

    if (g->complevel >= 0)
        fprintf(f, "-complevel %d\n", g->complevel);

    if (g->warp)
    {
        if (g->episodic)
        {
            int ep = g->warp / 10;
            int map = g->warp % 10;
            if (!ep) ep = 1;
            if (!map) map = 1;
            fprintf(f, "-warp %d %d\n", ep, map);
        }
        else
        {
            fprintf(f, "-warp %d\n", g->warp);
        }
    }

    if (g->monsters[0] == '1')
        fprintf(f, "-nomonsters\n");
    else if (g->monsters[0] == '2')
        fprintf(f, "-fast\n");
    else if (g->monsters[0] == '4')
        fprintf(f, "-respawn\n");
    else if (g->monsters[0] == '6')
        fprintf(f, "-fast\n-respawn\n");

    if (g->record)
        fprintf(f, "-record %s/mydemo\n", fs_temp_dir);
    else if (g->demo[0])
        fprintf(f, "-playdemo %s\n", g->demo);

    if (g->logfile)
        fprintf(f, "-logfile\n");

    fclose(f);
}

void FS_ExecGame(int profile)
{
    if (profile < 0 || profile >= MAX_PROFILES) return;

    struct Profile *g = fs_profiles + profile;
    static char rsp[MAX_FNAME];
    static char exe[128];
    static char *argv[3];

    if (g->rsp[0])
    {
        snprintf(rsp, sizeof(rsp), "@%s/%s", fs_pwad_dir, g->rsp);
    }
    else
    {
        rsp[0] = '@';
        snprintf(rsp + 1, sizeof(rsp) - 1, "%s/prboom.rsp", fs_temp_dir);
        WriteResponseFile(profile, rsp + 1);
    }

    snprintf(exe, sizeof(exe), "app0:/prboom.bin");

    argv[0] = exe;
    argv[1] = rsp;
    argv[2] = NULL;

    I_Cleanup();
    sceAppMgrLoadExec(exe, argv, NULL);
}
