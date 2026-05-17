#pragma once
#include <iostream>
#include <fstream>
#include<ctime>
#include "Graph.h"
using namespace std;

//2-OPT (Matrix/static)

int calcCost(int route[], int size, int matrix[100][100]) {
    int cost = 0;
    for (int i = 0; i < size - 1; i++)
        cost += matrix[route[i]][route[i + 1]];
    return cost;
}

void reverseSegment(int route[], int i, int k) {
    while (i < k) {
        swap(route[i], route[k]);
        i++;
        k--;
    }
}

int twoOptTSP(int matrix[100][100], int n, string cityNames[], int start, int outRoute[], int& outSize) {
    bool visited[100] = { false };
    int route[102];
    int size = 0;

    // start with a greedy route
    int current = start;
    visited[current] = true;
    route[size++] = current;

    for (int count = 0; count < n - 1; count++) {
        int nearestCity = -1;
        int minDistance = 999999;

        for (int i = 0; i < n; i++) {
            if (!visited[i] && matrix[current][i] != 0 && matrix[current][i] < minDistance) {
                minDistance = matrix[current][i];
                nearestCity = i;
            }
        }

        if (nearestCity == -1) {
            cout << "\n2-Opt: Disconnected graph!\n";
            return -1;
        }

        visited[nearestCity] = true;
        route[size++] = nearestCity;
        current = nearestCity;
    }
    route[size++] = start; // return to start

    // keep swapping until no improvement
    bool improved = true;
    while (improved) {
        improved = false;

        for (int i = 1; i <= size - 3; i++) {
            for (int k = i + 1; k <= size - 2; k++) {

                int oldCost = matrix[route[i - 1]][route[i]] + matrix[route[k]][route[k + 1]];
                int newCost = matrix[route[i - 1]][route[k]] + matrix[route[i]][route[k + 1]];

                if (newCost < oldCost) {
                    reverseSegment(route, i, k);
                    improved = true;
                }
            }
        }
    }

    outSize = 0;
    for (int i = 0; i < size; i++)
        outRoute[outSize++] = route[i];

    cout << "\n2-Opt (Matrix) Route: ";
    for (int i = 0; i < size - 1; i++)
        cout << cityNames[route[i]] << " -> ";
    cout << cityNames[route[size - 1]];

    return calcCost(outRoute, outSize, matrix);
}


//2-OPT (ADJ LIST/Dynamic)

int getDistAdj(int from, int to) {
    Node* temp = adjList[from];
    while (temp != NULL) {
        if (temp->city == to)
            return temp->distance;
        temp = temp->next;
    }
    return 999999; // no edge
}

int calcCostList(int route[], int size) {
    int cost = 0;
    for (int i = 0; i < size - 1; i++)
        cost += getDistAdj(route[i], route[i + 1]);
    return cost;
}

int twoOptTSPList(int n, string cityNames[], int start, int outRoute[], int& outSize) {
    bool visited[100] = { false };
    int route[102];
    int size = 0;

    // start with a greedy route using adj list
    int current = start;
    visited[current] = true;
    route[size++] = current;

    for (int count = 0; count < n - 1; count++) {
        int nearestCity = -1;
        int minDistance = 999999;

        Node* temp = adjList[current];
        while (temp != NULL) {
            if (!visited[temp->city] && temp->distance < minDistance) {
                minDistance = temp->distance;
                nearestCity = temp->city;
            }
            temp = temp->next;
        }

        if (nearestCity == -1) {
            cout << "\n2-Opt (List): Disconnected graph!\n";
            return -1;
        }

        visited[nearestCity] = true;
        route[size++] = nearestCity;
        current = nearestCity;
    }
    route[size++] = start; // return to start

    // keep swapping until no improvement
    bool improved = true;
    while (improved) {
        improved = false;

        for (int i = 1; i <= size - 3; i++) {
            for (int k = i + 1; k <= size - 2; k++) {

                int oldCost = getDistAdj(route[i - 1], route[i]) + getDistAdj(route[k], route[k + 1]);
                int newCost = getDistAdj(route[i - 1], route[k]) + getDistAdj(route[i], route[k + 1]);

                if (newCost < oldCost) {
                    reverseSegment(route, i, k);
                    improved = true;
                }
            }
        }
    }

    outSize = 0;
    for (int i = 0; i < size; i++)
        outRoute[outSize++] = route[i];

    cout << "\n2-Opt (Adj List) Route: ";
    for (int i = 0; i < size - 1; i++)
        cout << cityNames[route[i]] << " -> ";
    cout << cityNames[route[size - 1]];

    return calcCostList(outRoute, outSize);
}