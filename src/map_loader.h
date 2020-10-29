/**
 * Module map_loader
 *
 * This module provides a function to load a map from a JSON file.
 *
 * @author   Alexandre Blondin Massé
 * @version  1.0
 * @date     June 18th, 2017
 */
#ifndef MAP_LOADER_H
#define MAP_LOADER_H

#include "map.h"

// --------- //
// Functions //
// --------- //

/**
 * Loads and returns a map from a JSON file.
 *
 * @param filename  The name of the JSON file
 * @return          The loaded map
 */
struct Map *map_loadMapFromJSONFile(const char *filename);

#endif
