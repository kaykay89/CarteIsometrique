/**
 * Module tp2
 *
 * This is the main module of the program, which generates isometric map from a
 * JSON file. Currently, three output formats are supported:
 * - "text" format, which simply displays information about the map in a
 *   human-readable manner;
 * - "dot" format, which is the format used by Graphviz, a free and open-source
 *   software displaying graphs and networks;
 * - "png" format, which produces a PNG image of the map.
 *
 * The command line arguments are first retrieved and processed by the
 * `parse_args` module, then the pertinent services are called.
 *
 * @author   Alexandre Blondin Massé
 * @version  1.0
 * @date     June 18th, 2017
 */
#include <stdio.h>
#include <string.h>
#include "parse_args.h"
#include "map.h"
#include "map_graph.h"
#include "map_loader.h"

int main(int argc, char **argv) {
    struct Arguments arguments = parseArguments(argc, argv);
    if (arguments.status == TP2_OK && !arguments.showHelp) {
        struct Map *map;
        struct MapGraph graph;
        struct MapGraphPath *path;
        struct MapCell start, end;
        map = map_loadMapFromJSONFile(arguments.inputFilename);
        graph = mapgraph_create(map);
        path = NULL;
        start.layer = arguments.startLayer;
        start.row = arguments.startRow;
        start.column = arguments.startColumn;
        end.layer = arguments.endLayer;
        end.row = arguments.endRow;
        end.column = arguments.endColumn;
        if (arguments.withSolution) {
            path = mapgraph_shortestPath(&graph, &start, &end);
            map_addSolution(map, path);
        }
        if (strcmp(arguments.outputFormat, "text") == 0) {
            map_printMap(map, arguments.withSolution);
        } else if (strcmp(arguments.outputFormat, "png") == 0) {
            map_toPNG(map, arguments.outputFilename);
        } else if (strcmp(arguments.outputFormat, "dot") == 0) {
            mapgraph_toDot(&graph, arguments.outputFilename);
        }
        map_deleteMap(map);
        mapgraph_delete(&graph);
        if (arguments.withSolution) mapgraph_deletePath(path);
    }
    return arguments.status;
}
