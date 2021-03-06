#include "World.h"

#include <iostream>

#include "manager/InterfaceTypeManager.h"
#include "manager/ItemTypeManager.h"
#include "manager/TileTypeManager.h"
#include "utility/Tools.h"

#include "manager/item_defs.h"
#include "manager/sound_defs.h"
#include "manager/tile_defs.h"

#include "manager/SoundManager.h"
#include "utility/KeyListener.h"

#include "Graphics.h"

bool comparePtrToNode(Tile* a, Tile* b) {
  return (*a < *b);
}

/************
 * TILE MAP *
 ************/
World::World() {
  x = 0;
  y = 0;

  ticks = 0;
  map_buffer = nullptr;

  map_messages = new Messenger(1, false, -4);

  VIEWPORT_ZOOM = 1.0f;

  world_map = new TileMap();
}

// Ticker
World::~World() {}

volatile int World::ticks = 0;

void World::tickCounter() {
  ticks++;
}
END_OF_FUNCTION(tickCounter)

// Init ticker to 20 ticks per second
void World::initTicker() {
  LOCK_VARIABLE(ticks);
  LOCK_FUNCTION(tickCounter);

  install_int_ex(tickCounter, BPS_TO_TIMER(20));
}

/*
 * IMAGES
 */
// Draw bottom tiles
void World::draw(BITMAP* tempBuffer) {
  // Error check
  if (!tempBuffer || !map_buffer)
    return;

  // Clear buffer
  rectfill(map_buffer, 0, 0, map_buffer->w, map_buffer->h, makecol(255, 0, 0));

  // Drawable
  Graphics::Instance()->draw(map_buffer, x, y,
                             x + VIEWPORT_WIDTH / VIEWPORT_ZOOM,
                             y + VIEWPORT_HEIGHT / VIEWPORT_ZOOM);

  // Draw buffer
  stretch_blit(map_buffer, tempBuffer, 0, 0, VIEWPORT_WIDTH / VIEWPORT_ZOOM,
               VIEWPORT_HEIGHT / VIEWPORT_ZOOM, 0, 0, VIEWPORT_WIDTH,
               VIEWPORT_HEIGHT);

  // Draw temperature indicator
  const char temp =
      world_map->getTemperatureAt(x + (VIEWPORT_WIDTH / VIEWPORT_ZOOM) / 2,
                                  y + (VIEWPORT_HEIGHT / VIEWPORT_ZOOM) / 2);
  const char r_val = (temp > 0) ? (temp / 2) : 0;
  const char b_val = (temp < 0) ? (temp / 2 * -1) : 0;

  if (r_val > 0)
    rectfill(overlay_buffer, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
             makeacol(255, 0, 0, r_val));
  if (b_val > 0)
    rectfill(overlay_buffer, 0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT,
             makeacol(0, 0, 255, b_val));

  draw_trans_sprite(tempBuffer, overlay_buffer, 0, 0);

  // Day night indicator

  // Message system
  map_messages->draw(tempBuffer, 5, 145);
}

// Load images
void World::loadImages() {
  TileTypeManager::sprite_sheet_tiles = loadBitmap("images/tiles.png");
  ItemTypeManager::sprite_sheet_items = loadBitmap("images/items.png");

  std::cout << "Loading data/tiles.json \n";
  if (TileTypeManager::loadTiles("data/tiles.json"))
    abort_on_error("Could not load data/tiles.json");

  std::cout << "Loading data/items.json \n";
  if (ItemTypeManager::loadItems("data/items.json"))
    abort_on_error("Could not load data/items.json");

  std::cout << "Loading data/interfaces.json \n";
  if (InterfaceTypeManager::loadInterfaces("data/interfaces.json"))
    abort_on_error("Could not load data/interfaces.json");

  std::cout << "Loading data/sounds.json \n";
  if (SoundManager::load("data/sounds.json"))
    abort_on_error("Could not load data/sounds.json");

  // Create map buffer
  map_buffer = create_bitmap(VIEWPORT_WIDTH * VIEWPORT_MAX_ZOOM,
                             VIEWPORT_HEIGHT * VIEWPORT_MAX_ZOOM);

  overlay_buffer = create_bitmap(VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

  world_map->generateMap();
}

/*
 * MAP
 */
// Interact with
void World::interact(int inter_x, int inter_y, Item* inHand) {
  Tile* foregroundTile =
      world_map->getTileAt(inter_x, inter_y, LAYER_FOREGROUND);
  Tile* midgroundTile = world_map->getTileAt(inter_x, inter_y, LAYER_MIDGROUND);
  Tile* backgroundTile =
      world_map->getTileAt(inter_x, inter_y, LAYER_BACKGROUND);

  // Hoe
  if (inHand->getID() == ITEM_HOE) {
    if (midgroundTile && !foregroundTile) {
      if (midgroundTile->getID() == TILE_GRASS) {
        world_map->replaceTile(
            midgroundTile,
            new Tile(TILE_PLOWED_SOIL, midgroundTile->getX(),
                     midgroundTile->getY(), midgroundTile->getZ()));
        SoundManager::play(SOUND_HOE);
      } else if (midgroundTile->getID() == TILE_SOIL) {
        world_map->replaceTile(
            midgroundTile,
            new Tile(TILE_PLOWED_SOIL, midgroundTile->getX(),
                     midgroundTile->getY(), midgroundTile->getZ()));
        SoundManager::play(SOUND_HOE);
      } else {
        SoundManager::play(SOUND_ERROR);
      }
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Scythe
  else if (inHand->getID() == ITEM_SCYTHE) {
    if (foregroundTile && foregroundTile->getID() == TILE_DENSE_GRASS) {
      world_map->removeTile(foregroundTile);
      world_map->placeItemAt(new Item(ITEM_HAY, 0), foregroundTile->getX(),
                             foregroundTile->getY());
      SoundManager::play(SOUND_SCYTHE);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Berry
  else if (inHand->getID() == ITEM_BERRY_SEED) {
    if (midgroundTile && midgroundTile->getID() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      world_map->placeTile(
          new Tile(TILE_BERRY, inter_x, inter_y, LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Tomato
  else if (inHand->getID() == ITEM_TOMATO_SEED) {
    if (midgroundTile && midgroundTile->getID() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      world_map->placeTile(
          new Tile(TILE_TOMATO, inter_x, inter_y, LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Carrot
  else if (inHand->getID() == ITEM_CARROT_SEED) {
    if (midgroundTile && midgroundTile->getID() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      world_map->placeTile(
          new Tile(TILE_CARROT, inter_x, inter_y, LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Lavender
  else if (inHand->getID() == ITEM_LAVENDER_SEED) {
    if (midgroundTile && midgroundTile->getID() == TILE_PLOWED_SOIL &&
        !foregroundTile) {
      world_map->placeTile(
          new Tile(TILE_LAVENDER, inter_x, inter_y, LAYER_FOREGROUND));
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Watering can
  else if (inHand->getID() == ITEM_WATERING_CAN) {
    if (midgroundTile && midgroundTile->getID() == TILE_WELL_PATH) {
      map_messages->pushMessage("Watering can filled");
      inHand->setMeta(8);
      SoundManager::play(SOUND_WATER_FILL);
    } else if (inHand->getMeta() > 0) {
      inHand->changeMeta(-1);
      map_messages->pushMessage("Watered");
      SoundManager::play(SOUND_WATER_POUR);
    } else {
      map_messages->pushMessage("Out of water");
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Axe
  else if (inHand->getID() == ITEM_AXE) {
    if (foregroundTile && foregroundTile->getID() == TILE_TREE) {
      world_map->replaceTile(
          foregroundTile,
          new Tile(TILE_STUMP, foregroundTile->getX(), foregroundTile->getY(),
                   foregroundTile->getZ()));
      world_map->placeItemAt(new Item(ITEM_STICK, 0), foregroundTile->getX(),
                             foregroundTile->getY());
      world_map->placeItemAt(new Item(ITEM_STICK, 0), foregroundTile->getX(),
                             foregroundTile->getY());
      world_map->placeItemAt(new Item(ITEM_WOOD, 0), foregroundTile->getX(),
                             foregroundTile->getY());
      SoundManager::play(SOUND_AXE);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Shovel
  else if (inHand->getID() == ITEM_SHOVEL) {
    if (foregroundTile && (foregroundTile->getID() == TILE_BUSH ||
                           foregroundTile->getID() == TILE_STUMP)) {
      world_map->removeTile(foregroundTile);
      SoundManager::play(SOUND_SHOVEL);
    } else if (midgroundTile && midgroundTile->getID() == TILE_GRASS &&
               !foregroundTile) {
      world_map->removeTile(midgroundTile);
      world_map->placeItemAt(new Item(ITEM_GRASS, 0), midgroundTile->getX(),
                             midgroundTile->getY());
      SoundManager::play(SOUND_SHOVEL);
    } else if (midgroundTile && midgroundTile->getID() == TILE_SAND &&
               !foregroundTile) {
      world_map->removeTile(midgroundTile);
      world_map->placeItemAt(new Item(ITEM_SAND, 0), midgroundTile->getX(),
                             midgroundTile->getY());
      SoundManager::play(SOUND_SHOVEL);
    } else if (foregroundTile && (foregroundTile->getID() == TILE_STONE_WALL)) {
      world_map->removeTile(foregroundTile);
      world_map->placeItemAt(new Item(ITEM_STONE, 0), foregroundTile->getX(),
                             foregroundTile->getY());
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Wood Wall
  else if (inHand->getID() == ITEM_WOOD) {
    if (!foregroundTile && midgroundTile) {
      world_map->placeTile(new Tile(TILE_WOOD_WALL, midgroundTile->getX(),
                                    midgroundTile->getY(), LAYER_FOREGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Fence
  else if (inHand->getID() == ITEM_STICK) {
    if (!foregroundTile && midgroundTile) {
      world_map->placeTile(new Tile(TILE_FENCE, midgroundTile->getX(),
                                    midgroundTile->getY(), LAYER_FOREGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Place dirt
  else if (inHand->getID() == ITEM_GRASS) {
    if (backgroundTile && !midgroundTile) {
      world_map->placeTile(new Tile(TILE_GRASS, backgroundTile->getX(),
                                    backgroundTile->getY(), LAYER_MIDGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
  // Place dirt
  else if (inHand->getID() == ITEM_SAND) {
    if (backgroundTile && !midgroundTile) {
      world_map->placeTile(new Tile(TILE_SAND, backgroundTile->getX(),
                                    backgroundTile->getY(), LAYER_MIDGROUND));
      SoundManager::play(SOUND_SHOVEL);
    } else {
      SoundManager::play(SOUND_ERROR);
    }
  }
}

// Update tile map
void World::update() {
  // Regen map
  if (key[KEY_R]) {
    world_map->clearMap();
    world_map->generateMap();
  }

  // One game tick (20x second)
  if (ticks >= 1) {
    ticks = 0;

    // Update tile map
    world_map->tick(x, y, x + VIEWPORT_WIDTH / VIEWPORT_ZOOM,
                    y + VIEWPORT_HEIGHT / VIEWPORT_ZOOM);
  }

  // Zooming
  if (KeyListener::keyPressed[KEY_PLUS_PAD] &&
      VIEWPORT_ZOOM < VIEWPORT_MAX_ZOOM) {
    VIEWPORT_ZOOM *= 2.0f;
  }
  if (KeyListener::keyPressed[KEY_MINUS_PAD] &&
      VIEWPORT_ZOOM > VIEWPORT_MIN_ZOOM) {
    VIEWPORT_ZOOM *= 0.5f;
  }
}

// Scroll
void World::scroll(int player_x, int player_y) {
  x = player_x + (TILE_WIDTH - VIEWPORT_WIDTH / VIEWPORT_ZOOM) / 2;
  y = player_y + (TILE_HEIGHT - VIEWPORT_HEIGHT / VIEWPORT_ZOOM) / 2;

  if (x < 0)
    x = 0;
  else if (x >
           world_map->getWidth() * TILE_WIDTH - VIEWPORT_WIDTH / VIEWPORT_ZOOM)
    x = world_map->getWidth() * TILE_WIDTH - VIEWPORT_WIDTH / VIEWPORT_ZOOM;

  if (y < 0)
    y = 0;
  else if (y > world_map->getHeight() * TILE_HEIGHT -
                   VIEWPORT_HEIGHT / VIEWPORT_ZOOM)
    y = world_map->getHeight() * TILE_HEIGHT - VIEWPORT_HEIGHT / VIEWPORT_ZOOM;
}
