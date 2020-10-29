#include "map_loader.h"
#include <stdio.h>
#include <jansson.h>

// ----------------- //
// Private functions //
// ----------------- //

/**
 * Loads directions from a JSON array for the given tile.
 *
 * @param tile      The tile into which the directions are loaded
 * @param jsonTile  The JSON object containing the tile
 */
void map_loadDirections(struct Tile *tile, const json_t *jsonTile) {
    json_t *jsonDirections = json_object_get(jsonTile, "directions");
    unsigned int i;
    json_t *jsonDirection;
    json_array_foreach(jsonDirections, i, jsonDirection) {
        int deltaRow, deltaColumn, deltaLayer;
        deltaRow = json_integer_value(json_array_get(jsonDirection, 0));
        deltaColumn = json_integer_value(json_array_get(jsonDirection, 1));
        deltaLayer = json_integer_value(json_array_get(jsonDirection, 2));
        struct Direction direction = {deltaRow, deltaColumn, deltaLayer};
        map_addDirection(tile, &direction);
    }
}

/**
 * Loads tiles from a JSON array into the given map.
 *
 * @param map        The map into which the tiles are loaded
 * @param jsonTiles  The JSON array containing the tiles
 */
void map_loadTiles(struct Map *map, const json_t *jsonTiles) {
    for (unsigned int tileID = 1; tileID < map->maxTiles; ++tileID) {
        json_t *jsonTile = json_array_get(jsonTiles, tileID - 1);
        const char *name = json_string_value(json_object_get(jsonTile, "id"));
        const char *filename = json_string_value(json_object_get(jsonTile, "filename"));
        struct Tile *tile = map_addTile(map, name, filename);
        map_loadDirections(tile, jsonTile);
    }
}

/**
 * Loads layers from a JSON array into the given map.
 *
 * @param map         The map into which the layers are loaded
 * @param jsonLayers  The JSON array containing the layers
 */
void map_loadLayers(struct Map *map, const json_t *jsonLayers, int layerOffset) {
    unsigned int i;
    json_t *jsonLayer;
    json_array_foreach(jsonLayers, i, jsonLayer) {
        int yoffset = layerOffset * i;
        struct Layer *layer = map_addLayer(map, 0, yoffset);
        json_t *data = json_object_get(jsonLayer, "data");
        unsigned int j;
        json_t *dataTile;
        json_array_foreach(data, j, dataTile) {
            unsigned int tileID = (unsigned int)json_integer_value(dataTile);
            layer->tiles[j / map->numColumns][j % map->numColumns] = tileID;
        }
    }
}

// --------- //
// Functions //
// --------- //

struct Map *map_loadMapFromJSONFile(const char *filename) {
    json_t *jsonRoot, *jsonTiles, *jsonLayers;
    json_error_t error;
    jsonRoot = json_load_file(filename, 0, &error);
    unsigned int numRows, numColumns, maxLayers, maxTiles;
    int layerOffset;
    struct Map *map;

    numRows = json_integer_value(json_object_get(jsonRoot, "numrows"));
    numColumns = json_integer_value(json_object_get(jsonRoot, "numcols"));
    layerOffset = json_integer_value(json_object_get(jsonRoot, "layeryoffset"));
    jsonLayers = json_object_get(jsonRoot, "layers");
    maxLayers = json_array_size(jsonLayers);
    jsonTiles = json_object_get(jsonRoot, "tiles");
    maxTiles = json_array_size(jsonTiles) + 1;
    map = map_createMap(numRows, numColumns, maxLayers, maxTiles);
    map_loadTiles(map, jsonTiles);
    map_loadLayers(map, jsonLayers, layerOffset);
    return map;
}
