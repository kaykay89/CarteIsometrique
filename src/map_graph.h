/**
 * Module map_graph
 *
 * This module is responsible of handling graphs representing maps.
 *
 * A graph is a common data structure used to represent networks. It is
 * completely determined by its vertices (or nodes) and the edges (or links)
 * between the nodes.
 *
 * In our case, we represent an isometric map by a graph whose nodes are
 * exactly the cells that are free (i.e. there is no cell above the tile),
 * meaning that we can "move" on this cell. Also, there is an edge (a link)
 * between two cells if it is possible to move from one cell to the other. The
 * interest of representing a map by a graph is that one can, in particular,
 * compute a path between two cells in the map.
 *
 * The module provides four data structures:
 *
 * - ``struct MapCell``, representing a cell in the map;
 * - ``struct MapCellNode``, representing a node in the graph;
 * - ``struct MapGraph``, representing the graph of a map;
 * - ``struct MapGraphPath``, representing a path from one cell to another in
 *   the graph.
 *
 * @author   Alexandre Blondin Massé
 * @version  1.0
 * @date     June 18th, 2017
 */
#ifndef MAP_GRAPH_H
#define MAP_GRAPH_H

#include <stdbool.h>
#include "map.h"

// --------------- //
// Data structures //
// --------------- //

struct MapCell {         // A cell (position) on a map
    unsigned int row;    // The row of the cell
    unsigned int column; // The column of the cell
    unsigned int layer;  // The layer on which the cell is
};

struct MapCellNode {                // A node in the map graph
    unsigned int index;             // The index of this node
    struct MapCell cell;            // The cell for this node
    struct Tile *tile;              // The tile for this node
    struct MapCellNode **neighbors; // The neighbors of the node
    unsigned int numNeighbors;      // The number of neighbors of the node
    unsigned int capacity;          // The capacity of the node
};

struct MapGraph {              // A map graph
    const struct Map *map;     // The associated map
    struct MapCellNode *nodes; // The nodes in the graph
    unsigned int numNodes;     // The number of nodes
    unsigned int capacity;     // The capacity of the graph
};

struct MapGraphPath {          // A path in a map graph
    struct MapCell head;       // The first cell in the path
    struct MapGraphPath *tail; // The rest of the path
};

// --------- //
// Functions //
// --------- //

/**
 * Creates a graph from a map.
 *
 * In the resulting graph, each cell of the map that is free (i.e. there is no
 * other tile on top of it) is a node, and there is an edge (i.e. a link)
 * between two cells if it is possible to move from one cell to the other.
 *
 * @param map  The map
 * @return     The graph induced by the map
 */
struct MapGraph mapgraph_create(const struct Map *map);

/**
 * Deletes the given graph.
 *
 * @param graph  The graph to delete
 */
void mapgraph_delete(struct MapGraph *graph);

/**
 * Prints the given graph.
 *
 * @param graph  The graph to print
 */
void mapgraph_print(const struct MapGraph *graph);

/**
 * Writes the given graph to a dot file.
 *
 * The dot format is the format recognized by GraphViz, a software that allows
 * graph display. See http://www.graphviz.org/ for more details.
 *
 * @param graph           The graph to write
 * @param outputFilename  The name of the file
 */
void mapgraph_toDot(const struct MapGraph *graph,
                    const char *outputFilename);

/**
 * Returns a shortest path between two cells in the given graph.
 *
 * If such a path does not exist, then NULL is returned.
 *
 * Note: Do not forget to destroy the returned path once you are finished with
 * it.
 *
 * @param graph  The graph
 * @param start  The starting cell
 * @param end    The ending cell
 * @return       A shortest path between two cells
 */
struct MapGraphPath *mapgraph_shortestPath(const struct MapGraph *graph,
                                           const struct MapCell *start,
                                           const struct MapCell *end);

/**
 * Prints the given path.
 *
 * @param path  The path to be printed
 */
void mapgraph_printPath(const struct MapGraphPath *path);

/**
 * Deletes the given path.
 *
 * @param path  The path to delete
 */
void mapgraph_deletePath(struct MapGraphPath *path);

#endif
