//  This class show how to use the vForge "Play the Game" mode

#ifndef PLUGINMANAGER_H_INCLUDED
#define PLUGINMANAGER_H_INCLUDED

#include "T7RunnerEnginePluginModule.h"
#include "Battleground.h"

class Horse;

class MyGameManager : public IVisCallbackHandler_cl
{
public:

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  // Called when plugin is loaded/unloaded
  void OneTimeInit();
  void OneTimeDeInit();

  // Switch to play-the-game mode. When not in vForge, this is default
  void SetPlayTheGame(bool bStatus);

  // Access one global instance of the Game manager
  static MyGameManager& GlobalManager() { return g_GameManager; }

private:

  bool m_bPlayingTheGame;
  static MyGameManager g_GameManager;

  VInputMap* mInputMap;
  Battleground mBattleGround;
  Horse* mPlayerHorse;
};

#endif
