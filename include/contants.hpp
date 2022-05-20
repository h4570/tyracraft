#ifndef _CONSTANTS_
#define _CONSTANTS_

#define OVERWORLD_H_DISTANCE (256 / 2)                       // Total horizontal overworld distance;
#define OVERWORLD_V_DISTANCE (64 / 2)                        // Total vertical overworld distance;
#define HALF_OVERWORLD_H_DISTANCE (OVERWORLD_H_DISTANCE / 2) // Half horizontal overworld distance;
#define HALF_OVERWORLD_V_DISTANCE (OVERWORLD_V_DISTANCE / 2) // Half vertical overworld distance;
#define OVERWORLD_MIN_DISTANCE -HALF_OVERWORLD_H_DISTANCE  // Min overworld width;
#define OVERWORLD_MAX_DISTANCE HALF_OVERWORLD_H_DISTANCE   // Max overworld width;
#define OVERWORLD_MIN_HEIGH -HALF_OVERWORLD_V_DISTANCE     // Min overworld layer for badblock;
#define OVERWORLD_MAX_HEIGH HALF_OVERWORLD_V_DISTANCE      // Max overworld heigh layer;

#define OVERWORLD_SIZE ((OVERWORLD_H_DISTANCE * OVERWORLD_H_DISTANCE) * OVERWORLD_V_DISTANCE)
#define CHUNCK_SIZE 16
#define HALF_CHUNCK_SIZE (CHUNCK_SIZE / 2)
#define BLOCK_SIZE 7.0F
#define DUBLE_BLOCK_SIZE (BLOCK_SIZE * 2.0F)

// Define blocks ids baseds on https://minecraft.fandom.com/wiki/;
#define AIR_BLOCK 0
#define STONE_BLOCK 1
#define GRASS_BLOCK 2
#define DIRTY_BLOCK 3
#define WATER_BLOCK 4

//Should be updated avery time a new block is added;
#define BLOCKS_COUNTER 5

#define FIRST_PERSON_CAM 1
#define THIRD_PERSON_CAM 2

#define MAX_RANGE_PICKER (DUBLE_BLOCK_SIZE * 8.0F) // How far the player can pick a block

// Game states
#define MAIN_MENU 0
#define IN_GAME 1
#define IN_GAME_MENU 2
#define SPLASH_SCREEN 3

enum GAME_MODE { SURVIVAL, CREATIVE };

#endif