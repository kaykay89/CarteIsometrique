#include <stdio.h>
#include "map.h"
#include "map_graph.h"
#include "map_loader.h"
#include "queue.h"
#include "CUnit/Basic.h"

// Global variables
struct Map *map;
struct MapGraph graph;

int initSuite() {
    map = map_loadMapFromJSONFile("data/map3x3.json");
    if (map == NULL) {
        return -1;
    } else {
        graph = mapgraph_create(map);
        return 0;
    }
}

int cleanSuite() {
    mapgraph_delete(&graph);
    map_deleteMap(map);
    return 0;
}

void test_isEmpty() {
    Queue queue = queue_create();
    CU_ASSERT(queue_isEmpty(&queue));
    queue_enqueue(&queue, &graph.nodes[0], 0);
    CU_ASSERT(!queue_isEmpty(&queue));
    queue_enqueue(&queue, &graph.nodes[1], 1);
    queue_enqueue(&queue, &graph.nodes[2], 2);
    queue_dequeue(&queue);
    queue_dequeue(&queue);
    queue_dequeue(&queue);
    CU_ASSERT(queue_isEmpty(&queue));
    queue_delete(&queue);
}

void test_numNodes() {
    Queue queue = queue_create();
    CU_ASSERT(queue.numNodes == 0);
    queue_enqueue(&queue, &graph.nodes[0], 0);
    CU_ASSERT(queue.numNodes == 1);
    queue_enqueue(&queue, &graph.nodes[1], 1);
    CU_ASSERT(queue.numNodes == 2);
    queue_enqueue(&queue, &graph.nodes[2], 2);
    CU_ASSERT(queue.numNodes == 3);
    queue_dequeue(&queue);
    CU_ASSERT(queue.numNodes == 2);
    queue_dequeue(&queue);
    CU_ASSERT(queue.numNodes == 1);
    queue_dequeue(&queue);
    CU_ASSERT(queue.numNodes == 0);
    queue_delete(&queue);
}

void test_data() {
    Queue queue = queue_create();
    struct QueueContent content;
    queue_enqueue(&queue, &graph.nodes[0], 0);
    queue_enqueue(&queue, &graph.nodes[1], 1);
    queue_enqueue(&queue, &graph.nodes[2], 2);
    content = queue_dequeue(&queue);
    CU_ASSERT(content.cell == &graph.nodes[0]);
    content = queue_dequeue(&queue);
    CU_ASSERT(content.cell == &graph.nodes[1]);
    content = queue_dequeue(&queue);
    CU_ASSERT(content.cell == &graph.nodes[2]);
    queue_delete(&queue);
}

int main() {
    CU_pSuite pSuite = NULL;
    if (CU_initialize_registry() != CUE_SUCCESS )
        return CU_get_error();

    pSuite = CU_add_suite("Testing queue", NULL, NULL);
    if (pSuite == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "Testing isEmpty", test_isEmpty) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "Testing numNodes", test_numNodes) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "Testing data", test_data) == NULL) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    return CU_get_error();
}
