#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <chrono>   // For timing simulation
#include <thread>   // For sleep_for
#include <ctime>
using namespace std;

// Structure for Nodes (Intersections)
struct Nodes {
    string name;  // Name of the intersection
    int timing;
    
    Nodes() : name(""), timing(0) {}
    Nodes(string n, int t) : name(n), timing(t) {}
};

// Structure for Edges (Streets)
struct Edges {
    Nodes* from;  // Pointer to the starting node
    Nodes* to;    // Pointer to the ending node
    int weight;   // Weight of the edge (e.g., distance or time)
    int capacity; 
    bool isActive; // Indicates if the edge is active

    Edges() : from(nullptr), to(nullptr), weight(0), capacity(3), isActive(true) {}
    Edges(Nodes* f, Nodes* t, int w) : from(f), to(t), weight(w), capacity(3), isActive(true) {}
};

struct VehiclesNode {
    string ID;
    Edges * edge;
    string priorityLevel;  
    VehiclesNode() : edge(NULL), ID("") {}
    VehiclesNode(string id, Edges * e, string p) : ID(id), edge(e), priorityLevel(p) {}
};

// Graph class
class Graph {
private:
    Nodes* intersections;  // Array of intersections (nodes)
    Edges* streets;        // Array of streets (edges)
    int numNodes;          // Number of nodes
    int numEdges;  
    int nodeCapacity;      // Capacity for nodes array
    int edgeCapacity;      // Capacity for edges array
    int* vehicleCounts;

    void resizeNodes() {
        nodeCapacity *= 2;
        Nodes* temp = new Nodes[nodeCapacity];
        for (int i = 0; i < numNodes; i++) {
            temp[i] = intersections[i];
        }
        delete[] intersections;
        intersections = temp;
    }

    void resizeEdges() {
        edgeCapacity *= 2;
        Edges* temp = new Edges[edgeCapacity];
        for (int i = 0; i < numEdges; i++) {
            temp[i] = streets[i];
        }
        delete[] streets;
        streets = temp;
    }

public:
    Graph() : intersections(nullptr), streets(nullptr), numNodes(0), numEdges(0), nodeCapacity(10), edgeCapacity(10), vehicleCounts(nullptr) {
        intersections = new Nodes[nodeCapacity];
        streets = new Edges[edgeCapacity];
        vehicleCounts = new int[edgeCapacity];
    }

    int getnumEdges() {
        return numEdges; 
    }

    Edges getStreets(int i) {
        return streets[i];          
    }

    ~Graph() {
        delete[] intersections;
        delete[] streets;
        delete[] vehicleCounts;
    }

    Nodes* findNode(const string& name) {
        for (int i = 0; i < numNodes; i++) {
            if (intersections[i].name == name) {
                return &intersections[i];
            }
        }
        return nullptr;
    }

    int findEdgeIndex(const string& fromName, const string& toName) {
        for (int i = 0; i < numEdges; i++) {
            if (streets[i].from->name == fromName && streets[i].to->name == toName) {
                return i;
            }
        }
        return -1;
    }

    void incrementVehicleCount(const string& fromName, const string& toName) {
        int index = findEdgeIndex(fromName, toName);
        if (index != -1) {
            vehicleCounts[index]++;
        }
    }

    void decrementVehicleCount(const string& fromName, const string& toName) {
        int index = findEdgeIndex(fromName, toName);
        if (index != -1 && vehicleCounts[index] > 0) {
            vehicleCounts[index]--;
        }
    }

    bool isCongested(const string& fromName, const string& toName) {
        int index = findEdgeIndex(fromName, toName);
        if (index != -1) {
            return vehicleCounts[index] >= streets[index].capacity;
        }
        return false;
    }

    int findNodeIndex(const string& name) {
        for (int i = 0; i < numNodes; i++) {
            if (intersections[i].name == name) {
                return i;
            }
        }
        return -1;
    }

    Nodes** bfs(const string& startName, const string& endName) {
        const int MAX_NODES = 100;
        Nodes* queue[MAX_NODES];
        Nodes* parent[MAX_NODES];
        bool visited[MAX_NODES] = {false};
        int front = 0, rear = 0;

        Nodes* startNode = findNode(startName);
        Nodes* endNode = findNode(endName);
        if (startNode == nullptr || endNode == nullptr) {
            return nullptr;
        }

        int startIndex = findNodeIndex(startName);
        int endIndex = findNodeIndex(endName);

        queue[rear++] = startNode;
        visited[startIndex] = true;
        parent[startIndex] = nullptr;

        while (front != rear) {
            Nodes* current = queue[front++];
            int currentIndex = findNodeIndex(current->name);

            if (current == endNode) {
                int pathLength = 0;
                Nodes* path[MAX_NODES];
                Nodes* node = endNode;
                while (node != nullptr) {
                    path[pathLength++] = node;
                    node = parent[findNodeIndex(node->name)];
                }
                Nodes** result = new Nodes*[pathLength + 1];
                for (int i = 0; i < pathLength; i++) {
                    result[i] = path[pathLength - 1 - i];
                }
                result[pathLength] = nullptr;
                return result;
            }

            for (int i = 0; i < numEdges; i++) {
                if (streets[i].from == current && streets[i].isActive && !visited[findNodeIndex(streets[i].to->name)]) {
                    Nodes* neighbor = streets[i].to;
                    int neighborIndex = findNodeIndex(neighbor->name);
                    queue[rear++] = neighbor;
                    visited[neighborIndex] = true;
                    parent[neighborIndex] = current;
                }
            }
        }
        return nullptr;
    }

    void createNode(const string& name, int timings) {
        if (findNode(name) != nullptr) {
            cout << "Node with the name '" << name << "' already exists!" << endl;
            return;
        }

        if (numNodes == nodeCapacity) {
            resizeNodes();
        }

        intersections[numNodes++] = Nodes(name, timings);
        cout << "Node '" << name << "' has been added." << endl;
    }

    void createEdges(const string& from, const string& to, int weight) {
        Nodes* fromNode = findNode(from);
        Nodes* toNode = findNode(to);

        if (fromNode && toNode) {
            for (int i = 0; i < numEdges; i++) {
                if (streets[i].from == fromNode && streets[i].to == toNode) {
                    cout << "Edge from '" << from << "' to '" << to << "' already exists. Updating weight to " << weight << "." << endl;
                    streets[i].weight = weight;
                    return;
                }
            }

            if (numEdges == edgeCapacity) {
                resizeEdges();
            }
            streets[numEdges++] = Edges(fromNode, toNode, weight);
            cout << "Edge from '" << from << "' to '" << to << "' with weight " << weight << " has been added." << endl;
        } else {
            cout << "Error: One or both nodes not found!" << endl;
        }
    }

    void inputNodes() {
        cout << "Enter node names (type -1 to stop):" << endl;
        string name;
        int timings;
        while (true) {
            cout << "Enter node name: ";
            cin >> name;
            if (name == "-1") break;
            cout << "Enter Green Time: ";
            cin >> timings;
            createNode(name, timings);
            ofstream file("traffic_signal_timings.csv", std::ios::app);
            file << name << ',' << timings << endl; 
            file.close();
        }
    }

    void inputEdges() {
        cout << "Enter edges in the format: from to weight (type -1 to stop):" << endl;
        string from, to;
        int weight;
        while (true) {
            cout << "Enter edge (from to weight): ";
            cin >> from;
            if (from == "-1") break;
            cin >> to >> weight;
            createEdges(from, to, weight);
            ofstream file("road_network.csv", std::ios::app);
            file << from << "," << to << "," << weight << endl; 
            file.close();
        }
    }

    void displayGraph() {
        cout << "Nodes:" << endl;
        for (int i = 0; i < numNodes; i++) {
            cout << i << ": " << intersections[i].name << endl;
        }

        cout << "Edges:" << endl;
        for (int i = 0; i < numEdges; i++) {
            cout << i << ": " << streets[i].from->name << " -> "
                 << streets[i].to->name
                 << " (Weight: " << streets[i].weight << ")" << endl;
        }
    }

    void DeleteNode(const string& name) {
        Nodes* node = findNode(name);
        if (node == nullptr) {
            cout << "Node not found!" << endl;
            return;
        }

        int i = 0;
        while (i < numEdges) {
            if (streets[i].from == node || streets[i].to == node) {
                for (int j = i; j < numEdges - 1; j++) {
                    streets[j] = streets[j + 1];
                }
                numEdges--;
            } else {
                i++;
            }
        }

        int nodeIndex = -1;
        for (int i = 0; i < numNodes; i++) {
            if (intersections[i].name == name) {
                nodeIndex = i;
                break;
            }
        }

        if (nodeIndex != -1) {
            for (int i = nodeIndex; i < numNodes - 1; i++) {
                intersections[i] = intersections[i + 1];
            }
            numNodes--;
        }

        updateNodeFile(name);
        updateEdgeFile(name);
        cout << "Node " << name << " and its associated edges have been deleted." << endl;
    }

    void traffic_signal_timings() {
        ifstream file1("traffic_signal_timings.csv");
        string node;
        int timings;
        if (file1.good()) {
            string header;
            getline(file1, header);
        }
        
        while (file1.good()) {
            getline(file1, node, ',');
            file1 >> timings;
            file1.ignore(__LONG_MAX__, '\n');
            if (!node.empty()) {
                createNode(node, timings);
            }
        }
    }

    void road_Network() {
        ifstream file("road_network.csv");
        string from_node, to_node;
        int weight;

        if (file.good()) {
            string header;
            getline(file, header);
        }

        while (file.good()) {
            getline(file, from_node, ',');
            getline(file, to_node, ',');
            file >> weight;
            file.ignore(__LONG_MAX__, '\n');
            if (!from_node.empty() && !to_node.empty()) {
                createEdges(from_node, to_node, weight);
            }
        }
        file.close();
    }

    Edges* findEdge(Nodes* from, Nodes* to) {
        for (int i = 0; i < numEdges; i++) {
            if (streets[i].from == from && streets[i].to == to) {
                return &streets[i];
            }
        }
        return nullptr;
    }

    void DeleteEdge(const string& fromNode, const string& toNode, int weight) {
        bool edgeDeleted = false;
        int i = 0;
        while (i < numEdges) {
            if (streets[i].from->name == fromNode && streets[i].to->name == toNode && streets[i].weight == weight) {
                for (int j = i; j < numEdges - 1; j++) {
                    streets[j] = streets[j + 1];
                }
                numEdges--;
                edgeDeleted = true;
                cout << "Edge " << fromNode << " -> " << toNode << " (Weight: " << weight << ") has been deleted from the graph." << endl;
            } else {
                i++;
            }
        }

        if (!edgeDeleted) {
            cout << "No edge found between " << fromNode << " and " << toNode << " with weight " << weight << "." << endl;
        }

        updateEdgeFile(fromNode);
    }

    void updateEdgeFile(const string& nodeName) {
        ifstream infile("road_network.csv");
        ofstream outfile("temp.csv");

        string from, to, header;
        int weight;

        if (getline(infile, header)) {
            outfile << header << endl;
        }

        while (getline(infile, from, ',')) {
            getline(infile, to, ',');
            infile >> weight;
            infile.ignore(__LONG_MAX__, '\n');

            if (from == nodeName || to == nodeName) {
                continue;
            }

            outfile << from << "," << to << "," << weight << endl;
        }

        infile.close();
        outfile.close();

        remove("road_network.csv");
        rename("temp.csv", "road_network.csv");
    }

    void updateNodeFile(const string& nodeName) {
        ifstream infile("traffic_signal_timings.csv");
        ofstream outfile("temp.csv");

        string node;
        int timings;

        bool headerRead = false;

        while (infile.good()) {
            if (!headerRead) {
                string header;
                getline(infile, header);
                outfile << header << endl;
                headerRead = true;
            }
            
            getline(infile, node, ',');
            if (node.empty()) continue;
            
            infile >> timings;
            infile.ignore(__LONG_MAX__, '\n');

            if (node != nodeName) {
                outfile << node << "," << timings << endl;
            }
        }

        infile.close();
        outfile.close();

        remove("traffic_signal_timings.csv");
        rename("temp.csv", "traffic_signal_timings.csv");
    }

    Nodes** dijkstra(const string& fromNodeName, const string& toNodeName, int& pathLength) {
        const int INF = 999999;
        int* dist = new int[numNodes];
        bool* visited = new bool[numNodes];
        int* predecessor = new int[numNodes];

        for (int i = 0; i < numNodes; i++) {
            dist[i] = INF;
            visited[i] = false;
            predecessor[i] = -1;
        }

        int startIdx = -1, endIdx = -1;
        for (int i = 0; i < numNodes; i++) {
            if (intersections[i].name == fromNodeName) {
                startIdx = i;
            }
            if (intersections[i].name == toNodeName) {
                endIdx = i;
            }
        }

        if (startIdx == -1 || endIdx == -1) {
            delete[] dist;
            delete[] visited;
            delete[] predecessor;
            pathLength = 0;
            return nullptr;
        }

        dist[startIdx] = 0;

        for (int i = 0; i < numNodes; i++) {
            int u = -1, minDist = INF;
            for (int j = 0; j < numNodes; j++) {
                if (!visited[j] && dist[j] < minDist) {
                    minDist = dist[j];
                    u = j;
                }
            }

            if (u == -1) break;

            visited[u] = true;

            for (int j = 0; j < numEdges; j++) {
                if (streets[j].from == &intersections[u] && streets[j].isActive) {
                    int v = -1;
                    for (int k = 0; k < numNodes; k++) {
                        if (streets[j].to == &intersections[k]) {
                            v = k;
                            break;
                        }
                    }

                    if (v != -1 && !visited[v] && dist[u] + streets[j].weight < dist[v]) {
                        dist[v] = dist[u] + streets[j].weight;
                        predecessor[v] = u;
                    }
                }
            }
        }

        if (dist[endIdx] == INF) {
            delete[] dist;
            delete[] visited;
            delete[] predecessor;
            pathLength = 0;
            return nullptr;
        }

        pathLength = dist[endIdx];

        int tempPathLength = 0;
        int currIdx = endIdx;
        while (currIdx != -1) {
            tempPathLength++;
            currIdx = predecessor[currIdx];
        }

        Nodes** path = new Nodes*[tempPathLength + 1];
        currIdx = endIdx;
        for (int i = tempPathLength - 1; i >= 0; i--) {
            path[i] = &intersections[currIdx];
            currIdx = predecessor[currIdx];
        }
        path[tempPathLength] = nullptr;

        delete[] dist;
        delete[] visited;
        delete[] predecessor;

        return path;
    }

    void displayCongestion() {
        for (int i = 0; i < numEdges; i++) {
            cout << "Road " << streets[i].from->name << "-" << streets[i].to->name << ": "
                 << vehicleCounts[i] << " vehicles";
            if (isCongested(streets[i].from->name, streets[i].to->name)) {
                cout << " (Congested)";
            }
            cout << endl;
        }
    }

    void blockRoad(const string& fromName, const string& toName) {
        int index = findEdgeIndex(fromName, toName);
        if (index != -1) {
            streets[index].isActive = false;
            cout << "Road from " << fromName << " to " << toName << " has been closed." << endl;
        } else {
            cout << "Edge not found." << endl;
        }
    }

    void reopenRoad(const string& fromName, const string& toName) {
        int index = findEdgeIndex(fromName, toName);
        if (index != -1) {
            streets[index].isActive = true;
            cout << "Road from " << fromName << " to " << toName << " has been reopened." << endl;
        } else {
            cout << "Edge not found." << endl;
        }
    }

    bool isEdgeActive(const string& fromName, const string& toName) {
        int index = findEdgeIndex(fromName, toName);
        if (index != -1) {
            return streets[index].isActive;
        }
        return false;
    }
};

class Vehicles : public Graph {
private:
    VehiclesNode *vehicle;
    int capacity;
    int numVehicles;

    void IncreaseNumVehicles() {
        VehiclesNode *NVehicles = new VehiclesNode[capacity + 5];
        int i = 0;
        while (i < capacity) {
            NVehicles[i] = vehicle[i];
            i++;
        }
        capacity += 5;
        delete[] vehicle;
        vehicle = NVehicles;
    }

public:
    Vehicles() : Graph(), vehicle(new VehiclesNode[10]), capacity(10), numVehicles(0) {}

    ~Vehicles() {
        delete[] vehicle;
    }

    void createVehicles(const string& name, string& from_node, string& to_node, string priorityLevel) {
        Nodes* Start = findNode(from_node);
        Nodes* End = findNode(to_node);

        if (Start == nullptr || End == nullptr) {
            cout << "One or both Nodes not found!" << endl;
            return;
        }

        if (numVehicles >= capacity) {
            IncreaseNumVehicles();
        }

        VehiclesNode NewVehicle;
        NewVehicle.ID = name;
        NewVehicle.edge = new Edges(Start, End, 0);
        NewVehicle.priorityLevel = priorityLevel;
        vehicle[numVehicles++] = NewVehicle;
    }

    void Input_Vehicle() {
        string id, Start, End, priorityLevel;
        while (true) {
            if (numVehicles == capacity) {
                IncreaseNumVehicles();
            }

            cout << "Enter Vehicle ID, Start Intersection, End Intersection, and Priority Level: ";
            cin >> id >> Start >> End >> priorityLevel;
            if (id == "-1") {
                break;
            }
            createVehicles(id, Start, End, priorityLevel);
        }
    }

    void calcaulate_route() {
        int distance;
        for (int i = 0; i < numVehicles; i++) {
            cout << "Vehicle with id " << vehicle[i].ID << endl;

            Nodes** path = dijkstra(vehicle[i].edge->from->name, vehicle[i].edge->to->name, distance);

            if (path == nullptr) {
                cout << "Node is Unreachable" << endl;
            } else {
                cout << "Path: ";
                int j = 0;
                while (path[j] != nullptr) {
                    cout << path[j]->name;
                    j++;
                    if (path[j] != nullptr) {
                        cout << " -> ";
                    }
                }
                cout << endl;
                cout << "Shortest Distance: " << distance << endl;
                cout << endl;

                delete[] path;
            }
        }
    }

    void vehicles_csv() {
        ifstream file("vehicles.csv");
        string header;
        file >> header;
        string id, start, end;
        string priorityLevel;
        while (file.good()) {
            getline(file, id, ',');
            getline(file, start, ',');
            file >> end;
            priorityLevel = "Low";
            file.ignore(__LONG_MAX__, '\n');
            if (!start.empty() && !end.empty() && !id.empty()) {
                createVehicles(id, start, end, priorityLevel);
            }
        }
        file.close();

        ifstream file1("emergency_vehicles.csv");
        file1 >> header;

        while (file1.good()) {
            getline(file1, id, ',');
            getline(file1, start, ',');
            getline(file1, end, ',');
            file1 >> priorityLevel;
            file1.ignore(__LONG_MAX__, '\n');
            if (!start.empty() && !end.empty() && !id.empty() && !priorityLevel.empty()) {
                createVehicles(id, start, end, priorityLevel);
            }
        }
        file1.close();
    }

    void displayVehicles() {
        if (numVehicles == 0) {
            cout << "No vehicles available." << endl;
            return;
        }

        cout << "Vehicles in the system:" << endl;
        for (int i = 0; i < numVehicles; i++) {
            cout << "ID: " << vehicle[i].ID << ", Start: " << vehicle[i].edge->from->name
                << ", End: " << vehicle[i].edge->to->name << ", Priority: " << vehicle[i].priorityLevel << endl;
        }
    }

    int calculate_travel_time(Nodes** path, string start_name, string end_name, int red_duration) {
        int total_time = 0;
        int departure_time = 0;
        for (int index = 0; path[index + 1] != nullptr; index++) {
            Nodes* current_node = path[index];
            Nodes* next_node = path[index + 1];
            int edge_weight = get_edge_weight(current_node, next_node);
            if (edge_weight == -1) return -1;

            int travel_time = edge_weight;
            int arrival_time = departure_time + travel_time;
            if (!is_light_green(next_node, arrival_time, red_duration)) {
                int wait_time = calculate_wait_time(next_node, arrival_time, red_duration);
                total_time += travel_time + wait_time;
                departure_time = arrival_time + wait_time;
            } else {
                total_time += travel_time;
                departure_time = arrival_time;
            }
        }
        return total_time;
    }

    int get_edge_weight(Nodes* from, Nodes* to) {
        for (int i = 0; i < getnumEdges(); i++) {
            if (getStreets(i).from == from && getStreets(i).to == to) {
                return getStreets(i).weight;
            }
        }
        return -1;
    }

    bool is_light_green(Nodes* node, int arrival_time, int red_duration) {
        if (node == nullptr) {
            cerr << "Null node in is_light_green" << endl;
            return false;
        }
        int total_cycle = node->timing + red_duration;
        int time_in_cycle = arrival_time % total_cycle;
        return time_in_cycle < node->timing;
    }

    int calculate_wait_time(Nodes* node, int arrival_time, int red_duration) {
        if (node == nullptr) {
            cerr << "Null node in calculate_wait_time" << endl;
            return 0;
        }
        int total_cycle = node->timing + red_duration;
        int time_in_cycle = arrival_time % total_cycle;
        if (time_in_cycle < node->timing) {
            return 0;
        } else {
            return total_cycle - time_in_cycle;
        }
    }

    void simulate() {
        int red_duration = 30;
        int max_simulation_time = 300;

        Nodes*** vehicle_paths = new Nodes**[numVehicles];
        int* path_lengths = new int[numVehicles];

        for (int i = 0; i < numVehicles; i++) {
            int distance;
            Nodes** path = dijkstra(vehicle[i].edge->from->name, vehicle[i].edge->to->name, distance);
            if (path == nullptr) {
                std::cerr << "No path found for vehicle " << vehicle[i].ID << std::endl;
                continue;
            }
            int path_length = 0;
            while (path[path_length] != nullptr) {
                path_length++;
            }
            vehicle_paths[i] = new Nodes*[path_length + 1];
            for (int j = 0; j < path_length; j++) {
                vehicle_paths[i][j] = path[j];
            }
            vehicle_paths[i][path_length] = nullptr;
            path_lengths[i] = path_length;
            delete[] path;
        }

        int* vehicle_positions = new int[numVehicles];
        int* vehicle_departure_times = new int[numVehicles];
        for (int i = 0; i < numVehicles; i++) {
            vehicle_positions[i] = 0;
            vehicle_departure_times[i] = 0;
        }

        auto start_time = std::chrono::high_resolution_clock::now();

        bool all_arrived = false;
        while (!all_arrived) {
            auto current_real_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed_real = current_real_time - start_time;
            int current_simulation_time = static_cast<int>(elapsed_real.count());

            all_arrived = true;
            for (int i = 0; i < numVehicles; i++) {
                if (vehicle_positions[i] < path_lengths[i] - 1) {
                    all_arrived = false;
                    break;
                }
            }
            if (all_arrived) {
                break;
            }

            for (int i = 0; i < numVehicles; i++) {
                if (vehicle_positions[i] >= path_lengths[i] - 1) {
                    continue;
                }

                Nodes* current_node = vehicle_paths[i][vehicle_positions[i]];
                Nodes* next_node = vehicle_paths[i][vehicle_positions[i] + 1];
                int edge_weight = get_edge_weight(current_node, next_node);
                if (edge_weight == -1) {
                    std::cerr << "Invalid edge for vehicle " << vehicle[i].ID << std::endl;
                    continue;
                }

                int arrival_time = vehicle_departure_times[i] + edge_weight;
                if (arrival_time > current_simulation_time) {
                    continue;
                }

                if (vehicle[i].priorityLevel == "High") {
                    vehicle_positions[i]++;
                    vehicle_departure_times[i] = arrival_time + edge_weight;
                    std::cout << "Emergency Vehicle " << vehicle[i].ID << " moves to " << next_node->name
                            << " at time " << current_simulation_time << " seconds." << std::endl;
                } else {
                    if (!is_light_green(next_node, arrival_time, red_duration)) {
                        int wait_time = calculate_wait_time(next_node, arrival_time, red_duration);
                        vehicle_departure_times[i] += wait_time;
                        std::cout << "Vehicle " << vehicle[i].ID << " is waiting at " << next_node->name
                                << " for " << wait_time << " seconds." << std::endl;
                    } else {
                        if (isCongested(current_node->name, next_node->name)) {
                            Nodes** alternative_path = bfs(current_node->name, next_node->name);
                            if (alternative_path != nullptr) {
                                delete[] vehicle_paths[i];
                                vehicle_paths[i] = alternative_path;
                                path_lengths[i] = 0;
                                while (alternative_path[path_lengths[i]] != nullptr) {
                                    path_lengths[i]++;
                                }
                                vehicle_positions[i] = 0;
                                vehicle_departure_times[i] = current_simulation_time;
                            } else {
                                std::cerr << "No alternative path found for vehicle " << vehicle[i].ID << std::endl;
                            }
                        } else {
                            vehicle_positions[i]++;
                            vehicle_departure_times[i] = arrival_time + edge_weight;
                            std::cout << "Vehicle " << vehicle[i].ID << " moves to " << next_node->name
                                    << " at time " << current_simulation_time << " seconds." << std::endl;
                        }
                    }
                }
            }

            auto current_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> elapsed = current_time - start_time;
            double simulation_time = elapsed.count();
            if (simulation_time < max_simulation_time) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
            } else {
                break;
            }
        }

        for (int i = 0; i < numVehicles; i++) {
            cout << "Vehicle " << vehicle[i].ID << " path: ";
            int j;
            for (j = 0; j < path_lengths[i] - 1; j++) {
                cout << vehicle_paths[i][j]->name << " -> ";
            }
            cout << vehicle_paths[i][j]->name << " Destination Reached." << endl;
            cout << endl;
        }

        for (int i = 0; i < numVehicles; i++) {
            delete[] vehicle_paths[i];
        }
        delete[] vehicle_paths;
        delete[] path_lengths;
        delete[] vehicle_positions;
        delete[] vehicle_departure_times;

        std::cout << "Simulation complete." << std::endl;
    }
};

int main() {
    Vehicles vehicles;
    bool running = true;
    
    cout << "Reading File Data..." << endl;
    vehicles.traffic_signal_timings();
    vehicles.road_Network();
    vehicles.vehicles_csv();
    cout << "File read successfully" << endl;

    while (running) {
        cout << "\n=== Main Menu ===" << endl;
        cout << "1. Add Nodes (Graph)" << endl;
        cout << "2. Add Edges (Graph)" << endl;
        cout << "3. Display Graph" << endl;
        cout << "4. Delete Node (Graph)" << endl;
        cout << "5. Delete Edge (Graph)" << endl;
        cout << "6. Dijkstra's Algorithm (Graph)" << endl;
        cout << "7. Add Vehicle" << endl;
        cout << "8. Display Vehicles" << endl;
        cout << "9. Calculate Vehicle Routes" << endl;
        cout << "10. Simulation" << endl;
        cout << "11. Simulate Accident/Road Closure" << endl;
        cout << "12. Reopen Road" << endl;
        cout << "13. Exit" << endl;
        cout << "Choose an option: ";
        
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                vehicles.inputNodes();
                break;
            case 2:
                vehicles.inputEdges();
                break;
            case 3:
                vehicles.displayGraph();
                break;
            case 4: {
                cout << "Enter the name of the node to delete: ";
                string nodeName;
                cin >> nodeName;
                vehicles.DeleteNode(nodeName);
                break;
            }
            case 5: {
                string from, to;
                int weight;
                cout << "Enter the name of the From node: ";
                cin >> from;
                cout << "Enter the name of the To node: ";
                cin >> to;
                cout << "Enter the weight of the edge: ";
                cin >> weight;
                vehicles.DeleteEdge(from, to, weight);
                break;
            }
            case 6: {
                string startNode;
                string endNode;
                cout << "Enter the start node: ";
                cin >> startNode;
                cout << "Enter the end node: ";
                cin >> endNode;
                int distance;

                Nodes** path = vehicles.dijkstra(startNode, endNode, distance);

                if (path == nullptr) {
                    cout << "No path found!" << endl;
                } else {
                    cout << "Path: ";
                    int j = 0;
                    while (path[j] != nullptr) {
                        cout << path[j]->name;
                        j++;
                        if (path[j] != nullptr) {
                            cout << " -> ";
                        }
                    }
                    cout << endl;
                    cout << "Shortest Distance: " << distance << endl;
                    delete[] path;
                }
                break;
            }
            case 7: {
                vehicles.Input_Vehicle();
                break;
            }
            case 8: {
                vehicles.displayVehicles();
                break;
            }
            case 9: {
                vehicles.calcaulate_route();
                break;
            }
            case 10: {
                vehicles.simulate();
                break;
            }
            case 11: {
                string from, to;
                cout << "Enter the name of the From node: ";
                cin >> from;
                cout << "Enter the name of the To node: ";
                cin >> to;
                vehicles.blockRoad(from, to);
                break;
            }
            case 12: {
                string from, to;
                cout << "Enter the name of the From node: ";
                cin >> from;
                cout << "Enter the name of the To node: ";
                cin >> to;
                vehicles.reopenRoad(from, to);
                break;
            }
            case 13:
                running = false;
                break;
            default:
                cout << "Invalid option! Please choose a valid option." << endl;
                break;
        }
    }
    return 0;
}