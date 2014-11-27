#ifndef HORSE_HPP_INCLUDED
#define HORSE_HPP_INCLUDED

class Horse : public VisBaseEntity_cl
{
public:

  enum Direction
  {
    LEFT,
    RIGHT
  };

  Horse();

  virtual void ThinkFunction() HKV_OVERRIDE;
  virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) HKV_OVERRIDE;

  void turn(Direction dir);
  void revive();
  bool isDead();

private:

  V_DECLARE_SERIAL_DLLEXP(Horse, SAMPLEPLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(Horse, SAMPLEPLUGIN_IMPEXP);

  void triggerAnimationEvent(const char* str);

  enum State
  {
    RUN,
    TURN_START,
    TURN_END,
    DEAD
  };

  State mState;
  Direction mTurnDirection;
  int mTargetLane;
};

#endif
