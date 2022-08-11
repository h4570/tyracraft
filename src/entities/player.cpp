/*
# ______       ____   ___
#   |     \/   ____| |___|
#   |     |   |   \  |   |
#-----------------------------------------------------------------------
# Copyright 2020, tyra - https://github.com/h4570/tyra
# Licenced under Apache License 2.0
# Sandro Sobczyński <sandro.sobczynski@gmail.com>
*/

#include "entities/player.hpp"
#include "utils.hpp"
#include <debug/debug.hpp>
#include <math/math.hpp>
#include "file/file_utils.hpp"
#include "loaders/3d/md2/md2_loader.hpp"
#include "renderer/3d/mesh/dynamic/dynamic_mesh.hpp"

using Tyra::BBox;
using Tyra::FileUtils;
using Tyra::MD2Loader;

// ----
// Constructors/Destructors
// ----

Player::Player(Renderer* t_renderer, Audio* t_audio) {
  this->t_renderer = t_renderer;
  this->t_audio = t_audio;

  this->loadMesh();

  // Phisycs values
  lift = Vec4(0.0f, -250.0F, 0.0f);
  velocity = Vec4(0.0f, 0.0f, 0.0f);

  isWalking = false;
  isFighting = false;
  isWalkingAnimationSet = false;
  isJumpingAnimationSet = false;
  isFightingAnimationSet = false;

  walkAdpcm = this->t_audio->loadADPCM("sounds/walk.adpcm");
  jumpAdpcm = this->t_audio->loadADPCM("sounds/jump.adpcm");
  boomAdpcm = this->t_audio->loadADPCM("sounds/boom.adpcm");
  this->t_audio->setADPCMVolume(70, 0);
}

Player::~Player() {}

// ----
// Methods
// ----

void Player::update(const float& deltaTime, Pad& t_pad, Camera& t_camera,
                    Block* t_blocks[], unsigned int blocks_ammount) {
  this->handleInputCommands(t_pad);

  this->nextPlayerPos = getNextPosition(deltaTime, t_pad, t_camera);
  if (this->nextPlayerPos->x != this->mesh->getPosition()->x ||
      this->nextPlayerPos->y != this->mesh->getPosition()->y ||
      this->nextPlayerPos->z != this->mesh->getPosition()->z) {
    this->updatePosition(t_blocks, blocks_ammount, deltaTime);
  }

  this->checkIfIsOnBlock(t_blocks, blocks_ammount);
  this->updateGravity(deltaTime);

  delete nextPlayerPos;
  nextPlayerPos = NULL;
}

void Player::handleInputCommands(Pad& t_pad) {
  if (t_pad.getClicked().L1) this->moveSelectorToTheLeft();
  if (t_pad.getClicked().R1) this->moveSelectorToTheRight();
  if (t_pad.getClicked().Cross && this->isOnGround) {
    this->velocity = this->lift;
    this->isOnGround = 0;
    // this->t_audio->playADPCM(jumpAdpcm);
  }

  if (t_pad.getRightJoyPad().h >= 200)
    this->mesh->rotation.rotateZ(-0.08F);
  else if (t_pad.getRightJoyPad().h <= 100)
    this->mesh->rotation.rotateZ(0.08F);
}

Vec4* Player::getNextPosition(const float& deltaTime, Pad& t_pad,
                              const Camera& t_camera) {
  Vec4* result = new Vec4(*mesh->getPosition());
  Vec4 normalizedCamera;
  normalizedCamera.set(t_camera.unitCirclePosition);
  normalizedCamera.normalize();
  normalizedCamera *= (this->speed * deltaTime);

  if (t_pad.getLeftJoyPad().v <= 100) {
    result->x += -normalizedCamera.x;
    result->z += -normalizedCamera.z;
  } else if (t_pad.getLeftJoyPad().v >= 200) {
    result->x += normalizedCamera.x;
    result->z += normalizedCamera.z;
  }
  if (t_pad.getLeftJoyPad().h <= 100) {
    result->x += -normalizedCamera.z;
    result->z += normalizedCamera.x;
  } else if (t_pad.getLeftJoyPad().h >= 200) {
    result->x += normalizedCamera.z;
    result->z += -normalizedCamera.x;
  }

  return result;
}

/** Update player position by gravity and update index of current block */
void Player::updateGravity(const float& deltaTime) {
  this->velocity += GRAVITY;  // Negative gravity to decrease Y axis
  Vec4 newYPosition = *mesh->getPosition() - (this->velocity * deltaTime);

  if (newYPosition.y >= OVERWORLD_MAX_HEIGH * DUBLE_BLOCK_SIZE ||
      newYPosition.y < OVERWORLD_MIN_HEIGH * DUBLE_BLOCK_SIZE) {
    // Maybe has died, teleport to spaw area
    printf("\nReseting player position to:\n");
    this->spawnArea.print();
    this->mesh->getPosition()->set(this->spawnArea);
    this->velocity = Vec4(0.0f, 0.0f, 0.0f);
    return;
  }

  if (this->currentBlock != NULL &&
      newYPosition.y < this->currentBlock->maxCorner.y + 1.0f) {
    newYPosition.y = this->currentBlock->maxCorner.y;
    this->velocity = Vec4(0.0f, 0.0f, 0.0f);
    this->isOnGround = 1;
  }

  // Finally updates gravity after checks
  mesh->getPosition()->set(newYPosition);
}

void Player::updatePosition(Block* t_blocks[], int blocks_ammount,
                            const float& deltaTime) {
  // Check if is at the world's edge
  if (!nextPlayerPos->collidesBox(MIN_WORLD_POS, MAX_WORLD_POS)) return;

  Vec4 currentPlayerPos = *this->mesh->getPosition();
  Vec4 playerMin = Vec4();
  Vec4 playerMax = Vec4();
  BBox playerBB = (BBox)this->mesh->getCurrentBoundingBox();
  playerBB.getMinMax(&playerMin, &playerMax);
  playerMin += currentPlayerPos;
  playerMax += currentPlayerPos;
  Vec4 rayOrigin = currentPlayerPos;
  Vec4 rayDir = *this->nextPlayerPos - currentPlayerPos;
  rayDir.normalize();
  Vec4 inflatedMin = Vec4();
  Vec4 inflatedMax = Vec4();
  float finalHitDistance = -1.0f;
  float tempHitDistance = -1.0f;
  float maxDistanceOfFrame = this->speed * deltaTime;

  for (int i = 0; i < blocks_ammount; i++) {
    Vec4 tempInflatedMin = Vec4();
    Vec4 tempInflatedMax = Vec4();

    // Check if player would collide (Broad phase);
    // TODO: filter the block that are beyond the max distance of frame;
    if (playerMin.y >= t_blocks[i]->getPosition()->y) {
      continue;
    };

    // Inflate bbox
    // TODO: implement sliding on colide;
    Utils::GetMinkowskiSum(playerMin, playerMax, t_blocks[i]->minCorner,
                           t_blocks[i]->maxCorner, &tempInflatedMin,
                           &tempInflatedMax);

    tempHitDistance =
        Utils::Raycast(&rayOrigin, &rayDir, &tempInflatedMin, &tempInflatedMax);

    if (tempHitDistance > -1.0f &&
        (finalHitDistance == -1.0f || tempHitDistance < finalHitDistance)) {
      finalHitDistance = tempHitDistance;
      inflatedMin.set(tempInflatedMin);
      inflatedMax.set(tempInflatedMax);
    }
  }

  // Updates position if collides;

  if (finalHitDistance > -1.0f) {
    const float timeToHit = finalHitDistance / this->speed;
    if (timeToHit <= deltaTime ||
        finalHitDistance <
            this->mesh->getPosition()->distanceTo(*nextPlayerPos)) {
      // Slide on collision
      const float friction = 0.8f;
      const float remainingtime = deltaTime - timeToHit;
      const Vec4 hitPosition = rayOrigin + (rayDir * finalHitDistance);
      const Vec4 oldVel = *nextPlayerPos - currentPlayerPos;
      Vec4 normal = Utils::GetNormalFromHitPosition(hitPosition, inflatedMin,
                                                    inflatedMax);
      Vec4 tempVector = oldVel.cross(normal);
      Vec4 newVel = normal.cross(tempVector);
      newVel.normalize();
      newVel *= speed * friction * remainingtime;
      mesh->getPosition()->set(currentPlayerPos + newVel);
      return;
    }
  }

  // Apply new position;
  mesh->getPosition()->x = nextPlayerPos->x;
  mesh->getPosition()->z = nextPlayerPos->z;
}

void Player::checkIfIsOnBlock(Block* t_blocks[], int blocks_ammount) {
  this->currentBlock = NULL;
  Vec4 playerPos = *this->mesh->getPosition();

  for (int i = 0; i < blocks_ammount; i++) {
    if (playerPos.y < t_blocks[i]->maxCorner.y) continue;

    float distanceToBlock = playerPos.distanceTo(*t_blocks[i]->getPosition());

    if (distanceToBlock <= MAX_RANGE_PICKER) {
      if (playerPos.isOnBox(t_blocks[i]->minCorner, t_blocks[i]->maxCorner)) {
        if (this->currentBlock == NULL) {
          this->currentBlock = t_blocks[i];
          continue;
        }

        if (distanceToBlock <
            playerPos.distanceTo(*this->currentBlock->getPosition())) {
          this->currentBlock = t_blocks[i];
        }
      }
    }
  }
}

/**
 * Inventory controllers
 *
 */

ITEM_TYPES Player::getSelectedInventoryItemType() {
  return this->inventory[this->selectedInventoryIndex];
}

/**
 * @brief Return selected slot - int between 1 and 9
 *
 */
u8 Player::getSelectedInventorySlot() {
  return this->selectedInventoryIndex + 1;
}

void Player::moveSelectorToTheLeft() {
  selectedInventoryIndex--;
  if (selectedInventoryIndex < 0) selectedInventoryIndex = INVENTORY_SIZE - 1;
  selectedSlotHasChanged = 1;
}

void Player::moveSelectorToTheRight() {
  selectedInventoryIndex++;
  if (selectedInventoryIndex > INVENTORY_SIZE - 1) selectedInventoryIndex = 0;
  selectedSlotHasChanged = 1;
}

void Player::loadMesh() {
  MD2Loader loader;
  auto* data =
      loader.load(FileUtils::fromCwd("meshes/player/warrior.md2"), .35F, true);
  this->mesh = new DynamicMesh(*data);
  // result->translation.translateZ(-30.0F);
  this->mesh->rotation.rotateX(-1.566F);
  this->mesh->rotation.rotateZ(1.566F);
  delete data;
  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("meshes/player/warrior.png"))
      ->addLink(this->mesh->getId());
  this->mesh->playAnimation(0, this->mesh->getFramesCount() - 1);
  this->mesh->setAnimSpeed(0.17F);
}
