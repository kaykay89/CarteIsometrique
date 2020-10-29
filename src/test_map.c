#include "map.h"
#include <stdio.h>

int main() {
    struct Map *map;
    unsigned int r = 8, c = 7;
    int yoffset = -78;
    map = map_createMap(r, c, 3, 8);
    map_printMap(map, false);
    map_addTile(map, "flat", "art/flat.png");
    map_addTile(map, "start", "art/start.png");
    map_addTile(map, "end", "art/end.png");
    map_addTile(map, "ne", "art/ne.png");
    map_addTile(map, "ne", "art/sw.png");
    map_printMap(map, false);
    struct Layer *layer;
    // Layer 1
    layer = map_addLayer(map, 0, 0);
    for (unsigned int i = 0; i < r; ++i) {
        for (unsigned int j = 0; j < c; ++j) {
            layer->tiles[i][j] = 1;
        }
    }
    // Layer 2
    layer = map_addLayer(map, 0, yoffset);
    layer->tiles[0][0] = 2;
    layer->tiles[r - 1][c - 1] = 3;
    layer->tiles[2][1] = 4;
    layer->tiles[1][1] = 1;
    layer->tiles[6][2] = 1;
    layer->tiles[4][5] = 1;
    layer->tiles[5][6] = 1;
    layer->tiles[4][6] = 1;
    // Layer 3
    layer = map_addLayer(map, 0, 2 * yoffset);
    layer->tiles[4][5] = 4;
    layer->tiles[6][2] = 1;
    layer->tiles[5][6] = 4;
    layer->tiles[4][6] = 5;

    map_printMap(map, false);
    map_toPNG(map, "map.png");
    map_deleteMap(map);
}

