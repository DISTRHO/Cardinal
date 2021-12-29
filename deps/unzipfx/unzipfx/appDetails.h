
#ifndef __APP_DETAILS_H__
#define __APP_DETAILS_H__

#include "../../src/Cardinal/DistrhoPluginInfo.h"

#define SFX_APP_BANNER DISTRHO_PLUGIN_NAME " self-contained executable, based on UnZipSFX"

#ifdef WIN32
# define SFX_AUTORUN_CMD  "\\" DISTRHO_PLUGIN_LABEL ".exe"
#else
# define SFX_AUTORUN_CMD  "/" DISTRHO_PLUGIN_LABEL
#endif

void  sfx_app_set_args(int argc, char** argv);
int   sfx_app_autorun_now();
char* sfx_get_tmp_path();

#endif // __APP_DETAILS_H__
