#ifndef BATTLEGROUND_H_INCLUDED
#define BATTLEGROUND_H_INCLUDED

#include <Vision/Runtime/Base/Math/Random/VRandom.hpp>

class VPrefab;

class SceneBlocks
{
public:

  void prepare(const char* meshName, const hkvMat4& origin, float blockLength);
  void clear();
  void reset();
  void update(const hkvVec3& focus);

private:

  hkvMat4 mOrigin;
  float mBlockLength;
  VSmartPtr<VisStaticMesh_cl> mBlock;
  VPList mBlocks;
};

class Battleground
{
public:

  Battleground();

  void prepare(VisBaseEntity_cl* player);
  void clear();
  void restart();
  void updateEnvironment();
  void updateEnemy();

private:

  VisBaseEntity_cl* mPlayer;
  SceneBlocks mGround;
  SceneBlocks mFortification;

  VPrefab* mEnemyPrefab;
  VPList mEnemies;
  VRandom mRandom;
  float mLastSpawnDistance;
};

#endif