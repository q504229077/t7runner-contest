#include "T7RunnerEnginePluginPCH.h"
#include "T7RunnerEnginePluginModule.h"
#include "Horse.h"
#include "GameSetting.h"

#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/HavokBehaviorEnginePlugin.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokBehaviorEnginePlugin/vHavokBehaviorComponent.hpp>

V_IMPLEMENT_SERIAL(Horse, VisBaseEntity_cl, 0, &g_myComponentModule);

Horse::Horse()
{
  mState = RUN;
  mTurnDirection = LEFT;
  mTargetLane = 1;
}

void Horse::ThinkFunction()
{
  // Move forward
  const float timeDiff = Vision::GetTimer()->GetTimeDifference();
  float forwardDist = timeDiff * PLAYER_SPEED;
  hkvVec3 pos = GetPosition();
  pos.y -= forwardDist;

  if ((mState == TURN_START) || (mState == TURN_END))
  { // Changing lane
    float sideDist = timeDiff * TURN_SPEED;
    if (mTurnDirection == LEFT)
    {
      pos.x += sideDist;
      if (pos.x >= LANES[mTargetLane])
      {
        pos.x = LANES[mTargetLane];
        mState = RUN;
      }
    }
    else //if (mTurnDirection == RIGHT)
    {
      pos.x -= sideDist;
      if (pos.x <= LANES[mTargetLane])
      {
        pos.x = LANES[mTargetLane];
        mState = RUN;
      }
    }

    if (mState == TURN_START)
    {
      if (hkvMath::Abs(pos.x - LANES[mTargetLane]) < RESTORE_DISTANCE)
      {
        triggerAnimationEvent("ToForward");
        mState  = TURN_END;
      }
    }
  }

  SetPosition(pos);
}

void Horse::MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB)
{
  VisObject3D_cl::MessageFunction(iID, iParamA, iParamB);
  if (iID == VIS_MSG_TRIGGER)
  {
    mState = DEAD;
  }
}

void Horse::turn(Direction dir)
{
  if (mState != DEAD)
  {
    mTurnDirection = dir;

    int newLane;
    if (mTurnDirection == LEFT)
      newLane = hkvMath::Max(0, mTargetLane - 1);
    else //if (mTargetLane == RIGHT)
      newLane = hkvMath::Min(2, mTargetLane + 1);

    if (newLane != mTargetLane)
    {
      if (mTurnDirection == LEFT)
        triggerAnimationEvent("ToLeft");
      else if (mTurnDirection == RIGHT)
        triggerAnimationEvent("ToRight");

      mTargetLane = newLane;
      mState = TURN_START;
    }
  }
}

void Horse::revive()
{
  mState = RUN;
  mTargetLane = 1;
  SetPosition(LANE_CENTER, 0.f, 0.f);
  triggerAnimationEvent("ToForward");
}

bool Horse::isDead()
{
  return mState == DEAD;
}

void Horse::triggerAnimationEvent(const char* str)
{
  vHavokBehaviorComponent* behavior = Components().GetComponentOfType<vHavokBehaviorComponent>();
  VASSERT(behavior != NULL);
  if (behavior) behavior->TriggerEvent(str);
}
