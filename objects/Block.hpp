#ifndef _BLOCK_
#define _BLOCK_

#include "../camera.hpp"
#include <tamtypes.h>
#include <modules/pad.hpp>
#include <modules/timer.hpp>
#include <modules/texture_repository.hpp>

#define BOCK_SIZE 7.00F

/** Block 3D object class  */
class Block
{
public:
    Mesh mesh;

    Block();
    ~Block();
    void init(Mesh &mother, float X, float Y, float Z);
};

#endif
