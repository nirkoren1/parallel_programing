#include <stdio.h>
#include <stdlib.h>
#include "graph.h"
#include "parallel_pagerank.h"
#include <time.h>

#define D 0.15 // damping factor


void initializeRanks(float *ranks, int N) {
    for (int i = 0; i < N; i++) {
        ranks[i] = 1.0 / N;
    }
}


void PageRank(Graph *graph, int iterations, float* ranks) {
    int N = graph->numVertices;
    float *newRanks = (float *)malloc(N * sizeof(float));
    int* outlinkes = (int*)calloc(N, sizeof(int));
    initializeRanks(ranks, N);
    
    //outlinks calculations
    
    for(int i = 0; i < N; i++) {
        node* v = graph->adjacencyLists[i];
        while (v!=NULL) {
            outlinkes[i]++;
            v = v->next;
        }
    }
    
    for (int iter = 0; iter < iterations; iter++) {
        //calculate nodes with outlinks to i
        for (int i = 0; i < N; i++) {
            vertex* out2i = (vertex*)malloc(N * sizeof(vertex));
            
            for(int j = 0; j < N; j++) {
                if( j == i) continue;
                node* v = graph->adjacencyLists[j];
                while (v != NULL) {
                    if (v->v == i) {
                        out2i[j] = 1;
                        break;
                    }
                    v = v->next;
                }
            }
            
            //calculate i rank
            double sumA = 0.0;
            double sumB = 0.0;
            for(int j = 0 ; j < N; j++) {
                if(out2i[j] == 1) {
                    sumA += ranks[j]/outlinkes[j];
                } else if(outlinkes[j] == 0) {
                    sumB += ranks[j]/N;
                }
            }
            newRanks[i] = D/N +(1-D)*(sumA+sumB);
        }

        for (int i = 0; i < N; i++) {
            ranks[i] = newRanks[i];
        }
    }

    free(newRanks);
}

int main(void) {
    int N = 500; // number of nodes
    int iterations = 100; // number of iterations

    clock_t start, end;
    double cpu_time_used;

    // Initialize the graph
    Graph *graph = createGraph(N);

    // Example graph structure
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (i == j || i == 5 || j == 6)
                continue;
            addEdge(graph, i, j);
        }
    }
    

    // addEdge(graph, 0, 1);
    // addEdge(graph, 0, 3);
    // addEdge(graph, 2, 0);
    // addEdge(graph, 2, 4);
    // addEdge(graph, 3, 1);
    // addEdge(graph, 3, 2);
    // addEdge(graph, 3, 4);
    // addEdge(graph, 4, 1);
    // addEdge(graph, 4, 2);
    // addEdge(graph, 4, 3);


    // Calculate PageRank
    float *ranks = (float *)malloc(N * sizeof(float));
    start = clock();
    PageRank(graph, iterations, ranks);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    


    printf("Serial results\n");
    printf("Time taken for serial part: %f seconds\n", cpu_time_used);
    // Print the ranks
    // for (int i = 0; i < N; i++) {
    //     printf("Rank of node %d: %f\n", i, ranks[i]);
    // }

    start = clock();
    PageRank_P(graph, iterations, ranks);
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Parallel results\n");
    printf("Time taken for parallel part: %f seconds\n", cpu_time_used);
    // Print the ranks
    // for (int i = 0; i < N; i++) {
    //     printf("Rank of node %d: %f\n", i, ranks[i]);
    // }
    
    // Free allocated memory
    for (int i = 0; i < N; i++) {
        node *adjList = graph->adjacencyLists[i];
        while (adjList != NULL) {
            node *temp = adjList;
            adjList = adjList->next;
            free(temp);
        }
    }
    

    free(ranks);
    free(graph->adjacencyLists);
    free(graph);

    return 0;
}
