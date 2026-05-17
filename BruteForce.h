#pragma once
#include <iostream>
#include <fstream>
#include<ctime>
#include "Graph.h"
using namespace std;

//BRUTE FORCE Matrix/static implementation
int minCost = 999999;
int bestPath[100];

void tspBrute(int matrix[100][100], int n, int path[],bool visited[],int level,int currentCost) {
    if (level == n) {
        int totalCost = currentCost + matrix[path[level - 1]][path[0]];

        if (totalCost < minCost) {
            minCost = totalCost;

            for (int i = 0; i < n; i++) {
                bestPath[i] = path[i];
            }
        }
        return;
    }
    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            visited[i] = true;
            path[level] = i;

            tspBrute(matrix, n, path, visited, level + 1, currentCost + matrix[path[level - 1]][i]);

            visited[i] = false;
        }
    }
}

int runBruteForce(int matrix[100][100], int n, string cityNames[], int start, int outRoute[], int& outSize) {
    if (n > 10) {
        cout << "Too many cities for brute force!\n";
        return -1;
    }
    int path[100];
    bool visited[100] = { false };

    path[0] = start;
    visited[start] = true;

    tspBrute(matrix, n, path, visited, 1, 0);
    outSize = 0;
    for (int i = 0; i < n; i++)
        outRoute[outSize++] = bestPath[i];
    outRoute[outSize++] = bestPath[0];
    cout << "\nBrute Force Optimal Route: ";
    for (int i = 0; i < n; i++) {
        cout << cityNames[bestPath[i]] << " -> ";
    }
    cout << cityNames[bestPath[0]];

    return minCost;
}

//BRUTE FORCE (ADJ LIST/Dynamic implementation)
int minCostList = 999999;
int bestPathList[100];

// this function gets the distance between two cities from adj list
int getDistance(int from, int to) {
    Node* temp = adjList[from];
    while (temp != NULL) {
        if (temp->city == to)
            return temp->distance;
        temp = temp->next;
    }
    return 0; //and if there is no edge it will return 0
}

void tspBruteList(int n,
    int path[],
    bool visited[],
    int level,
    int currentCost) {

    if (level == n) {
        int returnDist = getDistance(path[level - 1], path[0]);
        int totalCost = currentCost + returnDist;

        if (totalCost < minCostList) {
            minCostList = totalCost;
            for (int i = 0; i < n; i++)
                bestPathList[i] = path[i];
        }
        return;
    }

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            int dist = getDistance(path[level - 1], i);
            if (dist != 0) {      //this will only traverse the edges that exist
                visited[i] = true;
                path[level] = i;

                tspBruteList(n, path, visited,
                    level + 1,
                    currentCost + dist);

                visited[i] = false;
            }
        }
    }
}

int runBruteForceList(int n, string cityNames[], int start, int outRoute[], int& outSize) {
    if (n > 10) {
        cout << "Too many cities for brute force!\n";
        return -1;
    }
    minCostList = 999999;
    int path[100];
    bool visited[100] = { false };

    path[0] = start;
    visited[start] = true;

    tspBruteList(n, path, visited, 1, 0);

    outSize = 0;
    for (int i = 0; i < n; i++)
        outRoute[outSize++] = bestPathList[i];
    outRoute[outSize++] = bestPathList[0];

    cout << "\nBrute Force (Adj List) Optimal Route: ";
    for (int i = 0; i < n; i++)
        cout << cityNames[bestPathList[i]] << " -> ";
    cout << cityNames[bestPathList[0]];
    return minCostList;
}