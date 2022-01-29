#ifndef _TERRAIN_GENERATOR_
#define _TERRAIN_GENERATOR_

#include "modules/texture_repository.hpp"
#include "camera.hpp"
#include "../objects/chunck.hpp"
class Engine;
class TerrainGenerator
{
    public:
        TerrainGenerator();
        ~TerrainGenerator();

        void init(Engine *t_engine);
        void update(const Pad &t_pad);
    private:
        
};

#endif