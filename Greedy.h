#pragma once
#include <iostream>
#include <fstream>
#include<ctime>
#include"Graph.h"
using namespace std;

//GREEDY TSP
int greedyTSP(int matrix[100][100], int n, string cityNames[], int start, int outRoute[], int& outSize) {
bool visited[100] = { false };

    int current = start;
    visited[current] = true;
    outSize = 0;
    outRoute[outSize++] = current;
    int totalCost = 0;

    cout << "\nGreedy Route: " << cityNames[current];

    for (int count = 0; count < n - 1; count++) {
        int nearestCity = -1;
        int minDistance = 999999;

        for (int i = 0; i < n; i++) {
            if (!visited[i] && matrix[current][i] < minDistance && matrix[current][i] != 0) {
                minDistance = matrix[current][i];
                nearestCity = i;
            }
        }

        if (nearestCity != -1) {
            visited[nearestCity] = true;
            totalCost += minDistance;
            cout << " -> " << cityNames[nearestCity];
            current = nearestCity;
            outRoute[outSize++] = nearestCity;
        }
        if (nearestCity == -1) {
            cout << "Graph is disconnected! No valid route exists.\n";
            return -1;
        }
    }
    totalCost += matrix[current][0];
    cout << " -> " << cityNames[0];

    outRoute[outSize++] = start;
    return totalCost;
}


int greedyTSPList(int n, string cityNames[], int start, int outRoute[], int& outSize){
bool visited[100] = { false };
    int current = start;
    visited[current] = true;
    int totalCost = 0;
    outSize = 0;
    outRoute[outSize++] = current;

    cout << "\nGreedy (Adj List) Route: " << cityNames[current];

    for (int count = 0; count < n - 1; count++) {
        int nearestCity = -1;
        int minDistance = 999999;

        //here instead of the matrix we will be traversing the list
        Node* temp = adjList[current];
        while (temp != NULL) {
            if (!visited[temp->city] && temp->distance < minDistance) {
                minDistance = temp->distance;
                nearestCity = temp->city;
            }
            temp = temp->next;
        }

        if (nearestCity == -1) {
            cout << "\nDisconnected graph!\n";
            return -1;
        }

        visited[nearestCity] = true;
        totalCost += minDistance;
        cout << " -> " << cityNames[nearestCity];
        current = nearestCity;
        outRoute[outSize++] = nearestCity;
    }

    //after this, we will return to start and find edge back in adj list
    Node* temp = adjList[current];
    while (temp != NULL) {
        if (temp->city == start) {
            totalCost += temp->distance;
            break;
        }
        temp = temp->next;
    }

    cout << " -> " << cityNames[start];

    outRoute[outSize++] = start;
    return totalCost;
}

