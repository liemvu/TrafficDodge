//
//  GameConfig.h
//  VNTraffic
//
//  Created by William Vo on 4/23/14.
//
//

#ifndef VNTraffic_GameConfig_h
#define VNTraffic_GameConfig_h

#define PLAYER_INITIAL_SPEED 200
#define PLAYER_INITIAL_MOVE_SPEED 200
#define ENV_ACCELERATION 10
#define NUMBER_OF_LINES 6

#define SPEED_INCREASE_MAX 1000
#define SPEED_INCREASE_AMOUNT 20
#define SPEED_INCREASE_INTERVAL 10

#define SPAWN_VEHICLE_MAX 0.5f
#define SPAWN_VEHICLE_DECREASE_AMOUNT -0.25f
#define SPAWN_VEHICLE_INTERVAL 2
#define VEHICLE_SPEED_RATIO 10

#define MAX_LIFE 3

#define SCORE_RATIO 0.2f

const float LINE_POS[6] = {0.859f, 0.71875f, 0.578125f, 0.421875f, 0.28125f, 0.140625f};
#endif
