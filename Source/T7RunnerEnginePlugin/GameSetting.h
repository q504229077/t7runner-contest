// The forward direction of player is -Y
// The left direction of player is +X
// The unit of Vision is cm

// Horse
const float PLAYER_SPEED = 1500.f;
const float ENEMY_SPEED = 1000.f;
const float TURN_SPEED = 600.f;
const float RESTORE_DISTANCE = 90.f;

// Lane
const int LANE_COUNT = 3;
const float LANE_CENTER = -50.f;
const float LANE_SPACING = 180.f;
const float LANES[3] = {LANE_CENTER + LANE_SPACING, LANE_CENTER, LANE_CENTER - LANE_SPACING};

// Environment
const float FAR_BLOCK = 5000.f;
const float NEAR_BLOCK = 300.f;

// Enemy
const float ENEMY_GAP = 1000.f;
const int ENEMY_STOCK = 10;
const int PATTERN_COUNT = 6;
const int SPAWN_PATTERN[PATTERN_COUNT][LANE_COUNT] = {
  { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 },
  { 1, 1, 0 }, { 1, 0, 1 }, { 0, 1, 1 },
};
