// 316443902 Nir Koren
// this code was provided in the Tirgul 5, as said in the instructions: "you may use the implementation showed in the sessions or a one of your own."

#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef int vertex;
typedef pthread_mutex_t mutex;

struct node {
    vertex v;
    struct node *next;
};

typedef struct node node;

struct Graph {
    unsigned int numVertices;
    node **adjacencyLists;
};

typedef struct Graph Graph;

node * createNode(vertex v);

void addEdge(Graph *graph, vertex source, vertex destination);

Graph * createGraph(int vertices);

#endif
