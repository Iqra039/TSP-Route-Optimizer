#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include "Graph.h"
#include "Greedy.h"
#include "BruteForce.h"
#include "TwoOpt.h"
using namespace std;

//this will create the rectangle button
sf::RectangleShape makeBtn(float x, float y, float w, float h, sf::Color col) {
    sf::RectangleShape btn(sf::Vector2f(w, h));
    btn.setPosition(x, y);
    btn.setFillColor(col);
    btn.setOutlineColor(sf::Color(80, 80, 80));
    btn.setOutlineThickness(1);
    return btn;
}
//creates text object
sf::Text makeText(sf::Font& font, string s, float x, float y, int size, sf::Color col) {
    sf::Text t;
    t.setFont(font);
    t.setString(s);
    t.setCharacterSize(size);
    t.setFillColor(col);
    t.setPosition(x, y);
    return t;
}

//checks the mouse collision with the button
bool isClicked(sf::RectangleShape& btn, sf::Vector2i mouse) {
    return btn.getGlobalBounds().contains((float)mouse.x, (float)mouse.y);
}


/*- - - - - - - - - MAIN- - - - - - - -*/
int main() {
    // load graph
    Graph g;
    g.loadFromFile("input.txt");
    buildAdjList(g.matrix, g.n);

    // load font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) return -1;

    //LOAD MAP TEXTURE
    sf::Texture mapTexture;
    bool mapLoaded = mapTexture.loadFromFile("pakistan_map.jpg");
    if (!mapLoaded) mapLoaded = mapTexture.loadFromFile("pakistan_map.png");
    sf::Sprite mapSprite;

    float mapOffsetX = 215.f;
    float mapOffsetY = 0.f;
    float mapW = 785.f;
    float mapH = 700.f;
    if (mapLoaded) {
        mapSprite.setTexture(mapTexture);
        mapSprite.setPosition(mapOffsetX, mapOffsetY);
        mapSprite.setScale(
            mapW / (float)mapTexture.getSize().x,
            mapH / (float)mapTexture.getSize().y
        );
    }

    sf::RenderWindow window(sf::VideoMode(1000, 700), "TSP Delivery Route Optimizer - Pakistan");
    window.setFramerateLimit(60);

    float cityX[100], cityY[100];

    struct CityPos { float px; float py; };
    CityPos positions[] = {
        { 0.390f, 0.930f },  // 0: Karachi
        { 0.758f, 0.370f },  // 1: Lahore    
        { 0.720f, 0.220f },  // 2: Islamabad
        { 0.660f, 0.165f },  // 3: Peshawar
        { 0.370f, 0.510f },  // 4: Quetta
    };

    for (int i = 0; i < g.n && i < 5; i++) {
        cityX[i] = mapOffsetX + mapW * positions[i].px;
        cityY[i] = mapH * positions[i].py;
    }

    // ---- STATE ----
    int screen = 0;  // 0=menu 1=city select 2=result 3=perf 4=compare
    int selectedAlgo = -1;
    int selectedCity = -1;
    int route[110];
    int routeSize = 0;
    int routeCost = 0;
    double lastTime = 0;
    string algoName = "";
    string statusMsg = "";

    // ---- COLORS ----
    sf::Color btnBlue(25, 25, 112);
    sf::Color btnRed(102, 0, 0);
    sf::Color btnPink(180, 80, 120);
    sf::Color btnGrey(100, 100, 100);
    sf::Color btnPurp(120, 70, 160);
    sf::Color btnTeal(30, 130, 130);
    sf::Color btnOrange(180, 90, 20);

    // ---- BUTTONS ----
    sf::RectangleShape btnGreedy = makeBtn(15, 100, 185, 42, btnBlue);
    sf::RectangleShape btnBrute = makeBtn(15, 152, 185, 42, btnBlue);
    sf::RectangleShape btnList = makeBtn(15, 204, 185, 42, btnTeal);
    sf::RectangleShape btnBruteList = makeBtn(15, 256, 185, 42, btnTeal);
    sf::RectangleShape btn2Opt = makeBtn(15, 308, 185, 42, btnPurp);
    sf::RectangleShape btn2OptList = makeBtn(15, 360, 185, 42, btnPurp);
    sf::RectangleShape btnClear = makeBtn(15, 422, 185, 42, btnRed);
    sf::RectangleShape btnCompare = makeBtn(15, 474, 185, 42, btnOrange);

    sf::RectangleShape cityBtns[100];
    for (int i = 0; i < g.n; i++)
        cityBtns[i] = makeBtn(15, 100 + i * 52, 185, 44, btnPink);

    sf::RectangleShape btnBack = makeBtn(15, 645, 185, 40, btnGrey);
    sf::RectangleShape btnPerf = makeBtn(15, 595, 185, 40, btnPurp);

    // ---- COMPARISON DATA ----
    int    cmpCost[6] = { 0, 0, 0, 0, 0, 0 };
    double cmpTime[6] = { 0, 0, 0, 0, 0, 0 };
    bool   cmpDone[6] = { false, false, false, false, false, false };
    string algoLabels[6] = { "Greedy (Matrix)", "Brute Force (Matrix)", "Greedy (List)", "Brute Force (List)", "2-Opt (Matrix)", "2-Opt (List)" };
    string algoComplexity[6] = { "O(n^2)", "O(n!)", "O(n^2)", "O(n!)", "O(n^2*iter)", "O(n^2*iter)" };
    string algoMemory[6] = { "O(n^2)", "O(n!)", "O(n)",   "O(n)",  "O(n)",        "O(n)" };

    // ================= MAIN LOOP =================
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                sf::Vector2i mouse = sf::Mouse::getPosition(window);

                // ---- SCREEN 0: Main Menu ----
                if (screen == 0) {
                    if (isClicked(btnGreedy, mouse)) { selectedAlgo = 0; algoName = "Greedy (Matrix)";      screen = 1; }
                    if (isClicked(btnBrute, mouse)) { selectedAlgo = 1; algoName = "Brute Force (Matrix)"; screen = 1; }
                    if (isClicked(btnList, mouse)) { selectedAlgo = 2; algoName = "Greedy (List)";        screen = 1; }
                    if (isClicked(btnBruteList, mouse)) { selectedAlgo = 3; algoName = "Brute Force (List)";   screen = 1; }
                    if (isClicked(btn2Opt, mouse)) { selectedAlgo = 4; algoName = "2-Opt (Matrix)";       screen = 1; }
                    if (isClicked(btn2OptList, mouse)) { selectedAlgo = 5; algoName = "2-Opt (List)";         screen = 1; }

                    if (isClicked(btnClear, mouse)) {
                        routeSize = 0; routeCost = 0;
                        algoName = ""; statusMsg = ""; lastTime = 0;
                        for (int i = 0; i < 6; i++) { cmpCost[i] = 0; cmpTime[i] = 0; cmpDone[i] = false; }
                    }

                    if (isClicked(btnCompare, mouse)) {
                        int tmpRoute[110]; int tmpSize = 0;
                        clock_t t1, t2;

                        // Greedy Matrix
                        t1 = clock();
                        cmpCost[0] = greedyTSP(g.matrix, g.n, g.cityNames, 0, tmpRoute, tmpSize);
                        t2 = clock();
                        cmpTime[0] = (double)(t2 - t1) / CLOCKS_PER_SEC;
                        cmpDone[0] = (cmpCost[0] != -1);

                        // Brute Force Matrix
                        if (g.n <= 10) {
                            minCost = 999999;
                            t1 = clock();
                            cmpCost[1] = runBruteForce(g.matrix, g.n, g.cityNames, 0, tmpRoute, tmpSize);
                            t2 = clock();
                            cmpTime[1] = (double)(t2 - t1) / CLOCKS_PER_SEC;
                            cmpDone[1] = (cmpCost[1] != -1);
                        }
                        else { cmpCost[1] = -1; cmpDone[1] = false; }

                        // Greedy List
                        t1 = clock();
                        cmpCost[2] = greedyTSPList(g.n, g.cityNames, 0, tmpRoute, tmpSize);
                        t2 = clock();
                        cmpTime[2] = (double)(t2 - t1) / CLOCKS_PER_SEC;
                        cmpDone[2] = (cmpCost[2] != -1);

                        // Brute Force List
                        if (g.n <= 10) {
                            minCostList = 999999;
                            t1 = clock();
                            cmpCost[3] = runBruteForceList(g.n, g.cityNames, 0, tmpRoute, tmpSize);
                            t2 = clock();
                            cmpTime[3] = (double)(t2 - t1) / CLOCKS_PER_SEC;
                            cmpDone[3] = (cmpCost[3] != -1);
                        }
                        else { cmpCost[3] = -1; cmpDone[3] = false; }

                        // 2-Opt Matrix
                        t1 = clock();
                        cmpCost[4] = twoOptTSP(g.matrix, g.n, g.cityNames, 0, tmpRoute, tmpSize);
                        t2 = clock();
                        cmpTime[4] = (double)(t2 - t1) / CLOCKS_PER_SEC;
                        cmpDone[4] = (cmpCost[4] != -1);

                        // 2-Opt List
                        t1 = clock();
                        cmpCost[5] = twoOptTSPList(g.n, g.cityNames, 0, tmpRoute, tmpSize);
                        t2 = clock();
                        cmpTime[5] = (double)(t2 - t1) / CLOCKS_PER_SEC;
                        cmpDone[5] = (cmpCost[5] != -1);

                        screen = 4;
                    }
                }

                // ---- SCREEN 1: City Selection ----
                else if (screen == 1) {
                    for (int i = 0; i < g.n; i++) {
                        if (isClicked(cityBtns[i], mouse)) {
                            selectedCity = i;
                            minCost = 999999; minCostList = 999999;

                            clock_t t1 = clock();
                            if (selectedAlgo == 0) routeCost = greedyTSP(g.matrix, g.n, g.cityNames, selectedCity, route, routeSize);
                            else if (selectedAlgo == 1) routeCost = runBruteForce(g.matrix, g.n, g.cityNames, selectedCity, route, routeSize);
                            else if (selectedAlgo == 2) routeCost = greedyTSPList(g.n, g.cityNames, selectedCity, route, routeSize);
                            else if (selectedAlgo == 3) routeCost = runBruteForceList(g.n, g.cityNames, selectedCity, route, routeSize);
                            else if (selectedAlgo == 4) routeCost = twoOptTSP(g.matrix, g.n, g.cityNames, selectedCity, route, routeSize);
                            else if (selectedAlgo == 5) routeCost = twoOptTSPList(g.n, g.cityNames, selectedCity, route, routeSize);
                            clock_t t2 = clock();
                            lastTime = (double)(t2 - t1) / CLOCKS_PER_SEC;

                            cmpCost[selectedAlgo] = routeCost;
                            cmpTime[selectedAlgo] = lastTime;
                            cmpDone[selectedAlgo] = (routeCost != -1);

                            statusMsg = (routeCost == -1) ? "Error: disconnected or too many cities!" : "";
                            screen = 2;
                        }
                    }
                    if (isClicked(btnBack, mouse)) screen = 0;
                }

                else if (screen == 2) {
                    if (isClicked(btnBack, mouse)) screen = 0;
                    if (isClicked(btnPerf, mouse)) screen = 3;
                }
                else if (screen == 3) {
                    if (isClicked(btnBack, mouse)) screen = 2;
                }
                else if (screen == 4) {
                    if (isClicked(btnBack, mouse)) screen = 0;
                }
            }
        }

        window.clear(sf::Color(245, 245, 245));

        // ---- LEFT PANEL ----
        sf::RectangleShape panel(sf::Vector2f(215, 700));
        panel.setFillColor(sf::Color(30, 30, 50));
        window.draw(panel);
        sf::Vertex divider[] = {
            sf::Vertex(sf::Vector2f(215, 0),   sf::Color(80, 80, 120)),
            sf::Vertex(sf::Vector2f(215, 700), sf::Color(80, 80, 120))
        };
        window.draw(divider, 2, sf::Lines);

        // ---- MAP BACKGROUND ----
        if (mapLoaded) window.draw(mapSprite);

        // ---- ALL EDGES (faint white) ----
        for (int i = 0; i < g.n; i++)
            for (int j = i + 1; j < g.n; j++)
                if (g.matrix[i][j] != 0) {
                    sf::Vertex e[] = {
                        sf::Vertex(sf::Vector2f(cityX[i], cityY[i]), sf::Color(255, 255, 255, 70)),
                        sf::Vertex(sf::Vector2f(cityX[j], cityY[j]), sf::Color(255, 255, 255, 70))
                    };
                    window.draw(e, 2, sf::Lines);
                }

        //ROUTE (bright red, with numbered midpoint markers)
        if ((screen == 2 || screen == 3) && routeSize > 0 && routeCost != -1) {
            for (int i = 0; i < routeSize - 1; i++) {
                int a = route[i], b = route[i + 1];

                // Draw thick line
                for (int off = -1; off <= 1; off++) {
                    sf::Vertex rl[] = {
                        sf::Vertex(sf::Vector2f(cityX[a], cityY[a] + off), sf::Color(255, 50, 50, 230)),
                        sf::Vertex(sf::Vector2f(cityX[b], cityY[b] + off), sf::Color(255, 50, 50, 230))
                    };
                    window.draw(rl, 2, sf::Lines);
                }

                float midX = (cityX[a] + cityX[b]) / 2;
                float midY = (cityY[a] + cityY[b]) / 2;

                // Step number circle at midpoint
                sf::CircleShape stepCircle(7);
                stepCircle.setFillColor(sf::Color(57, 255, 20, 230));  // Neon green
                stepCircle.setOutlineColor(sf::Color::White);  // Black outline
                stepCircle.setOutlineThickness(1.5f);
                stepCircle.setPosition(midX - 7, midY - 7);
                window.draw(stepCircle);

                // Step number text inside circle (BLACK text)
                window.draw(makeText(font, to_string(i + 1), midX - 4, midY - 8, 10, sf::Color::Black));  // Changed text color to Black, size 10

                // Distance label just below the circle
                string lbl = to_string(g.matrix[a][b]) + "km";
                sf::RectangleShape lblBg(sf::Vector2f(lbl.size() * 7.0f + 4, 14));
                lblBg.setPosition(midX - lblBg.getSize().x / 2, midY + 12);
                lblBg.setFillColor(sf::Color(0, 0, 0, 170));
                window.draw(lblBg);
                window.draw(makeText(font, lbl,
                    midX - lblBg.getSize().x / 2 + 2, midY + 13,
                    10, sf::Color(255, 210, 210)));
            }
        }

        // ---- CITY NODES ----
        for (int i = 0; i < g.n; i++) {
            bool isStart = (screen == 2 || screen == 3) && i == selectedCity;
            bool inRoute = false;
            if (screen == 2 || screen == 3)
                for (int r = 0; r < routeSize; r++)
                    if (route[r] == i) { inRoute = true; break; }

            // Glow
            sf::CircleShape glow(16);
            glow.setFillColor(sf::Color(255, 255, 255, 50));
            glow.setPosition(cityX[i] - 16, cityY[i] - 16);
            window.draw(glow);

            // Dot
            sf::CircleShape dot(11);
            dot.setFillColor(isStart ? sf::Color(255, 60, 60) :
                inRoute ? sf::Color(255, 165, 30) :
                sf::Color(30, 144, 255));
            dot.setOutlineColor(sf::Color::White);
            dot.setOutlineThickness(2);
            dot.setPosition(cityX[i] - 11, cityY[i] - 11);
            window.draw(dot);

            // Label background + text
            string name = g.cityNames[i];
            float bgW = name.size() * 7.5f + 8;
            sf::RectangleShape nameBg(sf::Vector2f(bgW, 17));
            nameBg.setPosition(cityX[i] + 14, cityY[i] - 9);
            nameBg.setFillColor(sf::Color(0, 0, 0, 170));
            window.draw(nameBg);
            window.draw(makeText(font, name, cityX[i] + 16, cityY[i] - 8, 12, sf::Color::White));
        }

        // SCREEN 0: MAIN MENU

        if (screen == 0) {
            window.draw(makeText(font, "TSP Delivery", 18, 15, 16, sf::Color(200, 200, 255)));
            window.draw(makeText(font, "Route Optimizer", 18, 36, 16, sf::Color(200, 200, 255)));
            window.draw(makeText(font, "Pakistan", 18, 58, 12, sf::Color(100, 200, 200)));

            sf::RectangleShape sep(sf::Vector2f(185, 1));
            sep.setPosition(15, 78); sep.setFillColor(sf::Color(70, 70, 100));
            window.draw(sep);

            window.draw(btnGreedy);
            window.draw(makeText(font, "Greedy", 30, 113, 13, sf::Color::White));
            window.draw(btnBrute);
            window.draw(makeText(font, "Brute Force", 30, 165, 13, sf::Color::White));

            window.draw(btnList);
            window.draw(makeText(font, "Greedy (Dynamic)", 30, 217, 13, sf::Color::White));
            window.draw(btnBruteList);
            window.draw(makeText(font, "Brute Force (Dynamic)", 30, 269, 13, sf::Color::White));

            window.draw(btn2Opt);
            window.draw(makeText(font, "2-Opt (Matrix)", 30, 321, 13, sf::Color::White));
            window.draw(btn2OptList);
            window.draw(makeText(font, "2-Opt (List)", 30, 373, 13, sf::Color::White));
            window.draw(btnClear);
            window.draw(makeText(font, "Clear Route", 42, 435, 13, sf::Color::White));
            window.draw(btnCompare);
            window.draw(makeText(font, "Compare All", 42, 487, 13, sf::Color::White));

            if (algoName != "" && routeCost > 0) {
                sf::RectangleShape sep2(sf::Vector2f(185, 1));
                sep2.setPosition(15, 528); sep2.setFillColor(sf::Color(70, 70, 100));
                window.draw(sep2);
                window.draw(makeText(font, "Last Run:", 18, 534, 11, sf::Color(140, 140, 180)));
                window.draw(makeText(font, algoName, 18, 549, 12, sf::Color::White));
                window.draw(makeText(font, "Cost: " + to_string(routeCost) + " km", 18, 566, 12, sf::Color(100, 220, 100)));
            }
        }
        // SCREEN 1: CITY SELECTION
        else if (screen == 1) {
            window.draw(makeText(font, algoName, 18, 15, 13, sf::Color(200, 200, 255)));
            window.draw(makeText(font, "Select start city:", 18, 60, 12, sf::Color(160, 160, 200)));
            for (int i = 0; i < g.n; i++) {
                window.draw(cityBtns[i]);
                window.draw(makeText(font, g.cityNames[i], 30, 115 + i * 52, 13, sf::Color::White));
            }
            window.draw(btnBack);
            window.draw(makeText(font, "< Back", 65, 657, 13, sf::Color::White));
        }

        // SCREEN 2: RESULT
        else if (screen == 2) {
            window.draw(makeText(font, algoName, 18, 15, 13, sf::Color(200, 200, 255)));
            if (statusMsg != "") {
                window.draw(makeText(font, statusMsg, 18, 55, 11, sf::Color(220, 80, 80)));
            }
            else {
                window.draw(makeText(font, "Start: " + g.cityNames[selectedCity], 18, 52, 12, sf::Color(160, 160, 200)));
                window.draw(makeText(font, "Cost:  " + to_string(routeCost) + " km", 18, 68, 13, sf::Color(100, 220, 100)));

                sf::RectangleShape sep(sf::Vector2f(185, 1));
                sep.setPosition(15, 88); sep.setFillColor(sf::Color(70, 70, 100));
                window.draw(sep);

                window.draw(makeText(font, "Route:", 18, 94, 12, sf::Color(160, 160, 200)));
                int y = 112;
                for (int i = 0; i < routeSize - 1; i++) {
                    int a = route[i], b = route[i + 1];
                    string line = g.cityNames[a] + " -> " + g.cityNames[b]
                        + " (" + to_string(g.matrix[a][b]) + "km)";
                    window.draw(makeText(font, line, 18, y, 11, sf::Color(220, 220, 220)));
                    y += 17;
                    if (y > 575) break;
                }
                window.draw(btnPerf);
                window.draw(makeText(font, "Performance", 38, 607, 13, sf::Color::White));
            }
            window.draw(btnBack);
            window.draw(makeText(font, "< Back", 65, 657, 13, sf::Color::White));
        }
        // SCREEN 3: PERFORMANCE
        else if (screen == 3) {
            window.draw(makeText(font, "Performance", 18, 15, 15, sf::Color(200, 200, 255)));
            sf::RectangleShape divLine(sf::Vector2f(185, 1));
            divLine.setPosition(15, 40); divLine.setFillColor(sf::Color(70, 70, 100));
            window.draw(divLine);

            auto row = [&](string label, string value, int y) {
                window.draw(makeText(font, label, 18, y, 11, sf::Color(140, 140, 180)));
                window.draw(makeText(font, value, 18, y + 15, 13, sf::Color::White));
                };

            string repr = (selectedAlgo == 0 || selectedAlgo == 1 || selectedAlgo == 4) ? "Adjacency Matrix" : "Adjacency List";
            string comp = (selectedAlgo == 1 || selectedAlgo == 3) ? "O(n!)" :
                (selectedAlgo == 4 || selectedAlgo == 5) ? "O(n^2*iter)" : "O(n^2)";
            string mem = (selectedAlgo == 1) ? "O(n!)" :
                (selectedAlgo == 0) ? "O(n^2)" : "O(n)";
            string acc = (selectedAlgo == 1 || selectedAlgo == 3) ? "Optimal (exact)" :
                (selectedAlgo == 4 || selectedAlgo == 5) ? "Near-optimal" : "Approximate";

            row("Algorithm:", algoName, 48);
            row("Representation:", repr, 90);
            row("Path Cost:", to_string(routeCost) + " km", 132);
            row("Exec Time:", to_string(lastTime) + " sec", 174);
            row("Time Complexity:", comp, 216);
            row("Space Complexity:", mem, 258);
            row("Cities (n):", to_string(g.n) + " cities", 300);
            row("Solution Type:", acc, 342);

            window.draw(btnBack);
            window.draw(makeText(font, "< Back", 65, 657, 13, sf::Color::White));
        }
        // ===================================================
        // SCREEN 4: COMPARISON TABLE
        // ===================================================
        else if (screen == 4) {
            window.draw(makeText(font, "Algorithm Comparison", 18, 12, 13, sf::Color(200, 200, 255)));
            sf::RectangleShape divLine(sf::Vector2f(185, 1));
            divLine.setPosition(15, 30); divLine.setFillColor(sf::Color(70, 70, 100));
            window.draw(divLine);
            window.draw(makeText(font, "All from city 0:", 18, 34, 10, sf::Color(140, 140, 180)));

            // 6 rows, each 98px tall, starting at y=48, ending at y=48+6*98=636, Back btn at 645
            int startY = 48;
            int rowH = 98;

            for (int i = 0; i < 6; i++) {
                int rowY = startY + i * rowH;

                sf::RectangleShape rowBg(sf::Vector2f(185, rowH - 4));
                rowBg.setPosition(15, rowY);
                rowBg.setFillColor(sf::Color(45, 45, 70));
                rowBg.setOutlineColor(sf::Color(70, 70, 100));
                rowBg.setOutlineThickness(1);
                window.draw(rowBg);

                // Algorithm name
                window.draw(makeText(font, algoLabels[i], 20, rowY + 4, 10, sf::Color(180, 220, 255)));

                sf::RectangleShape innerSep(sf::Vector2f(175, 1));
                innerSep.setPosition(20, rowY + 17); innerSep.setFillColor(sf::Color(70, 70, 100));
                window.draw(innerSep);

                if (!cmpDone[i]) {
                    string msg = ((i == 1 || i == 3) && g.n > 10) ? "N/A (n > 10)" : "Not run yet";
                    window.draw(makeText(font, msg, 20, rowY + 22, 10, sf::Color(160, 100, 100)));
                }
                else {
                    // Cost and Time on same line
                    window.draw(makeText(font, "Cost: " + to_string(cmpCost[i]) + "km", 20, rowY + 22, 10, sf::Color(100, 220, 100)));
                    window.draw(makeText(font, "T: " + to_string(cmpTime[i]) + "s", 20, rowY + 36, 10, sf::Color(220, 220, 100)));
                    // Complexity and Memory on same line
                    window.draw(makeText(font, "TC: " + algoComplexity[i], 20, rowY + 52, 10, sf::Color(200, 180, 255)));
                    window.draw(makeText(font, "SC: " + algoMemory[i], 20, rowY + 66, 10, sf::Color(200, 180, 255)));
                }
            }

            window.draw(btnBack);
            window.draw(makeText(font, "< Back", 65, 657, 13, sf::Color::White));
        }

        window.display();
    }

    return 0;
}