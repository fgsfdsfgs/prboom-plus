#include "net.h"

#include <stdlib.h>
#include <string.h>
#include <vitasdk.h>

#define NET_INIT_SIZE 1 * 1024 * 1024

char net_my_ip[64] = { 0 };
static void *net_memory = NULL;

int NET_Init(void)
{
    sceSysmoduleLoadModule(SCE_SYSMODULE_NET);

    int ret = sceNetShowNetstat();
    if (ret == SCE_NET_ERROR_ENOTINIT)
    {
        net_memory = malloc(NET_INIT_SIZE);
        SceNetInitParam initparam;
        initparam.memory = net_memory;
        initparam.size = NET_INIT_SIZE;
        initparam.flags = 0;
        ret = sceNetInit(&initparam);
        if (ret < 0) { free(net_memory); return -1; }
    }

    ret = sceNetCtlInit();
    if (ret < 0)
    {
        sceNetTerm();
        free(net_memory);
        return -2;
    }

    NET_GetMyIP(net_my_ip, sizeof(net_my_ip));
    return 0;
}

void NET_Free(void)
{
    sceNetCtlTerm();
    sceNetTerm();
    free(net_memory);
}

int NET_GetMyIP(char *out, int outlen)
{
    if (!out || !outlen) return -1;

    SceNetCtlInfo info;
    memset(&info, 0, sizeof(SceNetCtlInfo));
    if (sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &info) != 0)
        strncpy(out, "0.0.0.0", outlen);
    else
        strncpy(out, info.ip_address, outlen);

    return 0;
}
