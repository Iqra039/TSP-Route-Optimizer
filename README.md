# 🗺️ TSP Route Optimizer — Pakistan

A **Data Structures & Algorithms semester project** built in C++ with an SFML GUI. Solves the Travelling Salesperson Problem (TSP) for major Pakistani cities, visualized on a real Pakistan map.

> **Real-world use case:** A courier departs from Karachi, delivers to Lahore, Islamabad, Peshawar, and Quetta, and returns home — this system finds the shortest possible route, saving fuel and time.

---

## 🧠 Algorithms & Data Structures

| Algorithm | Representation | Complexity | Solution |
|-----------|---------------|------------|---------|
| Greedy (Nearest Neighbor) | Adjacency Matrix | O(n²) | Approximate |
| Greedy (Nearest Neighbor) | Adjacency List | O(n²) | Approximate |
| Brute Force (Exhaustive) | Adjacency Matrix | O(n!) | Optimal |
| Brute Force (Exhaustive) | Adjacency List | O(n!) | Optimal |

---

## ✨ Features

- Real Pakistan map background with geographically placed city nodes
- Route drawn in red with numbered step markers showing travel order
- Performance screen (cost, time, complexity, memory)
- Comparison screen running all 4 algorithms side by side
- Edge case handling: disconnected graphs, single city, n > 10

---

## ⚙️ Setup

1. Install **SFML 2.6** and link it in Visual Studio
2. Place these files in your `x64/Debug/` folder:
   ```
   input.txt   pakistan_map.jpg   arial.ttf
   ```
3. Build and run

---

## 📂 Files

```
main.cpp        — SFML GUI (5 screens)
Graph.h         — Graph class + adjacency list
Greedy.h        — Greedy TSP (matrix + list)
BruteForce.h    — Brute Force TSP (matrix + list)
input.txt       — City names and distance matrix
```

---

## 👩‍💻 Author

**Iqra Agha** — BSCS-3, Bahria University Islamabad
DSA Semester Project, 2026
