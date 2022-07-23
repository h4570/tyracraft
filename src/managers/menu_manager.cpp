#include "managers/menu_manager.hpp"
#include "file/file_utils.hpp"
#include <renderer/renderer_settings.hpp>
#include <debug/debug.hpp>

using Tyra::Audio;
using Tyra::Renderer;
using Tyra::RendererSettings;
using Tyra::FileUtils;

MainMenu::MainMenu() {}

MainMenu::~MainMenu() {
  t_audio->stopSong();
  t_audio->setSongLoop(1);
}

void MainMenu::init(Renderer* t_renderer, Audio* t_audio) {
  /**
   * TODO: ->
   * - Load menu option
   *      - Play Game (DONE!)
   *      - How to play
   *      - About
   * */
  this->t_renderer = t_renderer;
  this->t_audio = t_audio;

  const float halfWidth = t_renderer->core.getSettings().getWidth() / 2;
  const float halfHeight = t_renderer->core.getSettings().getHeight() / 2;

  // Load title
  // Title
  title[0].setMode(Tyra::MODE_STRETCH);
  title[0].size.set(128, 96);
  title[0].position.set((halfWidth)-256, 64);

  title[1].setMode(Tyra::MODE_STRETCH);
  title[1].size.set(128, 96);
  title[1].position.set(halfWidth - 128, 64);

  title[2].setMode(Tyra::MODE_STRETCH);
  title[2].size.set(128, 96);
  title[2].position.set(halfWidth, 64);

  title[3].setMode(Tyra::MODE_STRETCH);
  title[3].size.set(128, 96);
  title[3].position.set(halfWidth + 128, 64);

  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/menu/title_1.png"))
      ->addLink(title[0].getId());
  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/menu/title_2.png"))
      ->addLink(title[1].getId());
  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/menu/title_3.png"))
      ->addLink(title[2].getId());
  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/menu/title_4.png"))
      ->addLink(title[3].getId());

  // Alpha Version
  subtitle.setMode(Tyra::MODE_STRETCH);
  subtitle.size.set(130, 16);
  subtitle.position.set((halfWidth) - (130 / 2), 164);

  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/menu/sub_title.png"))
      ->addLink(subtitle.getId());

  // Buttons
  btnCross.setMode(Tyra::MODE_STRETCH);
  btnCross.size.set(45, 45);
  btnCross.position.set(30,
                        this->t_renderer->core.getSettings().getHeight() - 60);

  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/ui/btn_cross.png"))
      ->addLink(btnCross.getId());

  // Load slots
  slot[0].setMode(Tyra::MODE_STRETCH);
  slot[0].size.set(200, 25);
  slot[0].position.set((halfWidth)-100, 265);
  slot[1].setMode(Tyra::MODE_STRETCH);
  slot[1].size.set(200, 25);
  slot[1].position.set((halfWidth)-100, 265 + 30);
  slot[2].setMode(Tyra::MODE_STRETCH);
  slot[2].size.set(200, 25);
  slot[2].position.set((halfWidth)-100, 265 + 60);

  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/menu/slot.png"))
      ->addLink(slot[0].getId());
  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/menu/hovered_slot.png"))
      ->addLink(slot[1].getId());
  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/menu/selected_slot.png"))
      ->addLink(slot[2].getId());

  // Texts
  textPlayGame.setMode(Tyra::MODE_STRETCH);
  textPlayGame.size.set(80, 15);
  textPlayGame.position.set((halfWidth)-40, 265 + 35);

  if (this->activeOption == PLAY_GAME) {
    textPlayGame.color.r = 255;
    textPlayGame.color.g = 255;
    textPlayGame.color.b = 0;
  }

  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/menu/play_game.png"))
      ->addLink(textPlayGame.getId());

  textSelect.setMode(Tyra::MODE_STRETCH);
  textSelect.size.set(64, 16);
  textSelect.position.set(
      30 + 40, this->t_renderer->core.getSettings().getHeight() - 47);
  this->t_renderer->core.texture.repository
      .add(FileUtils::fromCwd("assets/textures/menu/select.png"))
      ->addLink(textSelect.getId());

  // TODO: Refactore to tyrav2 (no obj loader and BMP image format)
  //   menuSkybox.loadObj("meshes/menu-skybox/", "skybox", 400.0F, false);
  //   menuSkybox.shouldBeFrustumCulled = false;
  //   this->t_renderer->core.texture.repository.addByMesh("meshes/menu-skybox/",
  //   menuSkybox, BMP);

  // Load song
  t_audio->loadSong(FileUtils::fromCwd("sounds/menu.wav"));
  t_audio->playSong();
  t_audio->setSongLoop(1);
  t_audio->setSongVolume(100);
}

void MainMenu::update(Pad& t_pad) {
  if (t_pad.getClicked().Cross) this->selectedOption = this->activeOption;
}

void MainMenu::render() {
  // Meshes
  // menuSkybox.rotation.y += .001F;
  // this->t_renderer->renderer2D.render(&menuSkybox);

  /**
   * --------------- Sprites ---------------
   * */

  // Title & Subtitle
  this->t_renderer->renderer2D.render(&title[0]);
  this->t_renderer->renderer2D.render(&title[1]);
  this->t_renderer->renderer2D.render(&title[2]);
  this->t_renderer->renderer2D.render(&title[3]);
  this->t_renderer->renderer2D.render(&subtitle);

  // Slots
  this->t_renderer->renderer2D.render(&slot[1]);
  // this->t_renderer->renderer2D.render(&slot[1]);
  // this->t_renderer->renderer2D.render(&slot[2]);

  // Texts
  this->t_renderer->renderer2D.render(&textPlayGame);
  this->t_renderer->renderer2D.render(&textSelect);

  // Buttons
  this->t_renderer->renderer2D.render(&btnCross);
}

u8 MainMenu::shouldInitGame() { return this->selectedOption == PLAY_GAME; }