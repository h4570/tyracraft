#include <models/mesh.hpp>
#include <loaders/obj_loader.hpp>
#include "chunck.hpp"

Chunck::Chunck(){};

Chunck::~Chunck()
{
}

void Chunck::update(Player *t_player)
{
    for (u16 i = 0; i < this->blocks.size(); i++)
    {
        float visibility = 255 * this->getVisibityByPosition(
            t_player->getPosition().distanceTo(this->blocks[i]->mesh.position));
        for (u16 materialIndex = 0; materialIndex < this->blocks[i]->mesh.getMaterialsCount(); materialIndex++)
        {
            this->blocks[i]->mesh.getMaterial(materialIndex).color.a = visibility;
        }
    }
}

void Chunck::renderer(Renderer *t_renderer)
{
    // Draw mesh
    // TODO: draw with mesh array as Tyra Engine recommends;
    for (u16 i = 0; i < this->blocks.size(); i++)
    {
        t_renderer->draw(this->blocks[i]->mesh);
    }
};

void Chunck::add(Block *t_block)
{
    this->blocks.push_back(t_block);
};

void Chunck::clear()
{
    this->blocks.clear();
    this->blocks.shrink_to_fit();
}

/**
 * Calculate the FOG by distance;
 */
float Chunck::getVisibityByPosition(float d)
{
    return Utils::FOG_EXP_GRAD(d, 0.007F, 3.0F);
}