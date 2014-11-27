#include "T7RunnerEnginePluginPCH.h"
#include "GameManager.h"
#include "Horse.h"

#include <Vision/Runtime/Base/Input/VInputTouch.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VOrbitCamera.hpp>

enum RUNNER_CONTROL
{
  TURN_LEFT,
  TURN_RIGHT,
  TURN,
  TOUCH_ANY
};

// One global static instance
MyGameManager MyGameManager::g_GameManager;

void MyGameManager::OneTimeInit()
{
  m_bPlayingTheGame = false;

  Vision::Callbacks.OnEditorModeChanged += this;
  Vision::Callbacks.OnAfterSceneLoaded += this;
  Vision::Callbacks.OnUpdateSceneBegin += this;
  Vision::Callbacks.OnWorldDeInit += this;
}

void MyGameManager::OneTimeDeInit()
{
  Vision::Callbacks.OnEditorModeChanged -= this;
  Vision::Callbacks.OnAfterSceneLoaded -= this;
  Vision::Callbacks.OnUpdateSceneBegin -= this;
  Vision::Callbacks.OnWorldDeInit -= this;
}

void MyGameManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnUpdateSceneBegin)
  {
    if (m_bPlayingTheGame)
    {
      if (mPlayerHorse != NULL)
      {
        if (mPlayerHorse->isDead())
        {
          mPlayerHorse->revive();
          mBattleGround.restart();

          // Reset camera
          VisBaseEntity_cl* camTarget = Vision::Game.SearchEntity("CameraTarget");
          if (camTarget != NULL)
          {
            VOrbitCamera* orbCam = camTarget->Components().GetComponentOfType<VOrbitCamera>();
            if (orbCam != NULL)
              orbCam->Teleport();
          }
        }

        mBattleGround.updateEnvironment();
        mBattleGround.updateEnemy();

#if defined(WIN32)
        if (mInputMap->GetTrigger(TURN_LEFT))
          mPlayerHorse->turn(Horse::LEFT);
        else if (mInputMap->GetTrigger(TURN_RIGHT))
          mPlayerHorse->turn(Horse::RIGHT);
#else
        static bool sDetect = false;
        if (mInputMap->GetTrigger(TOUCH_ANY))
          sDetect = true;

        if (sDetect)
        {
          const float SWIPE_DIST = 0.01f;
          const float dx = mInputMap->GetTrigger(TURN);
          if (dx < -SWIPE_DIST)
          {
            mPlayerHorse->turn(Horse::LEFT);
            sDetect = false;
          }
          else if (dx > SWIPE_DIST)
          {
            mPlayerHorse->turn(Horse::RIGHT);
            sDetect = false;
          }
        }
#endif

        // Display distance
        const hkvVec3& pos = mPlayerHorse->GetPosition();
        Vision::Message.Print(1, Vision::Video.GetXRes() - 120, 10, "Distance : %.0f M", -pos.y / 100.f);
      }
    }
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnEditorModeChanged)
  { // When vForge switches back from EDITORMODE_PLAYING_IN_GAME, turn off our play the game mode
    if (((VisEditorModeChangedDataObject_cl*)pData)->m_eNewMode != VisEditorManager_cl::EDITORMODE_PLAYING_IN_GAME)
      SetPlayTheGame(false);
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnAfterSceneLoaded)
  { // Gets triggered when the play-the-game vForge is started or outside vForge after loading the scene
    if (Vision::Editor.IsPlayingTheGame()) 
      SetPlayTheGame(true);
  }
  else if (pData->m_pSender == &Vision::Callbacks.OnWorldDeInit)
  { // This is important when running outside vForge
    SetPlayTheGame(false);
  }
}

void MyGameManager::SetPlayTheGame(bool bStatus)
{
  if (m_bPlayingTheGame != bStatus)
  {
    m_bPlayingTheGame = bStatus;
    if (m_bPlayingTheGame)
    {
       // Setup input map
       mInputMap = new VInputMap(8, 8);
#if defined(WIN32)
       mInputMap->MapTrigger(TURN_LEFT, V_KEYBOARD, CT_KB_LEFT, VInputOptions::Once(ONCE_ON_PRESS));
       mInputMap->MapTrigger(TURN_RIGHT, V_KEYBOARD, CT_KB_RIGHT, VInputOptions::Once(ONCE_ON_PRESS));
#else
       VTouchAreaPtr touchArea = new VTouchArea(VInputManager::GetTouchScreen());
       mInputMap->MapTrigger(TURN, touchArea, CT_TOUCH_ABS_DELTA_X, VInputOptions::Sensitivity(0.25f / Vision::Video.GetDeviceDpi()));
       mInputMap->MapTrigger(TOUCH_ANY, touchArea, CT_TOUCH_ANY, VInputOptions::Once(ONCE_ON_PRESS));
#endif

       mPlayerHorse = vdynamic_cast<Horse*>(Vision::Game.SearchEntity("Horse"));
       VASSERT(mPlayerHorse != NULL);

       mBattleGround.prepare(mPlayerHorse);
    }
    else
    {
      V_SAFE_DELETE(mInputMap);
      mBattleGround.clear();
      mPlayerHorse = NULL;
    }
  }
}
