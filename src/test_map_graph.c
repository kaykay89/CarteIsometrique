#include <stdio.h>
#include "map.h"
#include "map_graph.h"
#include "map_loader.h"

int main() {
    printf("Testing the map_graph module...\n");
    struct Map *map = map_loadMapFromJSONFile("data/map.json");
    struct MapGraph graph = mapgraph_create(map);
    map_printMap(map, false);
    mapgraph_print(&graph);
    struct MapCell start = {9,0,1}, end = {0,9,1};
    struct MapGraphPath *path =
        mapgraph_shortestPath(&graph, &start, &end);
    mapgraph_printPath(path);
    map_addSolution(map, path);
    map_toPNG(map, "map.png");
    mapgraph_deletePath(path);
    mapgraph_delete(&graph);
    map_deleteMap(map);
    return 0;
}
