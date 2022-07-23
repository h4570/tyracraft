#ifndef _WORLD_
#define _WORLD_

#include <engine.hpp>
#include <tamtypes.h>
#include <time/timer.hpp>
#include <pad/pad.hpp>
#include <renderer/renderer.hpp>
#include "chunck.hpp"
#include "entities/player.hpp"
#include "managers/terrain_manager.hpp"
#include "managers/items_repository.hpp"
#include "contants.hpp"

using Tyra::Renderer;

class World {
 public:
  World();
  ~World();

  Renderer* t_renderer;
  Chunck* chunck;
  TerrainManager* terrainManager = new TerrainManager();

  void init(Renderer* t_renderer, ItemRepository* itemRepository);
  void update(Player* t_player, Camera* t_camera, Pad* t_pad);
  void render();
};

#endif