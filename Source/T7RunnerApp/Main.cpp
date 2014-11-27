#include "T7RunnerAppPCH.h"
#include <Vision/Runtime/Framework/VisionApp/VAppImpl.hpp>

// Use the following line to initialize a plugin that is statically linked.
// Only Windows platform links plugins dynamically (on Windows you can comment out this line).
VIMPORT IVisPlugin_cl* GetEnginePlugin_T7RunnerEnginePlugin();

class T7RunnerApp : public VAppImpl
{
public:
  virtual void SetupAppConfig(VisAppConfig_cl& config) HKV_OVERRIDE;
  virtual void PreloadPlugins() HKV_OVERRIDE;
  virtual void Init() HKV_OVERRIDE;
};

VAPP_IMPLEMENT_SAMPLE(T7RunnerApp);

void T7RunnerApp::SetupAppConfig(VisAppConfig_cl& config)
{
  // Set custom file system root name ("havok_sdk" by default)
  config.m_sFileSystemRootName = "workspace";

#if defined(WIN32)
  config.m_videoConfig.m_iXRes = 360;
  config.m_videoConfig.m_iYRes = 640;
  config.m_videoConfig.m_iXPos = 800;
  config.m_videoConfig.m_iYPos = 100;
  config.m_videoConfig.m_szWindowTitle = "T7Runner";
  config.m_videoConfig.m_bWaitVRetrace = true;
#endif
}

void T7RunnerApp::PreloadPlugins()
{
  // Use the following line to load a plugin. Remember that, except on Windows platform, in addition
  // you still need to statically link your plugin library (e.g. on mobile platforms) through project
  // Properties, Linker, Additional Dependencies.
  VISION_PLUGIN_ENSURE_LOADED(T7RunnerEnginePlugin);
}

void T7RunnerApp::Init()
{
  // Set filename and paths to our stand alone version.
  // Note: "/Data/Vision/Base" is always added by the sample framework
  VisAppLoadSettings settings("Scenes/Default.vscene");
  settings.m_customSearchPaths.Append(":workspace/Assets");
  LoadScene(settings);
}
