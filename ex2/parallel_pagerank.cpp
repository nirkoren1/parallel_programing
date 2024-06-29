// 316443902 Nir Koren

#include "parallel_pagerank.h"
#include <vector>
#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "threadpool.h"

#define D 0.15 
int Threads_N;


struct ThreadData {
    int start, end;
    const std::vector<float>* ranks;
    const std::vector<std::vector<float>>* matrix;
    std::vector<float>* result;
    float alpha, beta;
    int N;
};

void print_matrix(int N, std::vector<std::vector<float>> matrix) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

void transpose(std::vector<std::vector<float>>& matrix, std::vector<std::vector<float>> matrix_b, int N) {
    for(int i = 0; i < N; i++) {
        for(int j = 0; j < N; j++) {
            matrix[i][j] = matrix_b[j][i];
        }
    }
}

void init_matrix(Graph *g, int N, std::vector<std::vector<float>>& matrix) {
    for(int i = 0; i < N; i++) {
        node* v = g->adjacencyLists[i];
        int outlinkes = 0;
        while (v!=NULL) {
            outlinkes++;
            matrix[i][v->v] = 1.0;
            v = v->next;
        }
        if (outlinkes == 0) {
            for (int j = 0; j < N; j++)
            {
                matrix[i][j] = 1.0 / N;
            }
        } else {
            for (int j = 0; j < N; j++)
            {
                matrix[i][j] /= outlinkes;
            }
        }
    }
    transpose(matrix, matrix, N);
}




void partialMatMul(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    for (int i = data->start; i < data->end; ++i) {
        double sum = 0.0;
        for (int j = 0; j < data->N; ++j) {
            sum += data->ranks->at(j) * data->matrix->at(i)[j];
        }
        data->result->at(i) = data->alpha + data->beta*sum;
    }
}

std::vector<float> matmul_r(const std::vector<float>& ranks, const std::vector<std::vector<float>> matrix, int N, threadpool tpool, float alpha, float beta) {
    std::vector<float> result(N, 0);
    int numThreads = Threads_N;
    std::vector<ThreadData> threadData(numThreads);

    int chunkSize = N / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        threadData[i].start = i * chunkSize;
        threadData[i].end = (i == numThreads - 1) ? N : (i + 1) * chunkSize;
        threadData[i].ranks = &ranks;
        threadData[i].matrix = &matrix;
        threadData[i].result = &result;
        threadData[i].alpha = alpha;
        threadData[i].beta = beta;
        threadData[i].N = N;

        thpool_add_work(tpool, partialMatMul, (void*)&threadData[i]);
    }

    thpool_wait(tpool);

    return result;
}


void PageRank(Graph *g, int n, float* ranks) {
    int N = g->numVertices;
    float initialValue = 1.0 / N;
    float alpha = D/N;
    float beta = 1 - D;
    Threads_N = 4;
    threadpool tpool = thpool_init(Threads_N);
    std::vector<float> newRanks(N, initialValue);
    std::vector<int> outlinks(N);
    std::vector<std::vector<float>> matrix(N, std::vector<float>(N, 0.0));
    init_matrix(g, N, matrix);

    for (int i = 0; i < n; i++) {
        newRanks = matmul_r(newRanks, matrix, N, tpool, alpha, beta);
    }
    

    for (int i = 0; i < N; i++) {
            ranks[i] = newRanks[i];
        }
}