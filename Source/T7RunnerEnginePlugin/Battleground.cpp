#include "T7RunnerEnginePluginPCH.h"
#include "Battleground.h"
#include "GameSetting.h"

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/TriggerBoxEntity.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VPrefab.hpp>

void SceneBlocks::prepare(const char* meshName, const hkvMat4& origin, float blockLength)
{
  mOrigin = origin;
  mBlockLength = blockLength;
  mBlock =  VisStaticMesh_cl::GetResourceManager().LoadStaticMeshFile(meshName);
  VASSERT(mBlock != NULL);
}

void SceneBlocks::clear()
{
  // StaticMeshInstance will be purged by ResourceManager so no need to delete explicitly
  mBlocks.Clear();
  mBlock = NULL;
}

void SceneBlocks::reset()
{
  VisStaticMeshInstance_cl* block = (VisStaticMeshInstance_cl *)mBlocks.Pop();
  while (block != NULL)
  {
    block->DisposeObject();
    block = (VisStaticMeshInstance_cl *)mBlocks.Pop();
  }
}

void SceneBlocks::update(const hkvVec3& focus)
{
  if (mBlock == NULL) return;

  float expectFarDist = focus.y - FAR_BLOCK;
  float expectNearDist = focus.y + NEAR_BLOCK;
  float halfLength = mBlockLength / 2;

  VisStaticMeshInstance_cl* block = NULL;
  if (mBlocks.GetLength() == 0)
  { // Create the first block
    block = mBlock->CreateInstance(mOrigin, NULL, false);
    block->ReComputeBoundingBoxes();
    block->AssignToVisibilityZones();
    mBlocks.Append(block);
  }

  hkvMat4 transform;
  while (true)
  { // Spawn new blocks
    block = (VisStaticMeshInstance_cl*)mBlocks[mBlocks.GetLength() - 1];
    transform = block->GetTransform();
    if ((transform.getTranslation().y - halfLength) > expectFarDist)
    {
      hkvVec3 pos = transform.getTranslation();
      pos.y -= mBlockLength;
      transform.setTranslation(pos);
      block = mBlock->CreateInstance(transform, NULL, false);
      block->ReComputeBoundingBoxes();
      block->AssignToVisibilityZones();
      mBlocks.Append(block);
    }
    else break;
  }

  while (true)
  { // Dispose obsolete blocks
    VisStaticMeshInstance_cl* block = (VisStaticMeshInstance_cl *)mBlocks[0];
    if (block != NULL)
    {
      if ((block->GetTransform().getTranslation().y - halfLength) > expectNearDist)
      {
        block->DisposeObject();
        mBlocks.RemoveAt(0);
      }
      else break;
    }
    else break;
  }
}

Battleground::Battleground()
{
  mPlayer = NULL;
  mEnemyPrefab = NULL;
}

void Battleground::prepare(VisBaseEntity_cl* player)
{
  VASSERT(player != NULL);
  mPlayer = player;

  // Environment
  hkvMat4 transform; transform.setIdentity();
  mGround.prepare("Meshes/GroundBlock.vmesh", transform, 1000.f);

  transform.setRotationMatrixZ(180.f);
  transform.setScalingFactors(hkvVec3(0.5f, 0.5f, 0.5f));
  transform.setTranslation(hkvVec3(-510.f, 0.f, 120.f));
  mFortification.prepare("Meshes/Rd_Fortification03.VMESH", transform, 1610.f);

  // Enemy
  if (mEnemyPrefab == NULL)
    mEnemyPrefab = VPrefabManager::GlobalManager().LoadPrefab("Prefabs/BrownHorse.vprefab");

  // Pre-instance all enemies
  VPrefabInstanceInfo info;
  info.m_bOutputInstances = true;
  for (int i = 0; i < ENEMY_STOCK; ++i) {
    info.m_vInstancePos.set(LANE_CENTER, FAR_BLOCK, 0.f);
    info.m_vInstanceEuler.set(180.f, 0.f, 0.f); // Turn 180 so enemy faces the player
    mEnemyPrefab->Instantiate(info);

    VASSERT(info.m_Instances.GetSize() >= 1);
    if (info.m_Instances.GetSize() >= 1)
    {
      VisBaseEntity_cl* horse = vdynamic_cast<VisBaseEntity_cl*>(info.m_Instances[0]);
      VASSERT(horse != NULL);
      if (horse != NULL) mEnemies.Append(horse);
    }
  }

  // Observe enemies
  for (int i = 0; i < mEnemies.GetLength(); ++i)
    TriggerBoxEntity_cl::AddObservedEntity((VisBaseEntity_cl*)mEnemies[i]);

  float currentTime = Vision::GetTimer()->GetTimeDifference();
  mRandom.Reset((unsigned int)(currentTime * 1000)); // The time is usually 0.0X so x1000
  mLastSpawnDistance = 0.f;
}

void Battleground::clear()
{
  // Observe enemies
  for (int i = 0; i < mEnemies.GetLength(); ++i)
    TriggerBoxEntity_cl::RemoveObservedEntity((VisBaseEntity_cl*)mEnemies[i]);

  mEnemies.Clear();
  mGround.clear();
  mFortification.clear();
}

void Battleground::restart()
{
  mLastSpawnDistance = 0.f;

  // Reposition all enemies
  for (int i = 0; i < mEnemies.GetLength(); ++i)
    ((VisBaseEntity_cl*)mEnemies[i])->SetPosition(LANE_CENTER, FAR_BLOCK, 0.f);

  // Environment
  mGround.reset();
  mFortification.reset();
}

void Battleground::updateEnvironment()
{
  const hkvVec3& playerPos = mPlayer->GetPosition();
  mGround.update(playerPos);
  mFortification.update(playerPos);
}

void Battleground::updateEnemy()
{
  const hkvVec3& playerPos = mPlayer->GetPosition();
  float expectFarDist = playerPos.y - FAR_BLOCK;
  float expectNearDist = playerPos.y + NEAR_BLOCK;

  const float timeDiff = Vision::GetTimer()->GetTimeDifference();
  float forwardDistance = timeDiff * ENEMY_SPEED;

  // Move enemies and collect two free ones
  VPList freeEnemies(2);
  for (int i = 0; i < mEnemies.GetLength(); ++i)
  {
    VisBaseEntity_cl* enemy = (VisBaseEntity_cl*)mEnemies[i];
    hkvVec3 pos = enemy->GetPosition();
    if (pos.y < expectNearDist)
    {
      pos.y += forwardDistance;
      enemy->SetPosition(pos);
    }
    else if (freeEnemies.GetLength() < 2)
    {
      freeEnemies.Append(enemy);
    }
  }
  VASSERT_MSG(freeEnemies.GetLength() == 2, "Need to instance more enemies");

  if (mLastSpawnDistance - expectFarDist > ENEMY_GAP)
  { // Spawn new enemy
    int seed = mRandom.GetInt() % PATTERN_COUNT;
    for (int i = 0; i < LANE_COUNT; ++i)
    {
      if (SPAWN_PATTERN[seed][i] == 1) {
        VisBaseEntity_cl* enemy = (VisBaseEntity_cl*)freeEnemies.Pop();
        if (enemy != NULL)
        {
          hkvVec3 pos(LANES[i], expectFarDist, 0);
          enemy->SetPosition(pos);
        }
      }
    }
    mLastSpawnDistance = expectFarDist;
  }
}
