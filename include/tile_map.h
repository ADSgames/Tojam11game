#ifndef TILE_MAP_H
#define TILE_MAP_H

#include<allegro.h>
#include<vector>

#include <iostream>
#include <string>
#include <fstream>

#include "tile.h"
#include "tools.h"



class tile_map
{
    public:
        tile_map();
        virtual ~tile_map();

        // Constant
        int MAP_WIDTH;
        int MAP_HEIGHT;
        int timer;

        static const int VIEWPORT_WIDTH = 240;
        static const int VIEWPORT_HEIGHT = 160;

        void draw( BITMAP *tempBuffer);
        void drawForeground( BITMAP *tempBuffer);

        void replace_tile( int tileX, int tileY, int newID, bool foreground);
        int get_tile_at( int positionX, int positionY, bool foreground);

        void place_item( item newItem);
        void update();

        bool is_solid_at( int positionX, int positionY);
        bool is_item_at( int positionX, int positionY);
        item *get_item_at( int positionX, int positionY);
        void remove_item_at( int positionX, int positionY);

        void load_map( std::string fileName);

        void generate_map();

        void load_images();

        void scroll( int player_x, int player_y);

        int x, y;

    protected:

    private:
        // Tiles
        std::vector<tile> map_tiles;
        std::vector<tile> map_tiles_foreground;

        std::vector<item> map_items;

        BITMAP *tile_images[100];
        BITMAP *item_images[100];

        std::string item_names[100];

        BITMAP *map_buffer;
};

#endif // TILE_MAP_H
