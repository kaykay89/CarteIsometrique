#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "map_graph.h"
#include "queue.h"

// ----------------- //
// Private functions //
// ----------------- //

/**
 * Prints the given node.
 *
 * @param node  The node to print
 */
void mapgraph_printNode(const struct MapCellNode *node) {
    printf("  Node #%d of cell (%d,%d,%d) containing tile \"%s\" with %d neighbors\n",
           node->index,
           node->cell.row,
           node->cell.column,
           node->cell.layer,
           node->tile->name,
           node->numNeighbors);
}

/**
 * Adds a new cell to the given graph.
 *
 * @param graph   The graph
 * @param tile    The tile associated with the cell
 * @param row     The row of the cell
 * @param column  The column of the cell
 * @param layer   The layer of the cell
 */
void mapgraph_addCell(struct MapGraph *graph,
                      struct Tile *tile,
                      unsigned int row,
                      unsigned int column,
                      unsigned int layer) {
    if (graph->numNodes == graph->capacity) {
        graph->capacity *= 2;
        graph->nodes = realloc(graph->nodes,
                               graph->capacity * sizeof(struct MapCellNode));
    }
    struct MapCellNode *node = &graph->nodes[graph->numNodes];
    node->index = graph->numNodes;
    node->cell.row = row;
    node->cell.column = column;
    node->cell.layer = layer;
    node->tile = tile;
    node->neighbors = (struct MapCellNode**)malloc(sizeof(struct MapCellNode*));
    node->numNeighbors = 0;
    node->capacity = 1;
    ++graph->numNodes;
}

/**
 * Returns the node in the given graph associated with a cell.
 *
 * If the cell does not exist in the graph, NULL is returned.
 *
 * @param graph  The graph
 * @param cell   A cell in the graph
 * @return       The node associated with the cell
 */
struct MapCellNode *mapgraph_getNode(const struct MapGraph *graph,
                                     const struct MapCell *cell) {
    for (unsigned int i = 0; i < graph->numNodes; ++i) {
        if (graph->nodes[i].cell.row == cell->row &&
            graph->nodes[i].cell.column == cell->column &&
            graph->nodes[i].cell.layer == cell->layer)
            return &graph->nodes[i];
    }
    return NULL;
}

/**
 * Adds a neighbor to the given node.
 *
 * @param node      The node to which a neighbor is added
 * @param neighbor  The neighbor
 */
void mapgraph_addNeighborToNode(struct MapCellNode *node,
                                struct MapCellNode *neighbor) {
    if (node->numNeighbors == node->capacity) {
        node->capacity *= 2;
        node->neighbors = realloc(node->neighbors,
                                  node->capacity * sizeof(struct MapCellNode*));
    }
    node->neighbors[node->numNeighbors] = neighbor;
    ++node->numNeighbors;
}

/**
 * Adds a neighbor to a cell in the given graph.
 *
 * The neighbor is specified by a vector (deltaRow, deltaColumn, deltaLayer),
 * whose values are in {-1, 0, 1}. If the graph does not contain the given
 * corresponding node for the neighbor, nothing happens.
 *
 * @param graph        The graph
 * @param row          The row of the cell
 * @param column       The column of the cell
 * @param layer        The layer of the cell
 * @param deltaRow     The row variation for the neighbor
 * @param deltaColumn  The column variation for the neighbor
 * @param deltaLayer   The layer variation for the neighbor
 */
void mapgraph_addNeighbor(struct MapGraph *graph,
                          unsigned int row,
                          unsigned int column,
                          unsigned int layer,
                          int deltaRow,
                          int deltaColumn,
                          int deltaLayer) {
    struct MapCell cell = {row, column, layer};
    struct MapCell neighbor = {row + deltaRow,
                               column + deltaColumn,
                               layer + deltaLayer};
    struct MapCellNode *cellNode = mapgraph_getNode(graph, &cell);
    struct MapCellNode *neighborNode = mapgraph_getNode(graph, &neighbor);
    if (cellNode != NULL && neighborNode != NULL) {
        mapgraph_addNeighborToNode(cellNode, neighborNode);
        mapgraph_addNeighborToNode(neighborNode, cellNode);
    }
}

// --------- //
// Functions //
// --------- //

struct MapGraph mapgraph_create(const struct Map *map) {
    struct MapGraph graph;
    graph.map = map;
    graph.nodes = (struct MapCellNode*)malloc(sizeof(struct MapCellNode));
    graph.numNodes = 0;
    graph.capacity = 1;
    for (unsigned int k = 0; k < map->numLayers; ++k) {
        for (unsigned int i = 0; i < map->numRows; ++i) {
            for (unsigned int j = 0; j < map->numColumns; ++j) {
                unsigned int tileID = map->layers[k].tiles[i][j];
                if (tileID != 0 &&
                    !map_hasTileAbove(map, i, j, k)) {
                    mapgraph_addCell(&graph, &map->tiles[tileID], i, j, k);
                }
            }
        }
    }
    for (unsigned int i = 0; i < graph.numNodes; ++i) {
        const struct MapCell *cell = &graph.nodes[i].cell;
        unsigned int tileID = map->layers[cell->layer].tiles[cell->row][cell->column];
        const struct Tile *tile = &map->tiles[tileID];
        for (unsigned int j = 0; j < tile->numDirections; ++j) {
            struct Direction direction = tile->directions[j];
            struct Direction opposite = {
                -direction.deltaRow,
                -direction.deltaColumn,
                -direction.deltaLayer
            };
            struct MapCell neighbor = {
                cell->row + direction.deltaRow,
                cell->column + direction.deltaColumn,
                cell->layer + direction.deltaLayer
            };
            struct MapCellNode *neighborNode = mapgraph_getNode(&graph, &neighbor);
            if (neighborNode != NULL) {
                struct Tile *neighborTile = neighborNode->tile;
                if (map_hasDirection(neighborTile, &opposite)) {
                    mapgraph_addNeighborToNode(&graph.nodes[i], neighborNode);
                    mapgraph_addNeighborToNode(neighborNode, &graph.nodes[i]);
                }
            }
        }
    }
    return graph;
}

void mapgraph_delete(struct MapGraph *graph) {
    free(graph->nodes);
}

void mapgraph_print(const struct MapGraph *graph) {
    printf("Map graph of %d nodes\n", graph->numNodes);
    for (unsigned int i = 0; i < graph->numNodes; ++i) {
        mapgraph_printNode(&graph->nodes[i]);
    }
}

void mapgraph_toDot(const struct MapGraph *graph,
                    const char *outputFilename) {
    FILE *outputFile;
    if (strcmp(outputFilename, "stdout") == 0) {
        outputFile = stdout;
    } else {
        outputFile = fopen(outputFilename, "w");
    }
    fprintf(outputFile, "strict graph {\n");
    for (unsigned int k = 0; k < graph->map->numLayers; ++k) {
        for (unsigned int i = 0; i < graph->numNodes; ++i) {
            if (graph->nodes[i].cell.layer == k) {
                if (graph->map->layers[k].highlight[graph->nodes[i].cell.row][graph->nodes[i].cell.column]){
                    fprintf(outputFile, "  \"%d,%d,%d\" [style=filled fillcolor=yellow label=\"(%d,%d,%d)\"];\n",
                                        graph->nodes[i].cell.layer,
                                        graph->nodes[i].cell.row,
                                        graph->nodes[i].cell.column,
                                        graph->nodes[i].cell.layer,
                                        graph->nodes[i].cell.row,
                                        graph->nodes[i].cell.column);    
                } else if (graph->map->layers[k].highlight[graph->nodes[i].cell.row][graph->nodes[i].cell.column]==false) {
                    fprintf(outputFile, "  \"%d,%d,%d\" [label=\"(%d,%d,%d)\"];\n",
                                    graph->nodes[i].cell.layer,
                                    graph->nodes[i].cell.row,
                                    graph->nodes[i].cell.column,
                                    graph->nodes[i].cell.layer,
                                    graph->nodes[i].cell.row,
                                    graph->nodes[i].cell.column);
                }
            }
        }
    }

   for (unsigned int i = 0; i < graph->numNodes; ++i) {
        for (unsigned int j = 0; j < graph->nodes[i].numNeighbors; ++j) {
            struct MapCell *cell = &graph->nodes[i].cell;
            struct MapCell *neighbor = &graph->nodes[i].neighbors[j]->cell;
            fprintf(outputFile, "  \"%d,%d,%d\" -- \"%d,%d,%d\";\n",
                                cell->layer, cell->row, cell->column,
                                neighbor->layer, neighbor->row, neighbor->column);
        }
    }
 
    fprintf(outputFile, "}\n\n");
    if (strcmp(outputFilename, "stdout") != 0) {
        fclose(outputFile);
    }
}

struct MapGraphPath *mapgraph_retrievePath(struct MapCellNode **predecessors,
                                           unsigned int startIndex,
                                           unsigned int endIndex,
                                           const struct MapCell *endCell) {
    if (predecessors[endIndex] == NULL) {
        return NULL;
    } else {
        unsigned int index = endIndex;
        struct MapGraphPath *path =
            (struct MapGraphPath*)malloc(sizeof(struct MapGraphPath));
        path->head = *endCell;
        path->tail = NULL;
        while (index != startIndex) {
            struct MapGraphPath *tmpPath =
                (struct MapGraphPath*)malloc(sizeof(struct MapGraphPath));
            tmpPath->head = predecessors[index]->cell;
            tmpPath->tail = path;
            path = tmpPath;
            index = predecessors[index]->index;
        }
        return path;
    }
}

struct MapGraphPath *mapgraph_shortestPath(const struct MapGraph *graph,
                                           const struct MapCell *start,
                                           const struct MapCell *end) {
    assert(start->row    < graph->map->numRows);
    assert(start->column < graph->map->numColumns);
    assert(start->layer  < graph->map->numLayers);
    assert(end->row      < graph->map->numRows);
    assert(end->column   < graph->map->numColumns);
    assert(end->layer    < graph->map->numLayers);
    struct MapCellNode **predecessors =
        (struct MapCellNode**)malloc(graph->numNodes * sizeof(struct MapCellNode*));
    int *distance =
        (int*)malloc(graph->numNodes * sizeof(int));
    for (unsigned int i = 0; i < graph->numNodes; ++i) {
        predecessors[i] = NULL;
        distance[i] = -1;
    }
    struct MapCellNode *startNode =
        mapgraph_getNode(graph, start);
    Queue queue = queue_create();
    queue_enqueue(&queue, startNode, 0);
    distance[startNode->index] = 0;
    predecessors[startNode->index] = startNode;
    while (!queue_isEmpty(&queue)) {
        struct QueueContent content = queue_dequeue(&queue);
        for (unsigned int i = 0; i < content.cell->numNeighbors; ++i) {
            struct MapCellNode *neighbor = content.cell->neighbors[i];
            unsigned int index = neighbor->index;
            if (distance[index] == -1) {
                distance[index] = content.priority + 1;
                predecessors[index] = content.cell;
                queue_enqueue(&queue, neighbor, content.priority + 1);
            }
        }
    }
    struct MapGraphPath *path =
        mapgraph_retrievePath(predecessors,
                              startNode->index,
                              mapgraph_getNode(graph, end)->index,
                              end);
    free(predecessors);
    free(distance);
    return path;
}

void mapgraph_printPath(const struct MapGraphPath *path) {
    printf("[ ");
    while (path != NULL) {
        printf("(%d,%d,%d) ", path->head.row,
                             path->head.column,
                             path->head.layer);
        path = path->tail;
    }
    printf("]");
}

void mapgraph_deletePath(struct MapGraphPath *path) {
    if (path != NULL) {
        mapgraph_deletePath(path->tail);
        free(path);
    }
}
