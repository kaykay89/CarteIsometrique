/**
 * Module map
 *
 * This module is responsible of handling isometric maps.
 *
 * A map is represented as an ordered list of layers, on which there are tiles
 * that are distributed on a rectangular grid. Below is the logical
 * representation of a layer having 3 rows and 4 columns.
 *
 *           c c c c
 *           o o o o
 *           l l l l
 *           u u u u
 *           m m m m
 *           n n n n
 *                  
 *           1 2 3 4
 *                 
 *           | | | |
 *           V V V V
 *
 *          +-+-+-+-+
 * row 1 -> | | | | |
 *          +-+-+-+-+
 * row 2 -> | | | | |
 *          +-+-+-+-+
 * row 3 -> | | | | |
 *          +-+-+-+-+
 *
 * The corresponding isometric view of these tiles is then illustrated below.
 *
 *          row 1 -> /\
 *                  /  \ <- column 1
 *        row 2 -> /\  /\
 *                /  \/  \ <- column 2
 *      row 3 -> /\  /\  /\
 *              /  \/  \/  \ <- column 3
 *              \  /\  /\  /\
 *               \/  \/  \/  \ <- column 4
 *                \  /\  /\  /
 *                 \/  \/  \/ 
 *                  \  /\  /
 *                   \/  \/ 
 *                    \  /
 *                     \/ 
 *
 * Tiles are identified by a positive integer ID (the empty tile is identified
 * by 0). Since a tile might appear multiple times in a map, a precise copy is
 * called a cell and is identified uniquely by a triple (L,R,C), where L is the
 * layer number, R is the row and C is the column.
 *
 * The module provides four data structures:
 *
 * - ``struct Direction``, representing a move from one cell to another;
 * - ``struct Tile``, representing a tile, whose image can be used as many
 *   times as needed;
 * - ``struct Layer``, representing a layer in the map;
 * - ``struct Map``, representing the map.
 *
 * @author   Alexandre Blondin Massé
 * @version  1.0
 * @date     June 18th, 2017
 */
#ifndef MAP_H
#define MAP_H

#include <stdlib.h>
#include <stdbool.h>
#include <cairo.h>
#include "map_graph.h"

// --------------- //
// Data structures //
// --------------- //

struct Direction {   // A direction
    int deltaRow;    // The displacement row-wise
    int deltaColumn; // The displacement column-wise
    int deltaLayer;  // The displacement layer-wise
};

struct Tile {                        // A tile
    char *name;                      // The name of the tile
    char *filename;                  // The filename of the image for the tile
    struct Direction directions[12]; // The allowed directions
    unsigned int numDirections;      // The number of directions
    cairo_surface_t *image;          // The image of the tile
};

struct Layer {               // A layer
    struct Map *map;         // The map in which the layer is
    unsigned int **tiles;    // A matrix of the tiles in the layer
    bool **highlight;        // If true, hilight the tile
    unsigned int numRows;    // The number of rows
    unsigned int numColumns; // The number of columns
    double offsetx;          // The x-offset
    double offsety;          // The y-offset
};

struct Map {                       // A map
    struct Layer *layers;          // The layers
    unsigned int numLayers;        // The current number of layers
    unsigned int maxLayers;        // The maximum number of layers
    unsigned int numRows;          // The number of rows
    unsigned int numColumns;       // The number of columns
    struct Tile *tiles;            // The allowed tiles in the map
    unsigned int numTiles;         // The number of allowed tiles in the map
    unsigned int maxTiles;         // The maximum number of allowed tiles in
                                   // the map
    struct MapGraphPath *solution; // The solution of the map
};

// --------- //
// Functions //
// --------- //

/**
 * Creates an empty map with given dimensions.
 *
 * @param numRows     The number of rows of the map
 * @param numColumns  The number of columns of the map
 * @param maxLayers   The maximum number of layers in the map
 * @param maxTiles    The maximum number of allowed tiles in the map
 * @return            The created map
 */
struct Map *map_createMap(unsigned int numRows,
                          unsigned int numColumns,
                          unsigned int maxLayers,
                          unsigned int maxTiles);

/**
 * Deletes the given map.
 *
 * @param map  The map to be deleted
 */
void map_deleteMap(struct Map *map);

/**
 * Adds an allowed direction to the given tile.
 *
 * @param tile       The tile to which the direction is added
 * @param direction  The allowed direction
 */
void map_addDirection(struct Tile *tile,
                      const struct Direction *direction);

/**
 * Returns true if the given tile has the given direction.
 *
 * @param tile       The tile to be checked
 * @param direction  The direction to be checked
 * @return           True if the tile has the given direction
 */
bool map_hasDirection(const struct Tile *tile,
                      const struct Direction *direction);

/**
 * Adds a tile to the given map.
 *
 * @param map       The map to which the tile is added
 * @param name      The name of the tile
 * @param filename  The filename of the image for the tile
 * @return          The new tile
 */
struct Tile *map_addTile(struct Map *map,
                         const char *name,
                         const char *filename);

/**
 * Adds a layer to the given map.
 *
 * @param map      The map to which the layer is added
 * @param offsetx  The x-offset of the layer
 * @param offsety  The x-offset of the layer
 * @return         The new layer
 */
struct Layer *map_addLayer(struct Map *map, double offsetx, double offsety);

/**
 * Adds a solution to the given map.
 *
 * @param map   The map
 * @param path  The path to highlight
 */
void map_addSolution(struct Map *map, struct MapGraphPath *path);

/**
 * Returns true if the given tile has another tile above itself.
 *
 * @param map     The map containing the tile
 * @param row     The row of the tile
 * @param column  The column of the tile
 * @param layer   The layer of the tile
 */
bool map_hasTileAbove(const struct Map *map,
                      unsigned int row,
                      unsigned int column,
                      unsigned int layer);
/**
 * Prints the map to stdout.
 *
 * @param map           The map to be printed
 * @param withSolution  Prints the solution if true
 */
void map_printMap(const struct Map *map, bool withSolution);

/**
 * Generates a PNG file for the given map.
 *
 * @param map             The map to be drawn
 * @param outputFilename  The output filename
 */
void map_toPNG(const struct Map *map, const char *outputFilename);

#endif
