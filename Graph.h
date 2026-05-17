#pragma once
#include <iostream>
#include <fstream>
#include<ctime>
using namespace std;


//ADJ LIST
struct Node {
    int city;
    int distance;
    Node* next;
};

Node* adjList[100];

//GRAPH CLASS
class Graph {
public:
    int n;
    int matrix[100][100];
    string cityNames[100];

    void loadFromFile(string filename) {
        ifstream file(filename);

        if (!file) {
            cout << "Error opening file!" << endl;
            return;
        }

        file >> n;  
        if (n == 1) {
            cout << "Only one city. No route needed.\n";
            return;
        }
        for (int i = 0; i < n; i++) {
            file >> cityNames[i];  
        }
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                file >> matrix[i][j];  //read adjacency matrix
            }
        }

        file.close();
    }

    void displayMatrix() {
        cout << "\nAdjacency Matrix:\n";

        // Column headers
        cout << "\t";
        for (int i = 0; i < n; i++) {
            cout << cityNames[i] << "\t";
        }
        cout << endl;

        // Rows
        for (int i = 0; i < n; i++) {
            cout << cityNames[i] << "\t";
            for (int j = 0; j < n; j++) {
                cout << matrix[i][j] << "\t";
            }
            cout << endl;
        }
    }
};


void addEdge(int u, int v, int dist) {
    Node* newNode = new Node;
    newNode->city = v;
    newNode->distance = dist;
    newNode->next = adjList[u];
    adjList[u] = newNode;
}

void buildAdjList(int matrix[100][100], int n) {
    for (int i = 0; i < n; i++) {
        adjList[i] = NULL;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] != 0) {
                addEdge(i, j, matrix[i][j]);
            }
        }
    }
}

void displayAdjList(int n, string cityNames[]) {
    cout << "\nAdjacency List:\n";

    for (int i = 0; i < n; i++) {
        cout << cityNames[i] << " -> ";
        Node* temp = adjList[i];

        while (temp != NULL) {
            cout << "(" << cityNames[temp->city] << "," << temp->distance << ") ";
            temp = temp->next;
        }
        cout << endl;
    }
}