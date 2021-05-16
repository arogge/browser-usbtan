#include <gwenhywfar/libloader.h>
#include <gwenhywfar/plugin.h>
#include <stdio.h>

#include "resolve_func.h"

void* resolve_chiptanusb_func(const char* func)
{
  GWEN_PLUGIN_MANAGER* pm = GWEN_PluginManager_FindPluginManager("ct");
  if (!pm) {
    fprintf(stderr, "Plugin manager not found\n");
    return NULL;
  }

  GWEN_PLUGIN* pl = GWEN_PluginManager_GetPlugin(pm, "chiptanusb");
  if (!pl) {
    fprintf(stderr, "Plugin not found\n");
    return NULL;
  }

  GWEN_LIBLOADER* ll = GWEN_Plugin_GetLibLoader(pl);

  void* p;
  if (GWEN_LibLoader_Resolve(ll, func, &p) < 0) {
    fprintf(stderr, "Entrypoint not found\n");
    return NULL;
  }
  return p;
}
