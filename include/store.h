#ifndef STORE_H
#define STORE_H

#include <allegro.h>
#include <vector>

#include "tile.h"
#include "tools.h"
#include "character.h"
#include "keyListener.h"

class store
{
    public:
        store();
        virtual ~store();

        bool open;

        std::vector<item*> storeItems;

        void draw( BITMAP *tempBitmap);
        void draw_background( BITMAP *tempBitmap);

        void open_store( character *new_inventory);
        void close_store();

        void load_data();

        void add_item( item *storeItem){ storeItems.push_back( storeItem);}

        void update();

    protected:

    private:

        int tick;
        BITMAP *image;
        // Here's to your scummy coding standards
        BITMAP* image_background;
        BITMAP *indicator;

        SAMPLE* buy;
        SAMPLE* sell;
        SAMPLE* error;

        character *customer_inventory;

        int selector_index;
};

#endif // STORE_H
