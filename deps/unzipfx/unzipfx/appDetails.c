
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
# include <windows.h>
#else
# include <unistd.h>
#endif

#include "appDetails.h"

#define CMD_BUF_LEN 1024

static int    sfx_app_argc = 0;
static char** sfx_app_argv = NULL;
static char   sfx_tmp_path[512] = { 0 };

void sfx_app_set_args(int argc, char** argv)
{
    sfx_app_argc = argc;
    sfx_app_argv = argv;
}

int sfx_app_autorun_now()
{
    int i, cmdBufLen = 0;
    char cmdBuf[CMD_BUF_LEN];

    const char* const path = sfx_get_tmp_path();
    chdir(path);

    strcpy(cmdBuf, path);
    strcat(cmdBuf, SFX_AUTORUN_CMD);

    cmdBufLen = strlen(cmdBuf);

    for (i=0; i < sfx_app_argc; i++)
    {
        if (! sfx_app_argv[i])
            continue;

        cmdBufLen += strlen(sfx_app_argv[i]) + 1;
        if (cmdBufLen >= CMD_BUF_LEN-1)
            break;

        strcat(cmdBuf, " ");
        strcat(cmdBuf, sfx_app_argv[i]);
    }

    puts(SFX_APP_BANNER);
    printf("Launching: '%s'\n", cmdBuf);

#ifdef WIN32
    ShellExecute(NULL, "open", cmdBuf, NULL, NULL, SW_SHOWNORMAL);
    return 0;
#else
    const int ret = system(cmdBuf);
    exit(ret);
    return ret;
#endif
}

char* sfx_get_tmp_path()
{
#ifdef WIN32
    GetTempPathA(512 - strlen(DISTRHO_PLUGIN_LABEL), sfx_tmp_path);

    strcat(sfx_tmp_path, DISTRHO_PLUGIN_LABEL);
#else
    char* const tmp = getenv("TMP");

    if (tmp)
        strcpy(sfx_tmp_path, tmp);
    else
        strcpy(sfx_tmp_path, "/tmp");

    strcat(sfx_tmp_path, "/" DISTRHO_PLUGIN_LABEL);
#endif

    return sfx_tmp_path;
}
