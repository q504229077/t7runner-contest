// Basic Plugin Framework to house your own components
//

#include "T7RunnerEnginePluginPCH.h"
#include "GameManager.h"

#include <Common/Base/KeyCode.h>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokSync.inl>

VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavok();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavokBehavior();
VIMPORT IVisPlugin_cl* GetEnginePlugin_vFmodEnginePlugin();

class T7RunnerEnginePlugin : public IVisPlugin_cl
{
public:

  void OnInitEnginePlugin();
  void OnDeInitEnginePlugin();

  const char *GetPluginName()
  { // Must match DLL name
    return "T7RunnerEnginePlugin";
  }
};

// Global plugin instance
T7RunnerEnginePlugin g_myComponents;

// Create a global instance of a VModule class
// Note: g_myComponentModule is defined in stdfx.h
DECLARE_THIS_MODULE(g_myComponentModule, MAKE_VERSION(1,0),
                    "Sample Plugin",
                    "Havok",
                    "A sample plugin for entities", &g_myComponents);

// Use this to get and initialize the plugin when you link statically
VEXPORT IVisPlugin_cl* GetEnginePlugin_T7RunnerEnginePlugin(){ return &g_myComponents; }

#if (defined _DLL) || (defined _WINDLL)
// The engine uses this to get and initialize the plugin dynamically
VEXPORT IVisPlugin_cl* GetEnginePlugin() { return GetEnginePlugin_T7RunnerEnginePlugin(); }
#endif

void T7RunnerEnginePlugin::OnInitEnginePlugin()
{
  hkvLog::Info("T7RunnerEnginePlugin:OnInitEnginePlugin()");

  VISION_HAVOK_SYNC_STATICS();
  VISION_PLUGIN_ENSURE_LOADED(vHavok);
  VISION_PLUGIN_ENSURE_LOADED(vHavokBehavior);
  VISION_PLUGIN_ENSURE_LOADED(vFmodEnginePlugin);

  Vision::RegisterModule(&g_myComponentModule);
  MyGameManager::GlobalManager().OneTimeInit();
}

void T7RunnerEnginePlugin::OnDeInitEnginePlugin()
{
  hkvLog::Info("T7RunnerEnginePlugin:OnDeInitEnginePlugin()");
  MyGameManager::GlobalManager().OneTimeDeInit();
  Vision::UnregisterModule(&g_myComponentModule);
  VISION_HAVOK_UNSYNC_STATICS();
}
