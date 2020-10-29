#if __linux__
  #define _XOPEN_SOURCE 500
#endif
#include "map.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

// ----------------- //
// Private functions //
// ----------------- //

/**
 * Delete the given layer.
 *
 * @param tile  The layer to be deleted
 */
void map_deleteLayer(struct Layer *layer) {
    if (layer != NULL) {
        for (unsigned int i = 0; i < layer->numRows; ++i) {
            free(layer->tiles[i]);
            free(layer->highlight[i]);
        }
        free(layer->tiles);
        free(layer->highlight);
    }
}

/**
 * Delete the given tile.
 *
 * @param tile  The tile to be deleted
 */
void map_deleteTile(struct Tile *tile) {
    if (tile != NULL) {
        if (tile->name != NULL)     free(tile->name);
        if (tile->filename != NULL) free(tile->filename);
        if (tile->image != NULL)    cairo_surface_destroy(tile->image);
    }
}

/**
 * Prints the tile to stdout
 *
 * @param layer  The tile to be printed
 */
void map_printTile(const struct Tile *tile) {
    printf("Tile \"%s\" (filename: %s) ", tile->name, tile->filename);
    printf("with %d directions : ", tile->numDirections);
    for (unsigned int i = 0; i < tile->numDirections; ++i) {
        printf("(%d,%d,%d)",
               tile->directions[i].deltaRow,
               tile->directions[i].deltaColumn,
               tile->directions[i].deltaLayer);
        if (i != tile->numDirections - 1) printf(", ");
    }
    printf("\n");
}
/**
 * Prints the layer to stdout.
 *
 * @param layer  The layer to be printed
 */
void map_printLayer(const struct Layer *layer) {
    printf("A layer of %d rows and %d columns\n",
           layer->numRows, layer->numColumns);
    for (unsigned int i = 0; i < layer->numRows; ++i) {
        printf("  ");
        for (unsigned int j = 0; j < layer->numColumns; ++j) {
            printf("%d ", layer->tiles[i][j]);
        }
        printf("\n");
    }
}

// --------- //
// Functions //
// --------- //

struct Map *map_createMap(unsigned int numRows,
                          unsigned int numColumns,
                          unsigned int maxLayers,
                          unsigned int maxTiles) {
    struct Map *map = (struct Map*)malloc(sizeof(struct Map));
    map->layers = (struct Layer*)malloc(maxLayers * sizeof(struct Layer));
    map->numLayers = 0;
    map->maxLayers = maxLayers;
    map->numRows = numRows;
    map->numColumns = numColumns;
    map->tiles = (struct Tile*)malloc(maxTiles * sizeof(struct Tile));
    map->tiles[0].name = strdup("empty");
    map->tiles[0].filename = strdup("empty");
    map->tiles[0].image = NULL;
    map->numTiles = 1;
    map->maxTiles = maxTiles;
    map->solution = NULL;
    return map;
}

void map_deleteMap(struct Map *map) {
    if (map != NULL) {
        for (unsigned int i = 0; i < map->numLayers; ++i) {
            map_deleteLayer(&map->layers[i]);
        }
        free(map->layers);
        map->layers = NULL;
        for (unsigned int i = 0; i < map->numTiles; ++i) {
            map_deleteTile(&map->tiles[i]);
        }
        free(map->tiles);
        map->tiles = NULL;
        free(map);
    }
}

void map_addDirection(struct Tile *tile,
                      const struct Direction *direction) {
    tile->directions[tile->numDirections] = *direction;
    ++tile->numDirections;
}

bool map_hasDirection(const struct Tile *tile,
                      const struct Direction *direction) {
    for (unsigned int i = 0; i < tile->numDirections; ++i) {
        if (tile->directions[i].deltaRow    == direction->deltaRow &&
            tile->directions[i].deltaColumn == direction->deltaColumn &&
            tile->directions[i].deltaLayer  == direction->deltaLayer) {
            return true;
        }
    }
    return false;
}

struct Tile *map_addTile(struct Map *map, const char *name, const char *filename) {
    if (map->numTiles < map->maxTiles + 1) {
        struct Tile *tile = &map->tiles[map->numTiles];
        tile->name = strdup(name);
        tile->filename = strdup(filename);
        tile->image = cairo_image_surface_create_from_png(filename);
        tile->numDirections = 0;
        ++map->numTiles;
        return tile;
    } else {
        return NULL;
    }
}

struct Layer *map_addLayer(struct Map *map, double offsetx, double offsety) {
    if (map->numLayers < map->maxLayers + 1) {
        struct Layer *layer = &map->layers[map->numLayers];
        layer->map = map;
        layer->numRows = map->numRows;
        layer->numColumns = map->numColumns;
        layer->offsetx = offsetx;
        layer->offsety = offsety;
        layer->tiles = (unsigned int**)malloc(layer->numRows * sizeof(unsigned int*));
        layer->highlight = (bool**)malloc(layer->numRows * sizeof(bool*));
        for (unsigned int i = 0; i < layer->numRows; ++i) {
            layer->tiles[i] = (unsigned int*)malloc(layer->numColumns * sizeof(unsigned int));
            layer->highlight[i] = (bool*)malloc(layer->numColumns * sizeof(bool));
            for (unsigned int j = 0; j < layer->numColumns; ++j) {
                layer->tiles[i][j] = 0;
                layer->highlight[i][j] = false;
            }
        }
        ++map->numLayers;
        return layer;
    } else {
        return NULL;
    }
}

void map_addSolution(struct Map *map, struct MapGraphPath *path) {
    map->solution = path;
    while (path != NULL) {
        map->layers[path->head.layer].highlight[path->head.row][path->head.column]
            = true;
        path = path->tail;
    }
}

bool map_hasTileAbove(const struct Map *map,
                      unsigned int row,
                      unsigned int column,
                      unsigned int layer) {
    assert(layer  < map->numLayers);
    assert(row    < map->numRows);
    assert(column < map->numColumns);
    assert(map->layers[layer].tiles[row][column] != 0);
    return layer < map->numLayers - 1 &&
        map->layers[layer + 1].tiles[row][column] != 0;
}

void map_printMap(const struct Map *map, bool withSolution) {
    printf("A map of %d layers of %d rows and %d columns using %d tiles\n",
           map->numLayers, map->numRows, map->numColumns, map->numTiles);
    for (unsigned int i = 0; i < map->numTiles; ++i) {
        printf("  Tile %d: ", i);
        map_printTile(&map->tiles[i]);
    }
    for (unsigned int i = 0; i < map->numLayers; ++i) {
        printf("  Layer %d: ", i);
        map_printLayer(&map->layers[i]);
    }
    if (withSolution && map->solution != NULL) {
        printf("  Solution: ");
        mapgraph_printPath(map->solution);
    }
}

void map_toPNG(const struct Map *map, const char *outputFilename) {
    unsigned int width  = 128 * (map->numRows + map->numColumns + 1);
    unsigned int height = 64  * (map->numRows + map->numColumns + map->numLayers + 2);
    cairo_surface_t *outputImage =
        cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t *cr = cairo_create(outputImage);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_fill(cr);
    cairo_translate(cr, 128 * (map->numRows - 0.5), (map->numLayers - 1) * 78);
    for (unsigned int k = 0; k < map->numLayers; ++k) {
        cairo_save(cr);
        cairo_translate(cr, map->layers[k].offsetx,
                            map->layers[k].offsety);
        for (unsigned int i = 0; i < map->numRows; ++i) {
            cairo_save(cr);
            for (unsigned int j = 0; j < map->numColumns; ++j) {
                unsigned int tileID = map->layers[k].tiles[i][j];
                if (tileID != 0) {
                    cairo_surface_t *image = map->tiles[tileID].image;
                    cairo_pattern_t *imagePattern =
                        cairo_pattern_create_for_surface(image);
                    cairo_set_source(cr, imagePattern);
                    cairo_paint(cr);
                    cairo_pattern_destroy(imagePattern);
                    if (map->layers[k].highlight[i][j]) {
                        cairo_save(cr);
                        cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
                        cairo_set_source(cr, imagePattern);
                        cairo_paint(cr);
                        cairo_restore(cr);
                    }
                }
                cairo_translate(cr, 128, 64);
            }
            cairo_restore(cr);
            cairo_translate(cr, -128, 64);
        }
        cairo_restore(cr);
    }
    cairo_destroy(cr);
    cairo_surface_write_to_png(outputImage, outputFilename);
    cairo_surface_destroy(outputImage);
}
