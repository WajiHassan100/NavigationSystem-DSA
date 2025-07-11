#include <iostream>
#include <fstream>
#include <queue>
#include <string>
using namespace std;

#define MAX_NODES 100
const int INF = 1e9;
const double FUEL_EFFICIENCY = 14.0; // km per liter
const double FUEL_PRICE_PER_LITER = 240.3; // Current fuel price

struct Edge {
    int destination;
    int weight;
    bool blocked;
    bool underMaintenance; 
    Edge* next;
};

Edge* adjList[MAX_NODES];
string cityNames[MAX_NODES];
int cityCount = 0;

void initializeGraph() {
    for (int i = 0; i < MAX_NODES; i++)
        adjList[i] = nullptr;
}

int addCity(string name) {
    cityNames[cityCount] = name;
    return cityCount++;
}

int findCityIndex(string name) {
    for (int i = 0; i < cityCount; i++) {
        if (cityNames[i] == name) 
        return i;
    }
    return -1;
}

void addEdge(int src, int dst, int weight, bool blocked = false) {
Edge* newEdge = new Edge;
newEdge->destination = dst;
newEdge->weight = weight;
newEdge->blocked = blocked;
newEdge->next = adjList[src];
adjList[src] = newEdge;

}

void blockRoad(int src, int dst) {
    Edge* temp = adjList[src];
    while (temp!=nullptr) {
        if (temp->destination == dst) {
            temp->blocked = true;
            break;
        }
        temp = temp->next;
    }
}

void unblockRoad(int src, int dst) {
    Edge* temp = adjList[src];
    while (temp) {
        if (temp->destination == dst) {
            temp->blocked = false;
            break;
        }
        temp = temp->next;
    }
}

void printGraph() {
    cout << "\n=========== City Road Map ===========\n";
    for (int i = 0; i < cityCount; i++) {
        cout << cityNames[i] << " -> ";
        Edge* temp = adjList[i];
        
        while (temp!=nullptr) {
            cout << cityNames[temp->destination] << " (" << temp->weight << " km";
            if (temp->blocked) {
                cout << ", BLOCKED";
            }
            cout << ")  ";
            temp = temp->next;
        }
        cout << "\n";
    }
    cout << "====================================\n";
}

void runDijkstra(int start, int dist[], int parent[]) {
    bool visited[MAX_NODES] = {false};
    for (int i = 0; i < cityCount; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }
    dist[start] = 0;

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        
        if (visited[u]) {
        continue;
    }
        visited[u] = true;

        Edge* temp = adjList[u];
        while (temp!=nullptr) {
            if (!temp->blocked) {
                int v = temp->destination;
                int newDist = dist[u] + temp->weight;  // Calculate the distance to 'v' through 'u'
                if (newDist < dist[v]) {
                    dist[v] = newDist;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
            temp = temp->next;
        }
    }
}

Edge* findEdge(int u, int v) {
    Edge* temp = adjList[u];
    while (temp != nullptr) {
        if (temp->destination == v) 
        return temp;
        temp = temp->next;
    }
    return nullptr;
}

void printPath(int node, int parent[]) {

    if (parent[node] == -1) {
        cout << cityNames[node];  // Print the starting city
        return;                  // End recursion
    }

    printPath(parent[node], parent);  //Recursively print the path to the parent of the current node

    Edge* road = findEdge(parent[node], node);//parrent of the current node and the node itself
    if (road != nullptr && road->underMaintenance == true) {

        cout << " -> " << cityNames[node] << "(MAINTENANCE)";
    } else {
        cout << " -> " << cityNames[node];
    }
}

void suggestCities(int src) {
    int dist[MAX_NODES], parent[MAX_NODES];
    runDijkstra(src, dist, parent);
    cout << "\nReachable cities from " << cityNames[src] << ":\n";
    for (int i = 0; i < cityCount; i++) {
        if (i != src && dist[i] != INF)
            cout << "- " << cityNames[i] << " at " << dist[i] << " km\n";
    }
}

void findFarthestCities(int src, int n) {
    int dist[MAX_NODES], parent[MAX_NODES];
    runDijkstra(src, dist, parent);
    cout << "Reachable cities from " << cityNames[src] << ":\n";

    int maxDist = -1;
    int farthestCity = -1;

    for (int i = 0; i < cityCount; ++i) {
        if (i == src) {
            continue;
        }
        if (dist[i] != INF) {
            cout << "- " << cityNames[i] << " at " << dist[i] << " km\n";
            if (dist[i] > maxDist) {
                maxDist = dist[i];
                farthestCity = i;
            }
        }
    }

    if (farthestCity != -1) {
        cout << "\n Farthest reachable city from " << cityNames[src] << ": "
             << cityNames[farthestCity] << " at " << maxDist << " km\n";
    } else {
        cout << "No cities reachable from " << cityNames[src] << ".\n";
    }
}

void findRoutesWithFuelCost(int src, int dst) {
    int dist[MAX_NODES], parent[MAX_NODES];
    runDijkstra(src, dist, parent);

    if (dist[dst] == INF) {
        cout << "\nNo available route due to blocked roads.\n";
    } else {
        cout << "\nShortest Distance: " << dist[dst] << " km\nPath: ";
        printPath(dst, parent);
        double fuel = dist[dst] / FUEL_EFFICIENCY;
        cout << "\nEstimated Fuel Required: " << fuel << " liters\n";
    }
}

void budgetRouteSuggestion(int src, double budget) {
    int dist[MAX_NODES], parent[MAX_NODES];
    runDijkstra(src, dist, parent);

    cout << "\nCities reachable within Rs. " << budget << ":\n";
    
    bool found = false;
    
    for (int i = 0; i < cityCount; i++) {
        if (i != src && dist[i] != INF) {
            double cost = (dist[i] / FUEL_EFFICIENCY) * FUEL_PRICE_PER_LITER;
            
            if (cost <= budget) {
                cout << "- " << cityNames[i] << " (" << dist[i] << " km) | Cost: Rs. " << cost << "\n";
                found = true;
            }
        }
    }
    
    if (!found) {
        cout << "No destinations within your budget.\n";
    }
}

void sortCitiesByFuel(int src) {
    int dist[MAX_NODES], parent[MAX_NODES];
    runDijkstra(src, dist, parent);

    struct FuelInfo { 
        int index; 
        double fuel; 
    } 
    sorted[MAX_NODES];
    int count = 0;

    for (int i = 0; i < cityCount; i++) {
    
        if (i != src && dist[i] != INF) {
            sorted[count++] = {i, dist[i] / FUEL_EFFICIENCY};
        }
    }

    if (count == 0) {
    cout << "No reachable cities from " << cityNames[src] << ".\n";
    return;
}
    // Insertion sort by fuel requirement
    for (int i = 1; i < count; i++) {
        FuelInfo key = sorted[i];
        int j = i - 1; 
        while (j >= 0 && sorted[j].fuel > key.fuel) { // Shift cities with higher fuel requirement one position to the right
            sorted[j + 1] = sorted[j];
            j--;
        }
        sorted[j + 1] = key;
    }

    cout << "\nCities sorted by estimated fuel requirement from " << cityNames[src] << ":\n";
    for (int i = 0; i < count; i++) {
        cout << "- " << cityNames[sorted[i].index] << ": " << sorted[i].fuel << " liters\n";
    }
}

void primMST(int start) { //Connects all cities with the minimum total road cost 
    bool visited[MAX_NODES] = {false};
    int dist[MAX_NODES];
    int parent[MAX_NODES];
    
    //(weight, vertex) 
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    for (int i = 0; i < cityCount; i++) {
        dist[i] = INF;
        parent[i] = -1;
    }

    dist[start] = 0;
    pq.push({0, start});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();
        
        if (visited[u]) {
            continue;
        }

        visited[u] = true;

        Edge* temp = adjList[u];
        while (temp != nullptr) {
            if (!temp->blocked) {
                int v = temp->destination;
                int weight = temp->weight; // Cost to travel from 'u' to 'v'

                if (!visited[v] && weight < dist[v]) {
                    dist[v] = weight;
                    parent[v] = u;
                    pq.push({dist[v], v});
                }
            }
            temp = temp->next;
        }
    }

    cout << "\nMinimum Spanning Tree starting from " << cityNames[start] << ":\n";
    int totalCost = 0;
    for (int i = 0; i < cityCount; i++) {
        if (parent[i] != -1) {  // Skip nodes not in MST (disconnected)
            cout << cityNames[parent[i]] << " - " << cityNames[i] << " : " << dist[i] << " km\n";
            totalCost += dist[i];  // Sum all edge weights
        }
    }
    cout << "Total cost: " << totalCost << " km\n";
}

void roadMaintenanceMode() {
    cout << "\nROAD MAINTENANCE MODE\n";
    
    printGraph();  
    
    string city1, city2;
    cout << "Enter first city: ";
    getline(cin, city1);
    cout << "Enter second city: ";
    getline(cin, city2);
    
    int u = findCityIndex(city1);
    int v = findCityIndex(city2);
    
    if (u == -1 || v == -1) {
        cout << "Invalid cities!\n";
        return;
    }
    
    Edge* road = findEdge(u, v);
    if (road!=nullptr) {
        int originalWeight = road->weight;
        int reducedWeight = originalWeight * 1.3;  // 30% longer travel time
        
        road->weight = reducedWeight;
        road->underMaintenance = true;
        
        cout << "Road " << cityNames[u] << "-" << cityNames[v] 
             << " now has temporary speed restrictions:\n"
             << "Original distance: " << originalWeight << " km\n"
             << "Adjusted distance: " << reducedWeight << " km\n";
    } else {
        cout << "Road not found!\n";
    }
}

void loadDefaultMap(const string& filename) {
    ifstream fin(filename);
    if (!fin) {
        cout << "Error loading default map.\n";
        return;
    }

    int nCities; 
    fin >> nCities;// Read number of cities from the file
    fin.ignore();

    for (int i = 0; i < nCities; i++) {
        string city;
        getline(fin, city);
        addCity(city);
    }

    int nEdges;
    fin >> nEdges;
    fin.ignore();

    for (int i = 0; i < nEdges; i++) {
        string src, dst;
        int weight;
        fin >> src >> dst >> weight;
        int u = findCityIndex(src);
        int v = findCityIndex(dst);
        if (u != -1 && v != -1) {
            addEdge(u, v, weight);
            addEdge(v, u, weight);
        }
    }

    fin.close();
}

int main() {
    initializeGraph();
    loadDefaultMap("defaultMap.txt");

    string username;
    string password;

    bool isAdmin = false;

    cout << "Welcome to My Navigation System\n";
    cout << "Press Enter to login as User, or enter Admin username: ";
    getline(cin, username);

    if (!username.empty()) {
        cout << "Enter password: ";
        getline(cin, password);

        if (username == "waji" && password == "dsa123") {
            isAdmin = true;
            cout << "\nLogged in as Admin\n";
        } else {
            cout << "\nInvalid credentials. Logging in as User...\n";
        }
    } else {
        cout << "\nLogged in as User\n";
    }

    while (true) {
    cout << "\n----- Navigation Menu -----\n";
    cout << "1. Find Shortest Paths From City\n";
    cout << "2. Find N Farthest Cities from a City\n";
    cout << "3. Find Route Between Two Cities (with Fuel Estimation)\n";
    cout << "4. Show All Roads\n";
    cout << "5. Budget-Based Route Suggestion\n";
    cout << "6. Show Cities by Fuel Requirement\n";

    if (isAdmin) {
        cout << "7. Map New City\n";
        cout << "8. Define New Route\n";
        cout << "9. Block Road\n";
        cout << "10. Unblock Road\n";
        cout << "11. Enter Road Maintenance Mode\n";
    }

    cout << "12. Show Road Connectivity Map (Minimum Spanning Tree)\n";
    cout << "0. Exit\n";

    cout << "Enter your choice: ";
    int choice;
    cin >> choice;
    cin.ignore();

    if (choice == 1) {
        string city;
        cout << "Enter city name: ";
        getline(cin, city);
        int idx = findCityIndex(city);
        if (idx != -1)
            suggestCities(idx);
        else
            cout << "City not found.\n";

    } else if (choice == 2) {
        string city;
        int n;
        cout << "Enter city name: ";
        getline(cin, city);
        int idx = findCityIndex(city);
        if (idx != -1) {
            cout << "Enter number of cities: ";
            cin >> n;
            cin.ignore();
            findFarthestCities(idx, n);
        } else {
            cout << "City not found.\n";
        }

    } else if (choice == 3) {
        string from, to;
        cout << "Enter source city: ";
        getline(cin, from);
        cout << "Enter destination city: ";
        getline(cin, to);
        int src = findCityIndex(from);
        int dst = findCityIndex(to);
        if (src != -1 && dst != -1)
            findRoutesWithFuelCost(src, dst);
        else
            cout << "City not found.\n";

    } else if (choice == 4) {
        printGraph();

    } else if (choice == 5) {
        string city;
        double budget;
        cout << "Enter source city: ";
        getline(cin, city);
        int idx = findCityIndex(city);
        if (idx != -1) {
            cout << "Enter budget (Rs): ";
            cin >> budget;
            cin.ignore();
            budgetRouteSuggestion(idx, budget);
        } else {
            cout << "City not found.\n";
        }

    } else if (choice == 6) {
        string city;
        cout << "Enter source city: ";
        getline(cin, city);
        int idx = findCityIndex(city);
        if (idx != -1) {
            sortCitiesByFuel(idx);
        } else {
            cout << "City not found.\n";
        }

    } else if (isAdmin && choice == 7) {
        string city;
        cout << "Enter new city: ";
        getline(cin, city);
        if (findCityIndex(city) == -1) {
            addCity(city);
            cout << "City added.\n";
        } else {
            cout << "City already exists.\n";
        }

    } else if (isAdmin && choice == 8) {
        string c1, c2;
        int d;
        cout << "Enter first city: ";
        getline(cin, c1);
        cout << "Enter second city: ";
        getline(cin, c2);
        cout << "Enter distance: ";
        cin >> d;
        cin.ignore();
        int u = findCityIndex(c1);
        int v = findCityIndex(c2);
        if (u != -1 && v != -1) {
            addEdge(u, v, d);
            addEdge(v, u, d);
            cout << "Road added.\n";
        } else
            cout << "City not found.\n";

    } else if (isAdmin && choice == 9) {
        string c1, c2;
        cout << "Enter source city: ";
        getline(cin, c1);
        cout << "Enter destination city: ";
        getline(cin, c2);
        int u = findCityIndex(c1);
        int v = findCityIndex(c2);
        if (u != -1 && v != -1) {
            blockRoad(u, v);
            blockRoad(v, u);
            cout << "Road blocked.\n";
        } else {
            cout << "City not found.\n";
        }

    } else if (isAdmin && choice == 10) {
        string c1, c2;
        cout << "Enter source city: ";
        getline(cin, c1);
        cout << "Enter destination city: ";
        getline(cin, c2);
        int u = findCityIndex(c1);
        int v = findCityIndex(c2);
        if (u != -1 && v != -1) {
            unblockRoad(u, v);
            unblockRoad(v, u);
            cout << "Road unblocked.\n";
        } else {
            cout << "City not found.\n";
        }

    } else if (isAdmin && choice == 11) {
        roadMaintenanceMode();

    } else if (choice == 12) {
        string startCity;
        cout << "Enter the starting city for MST: ";
        getline(cin, startCity);
        int startIdx = findCityIndex(startCity);
        if (startIdx != -1) {
            primMST(startIdx);
        } else {
            cout << "City not found.\n";
        }

    } else if (choice == 0) {
        cout << "Goodbye!\n";
        break;

    } else {
        cout << "Invalid option.\n";
    }
}

}