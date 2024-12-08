#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <chrono>   
#include <thread>   // For Running multiple traffic simulations at a time
#include <ctime>    // To handle time in simulations

using namespace std;

struct Nodes {
    string name; // Intersection name
    int timing;  // Green light timing

    Nodes() : name(""), timing(0) {} 
    Nodes(string n, int t) : name(n), timing(t) {} 
};

struct Edges {
    Nodes* from; // Starting point of the road
    Nodes* to;   // Ending point of the road
    int weight;  // Weight or time for the road
    int capacity; // Max limit for vehicles
    bool isActive; // If the road is operational

    Edges() : from(nullptr), to(nullptr), weight(0), capacity(3), isActive(true) {} // Default constructor
    Edges(Nodes* f, Nodes* t, int w) : from(f), to(t), weight(w), capacity(3), isActive(true) {} // Constructor with values
};

struct VehiclesNode {
    string ID; // Vehicle ID
    Edges* edge; // The road it is on
    string priorityLevel; // Vehicles Priority e.g Ambulance might have a higher priority

    VehiclesNode() : edge(NULL), ID("") {} 
    VehiclesNode(string id, Edges* e, string p) : ID(id), edge(e), priorityLevel(p) {} 
};

class Graph {
private:
    Nodes* intersections; // Array of intersections
    Edges* streets; // Array of roads, for example if from has A and To has D than the street will be from A to D
    int numNodes; // Current number of intersections
    int numEdges; // Current number of roads
    int nodeCapacity; // Maximum number of intersections
    int edgeCapacity; // Maximum number of roads
    int* vehicleCounts; // Number of vehicles per road
    int hashTableSize; // Size of the hash table

    void resizeNodes() {   // increasing capacity of intersection
        nodeCapacity *= 2; 
        Nodes* temp = new Nodes[nodeCapacity]; // Create a new larger array
        for (int i = 0; i < numNodes; i++) temp[i] = intersections[i]; 
        delete[] intersections; // Free old memory
        intersections = temp; // Update to new array
    }

    void resizeEdges() { // Increase capacity of roads array
        edgeCapacity *= 2; 
        Edges* temp = new Edges[edgeCapacity]; 
        for (int i = 0; i < numEdges; i++) temp[i] = streets[i];
        delete[] streets;
        streets = temp; 
    }

    int hashFunction(const string& fromName, const string& toName) {
        /* Simple hash function using the
         sum of ASCII values of the names*/
        int hash = 0;
        for (char c : fromName) hash += c;
        for (char c : toName) hash += c;
        return hash % hashTableSize;
    }

public:
    Graph() : intersections(nullptr), streets(nullptr), numNodes(0), numEdges(0), nodeCapacity(10), edgeCapacity(10), vehicleCounts(nullptr), hashTableSize(100) {
        intersections = new Nodes[nodeCapacity]; // Initialize intersections with initial capacity of 10
        streets = new Edges[edgeCapacity]; // Initialize roads....
        vehicleCounts = new int[hashTableSize]; // Initialize vehicle counts...
        for (int i = 0; i < hashTableSize; i++) vehicleCounts[i] = 0; // Initialize all counts to 0
    }

    ~Graph() { // For memory release
        delete[] intersections; // Free intersections array
        delete[] streets; // Free roads array
        delete[] vehicleCounts; // Free vehicle counts
    }
     int getnumEdges() {
    return numEdges;  // Just returning the number of edges here.
    }

    Edges getStreets(int i) {
        return streets[i];  // Returning the street of required index
    }

    Nodes* findNode(const string& name) {
        for (int i = 0; i < numNodes; i++) {  // looping through all nodes to find desired one
            if (intersections[i].name == name) {
                  return &intersections[i]; 
            }
        }
    return nullptr;  // If not found
    }

    int findEdgeIndex(const string& fromName, const string& toName) {
            for (int i = 0; i < numEdges; i++) {  // Searching for the edge with the matching 'from' and 'to' names.
                if (streets[i].from->name == fromName && streets[i].to->name == toName) {
                    return i;  
                }
            }
    return -1;  // if not found.
    }

    void incrementVehicleCount(const string& fromName, const string& toName) {
            int index = hashFunction(fromName, toName);  // First, find the index of the edge.
            vehicleCounts[index]++;  // If the edge exists, increment the vehicle count.
        }

    void decrementVehicleCount(const string& fromName, const string& toName) {
        int index = hashFunction(fromName, toName);  // Find the index of the edge.
        if (vehicleCounts[index] > 0) {
            vehicleCounts[index]--;  // Decrease the vehicle count if it's above 0.
        }
    }

    bool isCongested(const string& fromName, const string& toName) {
        int index = hashFunction(fromName, toName);  // again finding edge's index first.
        return vehicleCounts[index] >= streets[findEdgeIndex(fromName, toName)].capacity;  // Return true if the vehicles are more than or equal to capacity.
    }

    int findNodeIndex(const string& name) {
        for (int i = 0; i < numNodes; i++) {  // Searching through nodes by name.
            if (intersections[i].name == name) {
                 return i;  // Return the index if found.
            }
        }
        return -1;  // if not found.
    }

    Nodes** bfs(const string& startName, const string& endName) {
        const int MAX_NODES = 100;
        Nodes* queue[MAX_NODES];
        Nodes* parent[MAX_NODES];
        bool visited[MAX_NODES] = {false};  // Array to track nodes that were already visited.
        int front = 0, rear = 0;  // Queue pointers for BFS.

        Nodes* startNode = findNode(startName);  // Find the starting node.
        Nodes* endNode = findNode(endName);  // Find the destination node.
        if (startNode == nullptr || endNode == nullptr) {
            return nullptr;  // If either node is not found, return null.
        }

        int startIndex = findNodeIndex(startName);
        int endIndex = findNodeIndex(endName);

        queue[rear++] = startNode;  // Add the start node to the queue.
        visited[startIndex] = true;  // Mark the start node as visited.
        parent[startIndex] = nullptr;

        while (front != rear) {  // Standard BFS loop.
        Nodes* current = queue[front++];  // Dequeue the next node.
        int currentIndex = findNodeIndex(current->name);

        if (current == endNode) {  // If we've reached the destination, build the path.
                int pathLength = 0;
                Nodes* path[MAX_NODES];
                Nodes* node = endNode;
                while (node != nullptr) {
                    path[pathLength++] = node;
                    node = parent[findNodeIndex(node->name)];
                }
                Nodes** result = new Nodes*[pathLength + 1];
                for (int i = 0; i < pathLength; i++) {
                    result[i] = path[pathLength - 1 - i];  // Reverse the path to get the correct order.
                }
                result[pathLength] = nullptr;  // Null-terminate the result.
                return result;
            }

        for (int i = 0; i < numEdges; i++) {  // Loop through all edges to find unvisited neighbors.
                if (streets[i].from == current && streets[i].isActive && !visited[findNodeIndex(streets[i].to->name)]) {
                    Nodes* neighbor = streets[i].to;
                    int neighborIndex = findNodeIndex(neighbor->name);
                    queue[rear++] = neighbor;  // Enqueue the neighbor.
                    visited[neighborIndex] = true;  // Mark as visited.
                    parent[neighborIndex] = current;  // Set the parent node.
                }
            }
        }
        return nullptr;  // Return null if no path is found.
    }

    void createNode(const string& name, int timings) {
        if (findNode(name) != nullptr) {  // Check if the node already exists.
            cout << "Node with the name '" << name << "' already exists!" << endl;
            return;
        }   

        if (numNodes == nodeCapacity) {  // Resize the array if needed.
            resizeNodes();
        }

        intersections[numNodes++] = Nodes(name, timings);  // Add the new node.
        cout << "Node '" << name << "' has been added." << endl;
    }

    void createEdges(const string& from, const string& to, int weight) {
        Nodes* fromNode = findNode(from);
        Nodes* toNode = findNode(to);

        if (fromNode && toNode) {  // Check if both nodes exist.
            for (int i = 0; i < numEdges; i++) {  // Check if the edge already exists.
                if (streets[i].from == fromNode && streets[i].to == toNode) {
                    cout << "Edge from '" << from << "' to '" << to << "' already exists. Updating weight to " << weight << "." << endl;
                    streets[i].weight = weight;  // Update the weight if the edge is found.
                    return;
                }
            }

        if (numEdges == edgeCapacity) {  // Resize if needed.
            resizeEdges();
        }
        streets[numEdges++] = Edges(fromNode, toNode, weight);  // Add the new edge.
        cout << "Edge from '" << from << "' to '" << to << "' with weight " << weight << " has been added." << endl;
        }
        else {
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
            if (name == "-1") break;  // Break if user types -1.
            cout << "Enter Green Time: ";
            cin >> timings;
            createNode(name, timings);  // Create the node and store in the file.
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
            if (from == "-1") break;  // Stop if -1 is typed.
            cin >> to >> weight;
            createEdges(from, to, weight);  // Create the edge and store in the file.
            ofstream file("road_network.csv", std::ios::app);
            file << from << "," << to << "," << weight << endl; 
            file.close();
        }
    }

    void displayGraph() {
        cout << "Nodes:" << endl;
        for (int i = 0; i < numNodes; i++) {
            cout << i << ": " << intersections[i].name << endl;  // Display each node.
        }

        cout << "Edges:" << endl;
        for (int i = 0; i < numEdges; i++) {
            cout << i << ": " << streets[i].from->name << " -> "
            << streets[i].to->name
            << " (Weight: " << streets[i].weight << ")" << endl;  // Display each edge.
        }
    }


    void DeleteNode(const string& name) {
        Nodes* node = findNode(name); // First, find the node
        if (node == nullptr) {
            cout << "Node not found!" << endl; // If node is not found, show error and return
            return;
        }

        int i = 0;
        while (i < numEdges) { // Loop through all edges to remove the ones connected to this node
            if (streets[i].from == node || streets[i].to == node) {
                for (int j = i; j < numEdges - 1; j++) {
                    streets[j] = streets[j + 1]; // Shift edges to remove the deleted one
                }
                numEdges--; // Reduce the edge count
            } else {
                i++; // If no match, move to the next edge
            }
        }

        int nodeIndex = -1;
        for (int i = 0; i < numNodes; i++) {
            if (intersections[i].name == name) {
                nodeIndex = i; // Get the index of the node to be deleted
                break;
            }
        }

        if (nodeIndex != -1) {
            for (int i = nodeIndex; i < numNodes - 1; i++) { // Shift nodes to fill the gap of the deleted node
                intersections[i] = intersections[i + 1];
            }
            numNodes--; // Reduce the node count
        }

        /* Update the node and 
        edge files after deletion*/
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
            getline(file1, header); // Skip the header row
        }
        
        while (file1.good()) {
            getline(file1, node, ','); // Read node name
            file1 >> timings;
            file1.ignore(__LONG_MAX__, '\n'); // Move to the next line
            if (!node.empty()) {
                createNode(node, timings); // Create node with the read data
            }
        }
    }

    void road_Network() {
        ifstream file("road_network.csv");
        string from_node, to_node;
        int weight;

        if (file.good()) {
            string header;
            getline(file, header); // Skip the header row
        }

        while (file.good()) {
            getline(file, from_node, ','); 
            getline(file, to_node, ','); 
            file >> weight; 
            file.ignore(__LONG_MAX__, '\n'); // Move to the next line
            if (!from_node.empty() && !to_node.empty()) {
                createEdges(from_node, to_node, weight); // Create edge with the read data
            }
        }
        file.close();
    }

    Edges* findEdge(Nodes* from, Nodes* to) {
        for (int i = 0; i < numEdges; i++) { // Find the edge that connects two nodes
            if (streets[i].from == from && streets[i].to == to) {
                return &streets[i]; // Return the edge if found
            }
        }
        return nullptr; // if no edge is found
    }

    void DeleteEdge(const string& fromNode, const string& toNode, int weight) {
        bool edgeDeleted = false;
        int i = 0;
        /*Loop through edges to find the one that
         matches the 'from', 'to', and 'weight'*/ 
        while (i < numEdges) {
            if (streets[i].from->name == fromNode && streets[i].to->name == toNode && streets[i].weight == weight) {
                for (int j = i; j < numEdges - 1; j++) {
                    streets[j] = streets[j + 1]; // Shift edges to remove the one being deleted
                }
                numEdges--; // Reduce edge count
                edgeDeleted = true;
                cout << "Edge " << fromNode << " -> " << toNode << " (Weight: " << weight << ") has been deleted from the graph." << endl;
            } else {
                i++; // Move to the next edge if no match
            }
        }

        if (!edgeDeleted) {
            cout << "No edge found between " << fromNode << " and " << toNode << " with weight " << weight << "." << endl;
        }

        updateEdgeFile(fromNode); // Update the edge file after deletion
    }

    void updateEdgeFile(const string& nodeName) {
        ifstream infile("road_network.csv");
        ofstream outfile("temp.csv");

        string from, to, header;
        int weight;

        if (getline(infile, header)) {
            outfile << header << endl; // Copy the header to the temp file
        }

        /* Copy all edges except the
         ones related to 'nodeName'*/
        while (getline(infile, from, ',')) {
            getline(infile, to, ',');
            infile >> weight;
            infile.ignore(__LONG_MAX__, '\n');

            if (from == nodeName || to == nodeName) {
                continue; // Skip if it's the edge to delete
            }

            outfile << from << "," << to << "," << weight << endl; // Write the valid edge to the temp file
        }

        infile.close();
        outfile.close();

        remove("road_network.csv"); // Delete old file
        rename("temp.csv", "road_network.csv"); // Rename temp file to original file name
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
                outfile << header << endl; // Write the header to the temp file
                headerRead = true;
            }
            
            getline(infile, node, ','); // Read node name
            if (node.empty()) continue; // Skip if node is empty
            
            infile >> timings; // Read timings
            infile.ignore(__LONG_MAX__, '\n'); // Move to the next line

            if (node != nodeName) {
                outfile << node << "," << timings << endl; // Write the valid node to the temp file
            }
        }

        infile.close();
        outfile.close();

        remove("traffic_signal_timings.csv"); // Delete old file
        rename("temp.csv", "traffic_signal_timings.csv"); // Rename temp file to original file name
    }

    Nodes** dijkstra(const string& fromNodeName, const string& toNodeName, int& pathLength) {
        const int INF = 999999; // Use a big number to represent infinity
        int* dist = new int[numNodes]; // Distance array for shortest paths
        bool* visited = new bool[numNodes]; // Track visited nodes
        int* predecessor = new int[numNodes]; // To store the previous node in the path

        for (int i = 0; i < numNodes; i++) { // Initialize arrays with default values
            dist[i] = INF; // Set all distances to infinity
            visited[i] = false; // Mark all nodes as unvisited
            predecessor[i] = -1; // No predecessors yet
        }

        int startIdx = -1, endIdx = -1; 
        for (int i = 0; i < numNodes; i++) {    // Find the index of the start and end nodes
            if (intersections[i].name == fromNodeName) {
                startIdx = i;
            }
            if (intersections[i].name == toNodeName) {
                endIdx = i;
            }
        }
        /*If start or end node is not found,
         free memory and return null*/ 
        if (startIdx == -1 || endIdx == -1) {
            delete[] dist;
            delete[] visited;
            delete[] predecessor;
            pathLength = 0;
            return nullptr;
        }

        dist[startIdx] = 0; // Distance to the start node is 0

        for (int i = 0; i < numNodes; i++) { // Start the Dijkstra's algorithm
            int u = -1, minDist = INF;
            for (int j = 0; j < numNodes; j++) { // Find the unvisited node with the smallest distance
                if (!visited[j] && dist[j] < minDist) {
                    minDist = dist[j];
                    u = j;
                }
            }

            if (u == -1) break; // If no such node, exit

            visited[u] = true; // Mark this node as visited

            for (int j = 0; j < numEdges; j++) { // Update distances for neighboring nodes
                if (streets[j].from == &intersections[u] && streets[j].isActive) {
                    int v = -1;
                    for (int k = 0; k < numNodes; k++) {  // Find the neighboring node index
                        if (streets[j].to == &intersections[k]) {
                            v = k;
                            break;
                        }
                    }

                    /* Relaxation step: update the distance
                     if a shorter path is found*/
                    if (v != -1 && !visited[v] && dist[u] + streets[j].weight < dist[v]) {
                        dist[v] = dist[u] + streets[j].weight;
                        predecessor[v] = u; // Update predecessor for path tracking
                    }
                }
            }
        }
        // If the destination node is not reachable
        if (dist[endIdx] == INF) {
            delete[] dist;
            delete[] visited;
            delete[] predecessor;
            pathLength = 0;
            return nullptr;
        }
        pathLength = dist[endIdx]; // Set the shortest path length
        /* Calculate the path length by
         backtracking from the destination*/
        int tempPathLength = 0;
        int currIdx = endIdx;
        while (currIdx != -1) {
            tempPathLength++;
            currIdx = predecessor[currIdx];
        }

        // Create a path array and backtrack to fill it
        Nodes** path = new Nodes*[tempPathLength + 1];
        currIdx = endIdx;
        for (int i = tempPathLength - 1; i >= 0; i--) {
            path[i] = &intersections[currIdx]; // Add node to path
            currIdx = predecessor[currIdx]; // Move to the previous node
        }
        path[tempPathLength] = nullptr; // Null-terminate the path

        // Free memory
        delete[] dist;
        delete[] visited;
        delete[] predecessor;

        return path; // Return the shortest path
    }

    void displayCongestion() {
        /* Loop through each road 
        and display the vehicle count*/
        for (int i = 0; i < numEdges; i++) {
            int index = hashFunction(streets[i].from->name, streets[i].to->name);
            cout << "Road " << streets[i].from->name << "-" << streets[i].to->name << ": "
                 << vehicleCounts[index] << " vehicles";
            // Check if the road is congested
            if (isCongested(streets[i].from->name, streets[i].to->name)) {
                cout << " (Congested)"; // Mark as congested if true
            }
            cout << endl;
        }
    }

    void blockRoad(const string& fromName, const string& toName) {
        int index = findEdgeIndex(fromName, toName); // Find the index of the start and end nodes
        if (index != -1) {
            streets[index].isActive = false; // Set road as inactive
            cout << "Road from " << fromName << " to " << toName << " has been closed." << endl;
        } else {
            cout << "Edge not found." << endl; // If no such road, show error
        }
    }

    void reopenRoad(const string& fromName, const string& toName) {
        int index = findEdgeIndex(fromName, toName); // Find the index of the road to reopen
        if (index != -1) {
            streets[index].isActive = true; // Set road as active
            cout << "Road from " << fromName << " to " << toName << " has been reopened." << endl;
        } else {
            cout << "Edge not found." << endl; // If no such road, show error
        }
    }

    bool isEdgeActive(const string& fromName, const string& toName) {
        int index = findEdgeIndex(fromName, toName);  // Find the index of the road to reopen
        if (index != -1) {
            return streets[index].isActive; // Return true if the road is active
        }
        return false; // Return false if no such road is found.
    }
};

class Vehicles : public Graph {
private:
    VehiclesNode *vehicle;
    int capacity;
    int numVehicles;

    void IncreaseNumVehicles() {     // Increase the array size when it's full
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
    Vehicles() : Graph(), vehicle(new VehiclesNode[10]), capacity(10), numVehicles(0) {} // Constructor to initialize vehicle array with capacity 10

    ~Vehicles() {  // Destructor to cleaning up
        delete[] vehicle;
    }
       
    void createVehicles(const string& name, string& from_node, string& to_node, string priorityLevel) {  // Creating a new gari
        Nodes* Start = findNode(from_node);
        Nodes* End = findNode(to_node);
        /* If either start or end node is not found,
         show an error message*/
        if (Start == nullptr || End == nullptr) { 
            cout << "One or both Nodes not found!" << endl;
            return;
        }

        if (numVehicles >= capacity) {
            IncreaseNumVehicles();  // If vehicle array is full, increase its capacity
        }

        VehiclesNode NewVehicle;  // Create a new vehicle and set its properties
        NewVehicle.ID = name;
        NewVehicle.edge = new Edges(Start, End, 0);
        NewVehicle.priorityLevel = priorityLevel;
        vehicle[numVehicles++] = NewVehicle;
    }

        void Input_Vehicle() {  // Create a new vehicle and set its properties
        string id, Start, End, p_level;
        while (true) {
            if (numVehicles == capacity) {
                IncreaseNumVehicles();
            }

            cout << "Enter Vehicle ID, Start Intersection, End Intersection and Priority_Level : "; 
            cin >> id >> Start >> End >> p_level;   // Create a new vehicle and set its properties
            if (id == "-1") {
                break; 
            }
            createVehicles(id, Start, End, p_level); // Default priority level is "Low"
            if(p_level != "High"){
                ofstream file("vehicles.csv", std::ios::app);
                file << id << "," << Start << "," << End << endl; 
                file.close();
            }
            else{
                ofstream file1("emergency_vehicles.csv", std::ios::app);
                file1 << id << "," << Start << "," << End << "," << p_level << endl; 
                file1.close();
            }
            
        }
    }
    void calcaulate_route() {   // Calculate the shortest route for each vehicle
        int distance;
        for (int i = 0; i < numVehicles; i++) {
            cout << "Vehicle with id " << vehicle[i].ID << endl;

            /*Use Dijkstra's algorithm
             to find the path*/ 
            Nodes** path = dijkstra(vehicle[i].edge->from->name, vehicle[i].edge->to->name, distance);

            if (path == nullptr) {
                cout << "Node is Unreachable" << endl;
            } 
            else {     // Display the path and the distance
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

                delete[] path;  // Clean up the allocated path array
            }
        }
    }

    void vehicles_csv() {   // Read vehicle data from CSV files
        ifstream file("vehicles.csv");
        string header;
        file >> header;
        string id, start, end;
        string priorityLevel;

        while (file.good()) {  // Process general vehicle data
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

        while (file1.good()) {  // Process emergency vehicle data
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

    void displayVehicles() { // Display the list of all vehicles
        if (numVehicles == 0) {
            cout << "No vehicles available." << endl;
            return;
        }
        cout << "Vehicles in the system:" << endl;
        for (int i = 0; i < numVehicles; i++) {  // Loop through each vehicle and print its details
            cout << "ID: " << vehicle[i].ID << ", Start: " << vehicle[i].edge->from->name
                << ", End: " << vehicle[i].edge->to->name << ", Priority: " << vehicle[i].priorityLevel << endl;
        }
    }
    /*Calculate the total travel
     time for a vehicle along a path*/ 
    int calculate_travel_time(Nodes** path, string start_name, string end_name, int red_duration) {
        int total_time = 0;
        int departure_time = 0;

        for (int index = 0; path[index + 1] != nullptr; index++) {  // Traverse each segment of the path
            Nodes* current_node = path[index];
            Nodes* next_node = path[index + 1];
            int edge_weight = get_edge_weight(current_node, next_node);
            if (edge_weight == -1) return -1;

            int travel_time = edge_weight;
            int arrival_time = departure_time + travel_time;

            if (!is_light_green(next_node, arrival_time, red_duration)) { // Check traffic light condition 
                int wait_time = calculate_wait_time(next_node, arrival_time, red_duration);  //calculate waiting time
                total_time += travel_time + wait_time;
                departure_time = arrival_time + wait_time;
            } 
            else {
                total_time += travel_time;
                departure_time = arrival_time;
            }
        }
        return total_time;
    }

    int get_edge_weight(Nodes* from, Nodes* to) {   // Get the weight (time) of a road segment between two nodes
        for (int i = 0; i < getnumEdges(); i++) {
            if (getStreets(i).from == from && getStreets(i).to == to) {
                return getStreets(i).weight;
            }
        }
        return -1;
    }

    /* Check if the traffic light is green when 
    a vehicle arrives at a node*/
    bool is_light_green(Nodes* node, int arrival_time, int red_duration) {
        if (node == nullptr) {
            cerr << "Null node in is_light_green" << endl;
            return false;
        }
        int total_cycle = node->timing + red_duration;
        int time_in_cycle = arrival_time % total_cycle;
        return time_in_cycle < node->timing;
    }
    /* Calculate how long a vehicle
     needs to wait for a green light*/
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
    int red_duration = 30; // Red light duration and maximum simulation time
    int max_simulation_time = 300;

    Nodes*** vehicle_paths = new Nodes**[numVehicles];  // Arrays to store the paths and their lengths for each vehicle
    int* path_lengths = new int[numVehicles];

    for (int i = 0; i < numVehicles; i++) { // Calculate paths for each vehicle
        int distance;
        Nodes** path = dijkstra(vehicle[i].edge->from->name, vehicle[i].edge->to->name, distance);

        if (path == nullptr) {    // If no path is found, print an error and skip the vehicle
            cerr << "No path found for vehicle " << vehicle[i].ID << endl;
            vehicle_paths[i] = nullptr;
            path_lengths[i] = 0;
            continue;
        }

        int path_length = 0;  // Count the length of the path and copy it to vehicle_paths
        while (path[path_length] != nullptr) {
            path_length++;
        }
        vehicle_paths[i] = new Nodes*[path_length + 1];
        for (int j = 0; j < path_length; j++) {
            vehicle_paths[i][j] = path[j];
        }
        vehicle_paths[i][path_length] = nullptr;
        path_lengths[i] = path_length;

        delete[] path;   // Free the original path array
    }

    int* vehicle_positions = new int[numVehicles]; // Initialize vehicle positions and departure times
    int* vehicle_departure_times = new int[numVehicles];
    for (int i = 0; i < numVehicles; i++) {
        vehicle_positions[i] = 0;
        vehicle_departure_times[i] = 0;
    }

    auto start_time = std::chrono::high_resolution_clock::now();  // Get the start time of the simulation
    bool all_arrived = false;
    
    while (!all_arrived) {  // Continue the simulation until all vehicles reach their destinations
        auto current_real_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_real = current_real_time - start_time;
        int current_simulation_time = static_cast<int>(elapsed_real.count());

        all_arrived = true; // Check if all vehicles have reached their destinations
        for (int i = 0; i < numVehicles; i++) {
            if (vehicle_positions[i] < path_lengths[i] - 1) {
                all_arrived = false;
                break;
            }
        }
        if (all_arrived) {
            break;
        }

        for (int i = 0; i < numVehicles; i++) {  // Process each vehicle's movement
            if (vehicle_positions[i] >= path_lengths[i] - 1) {
                continue; // Skip if the vehicle has already reached its desired path xD
            }

            Nodes* current_node = vehicle_paths[i][vehicle_positions[i]];
            Nodes* next_node = vehicle_paths[i][vehicle_positions[i] + 1];
            int edge_weight = get_edge_weight(current_node, next_node);

            if (edge_weight == -1) { // If no valid edge, show an error and skip this vehicle
                cerr << "Invalid edge for vehicle " << vehicle[i].ID << endl;
                continue;
            }

            int arrival_time = vehicle_departure_times[i] + edge_weight;

            if (arrival_time > current_simulation_time) {  // If the vehicle hasn't reached the next node yet, skip to the next iteration
                continue;
            }

            if (vehicle[i].priorityLevel == "High") {  // Handle emergency vehicles (high priority)
                vehicle_positions[i]++;
                vehicle_departure_times[i] = arrival_time + edge_weight;
                cout << "Emergency Vehicle " << vehicle[i].ID << " moves to " << next_node->name
                          << " at time " << current_simulation_time << " seconds." << endl;
            } 
            else { // Handle normal vehicles
                if (!is_light_green(next_node, arrival_time, red_duration)) { 
                    int wait_time = calculate_wait_time(next_node, arrival_time, red_duration);
                    vehicle_departure_times[i] += wait_time;
                    cout << "Vehicle " << vehicle[i].ID << " is waiting at " << next_node->name
                              << " for " << wait_time << " seconds." << endl;
                } 
                else {   // Check for congestion and find an alternative path if needed
                    if (isCongested(current_node->name, next_node->name)) {
                        Nodes** alternative_path = bfs(current_node->name, next_node->name);
                        if (alternative_path != nullptr) {
                            delete[] vehicle_paths[i];
                            vehicle_paths[i] = alternative_path;

                            // Update the path length and reset the position for rerouting
                            path_lengths[i] = 0;
                            while (alternative_path[path_lengths[i]] != nullptr) {
                                path_lengths[i]++;
                            }
                            vehicle_positions[i] = 0;
                            vehicle_departure_times[i] = current_simulation_time;
                        }
                        else {
                            cerr << "No alternative path found for vehicle " << vehicle[i].ID << endl;
                        }
                    } 
                    else {
                        vehicle_positions[i]++;
                        vehicle_departure_times[i] = arrival_time + edge_weight;
                        cout << "Vehicle " << vehicle[i].ID << " moves to " << next_node->name
                                  << " at time " << current_simulation_time << " seconds." << endl;
                    }
                }
            }
        }

        auto current_time = std::chrono::high_resolution_clock::now(); // Check if the simulation time has come at exact time or not
        std::chrono::duration<double> elapsed = current_time - start_time;
        double simulation_time = elapsed.count();
        if (simulation_time < max_simulation_time) {
            std::this_thread::sleep_for(std::chrono::seconds(1));  // Simulate real time by sleeping 1 second 
        } else {
            break;
        }
    }

    for (int i = 0; i < numVehicles; i++) { // After the simulation, print the paths for each vehicle
        if (vehicle_paths[i] == nullptr) {
            cerr << "No path found for vehicle " << vehicle[i].ID << endl;
            continue;
        }
        cout << "Vehicle " << vehicle[i].ID << " path: ";
        int j;
        for (j = 0; j < path_lengths[i] - 1; j++) {
            cout << vehicle_paths[i][j]->name << " -> ";
        }
        cout << vehicle_paths[i][j]->name << " Destination Reached." << endl;
        cout << endl;
    }

    for (int i = 0; i < numVehicles; i++) { // Clean up memory 
        if (vehicle_paths[i] != nullptr) {
            delete[] vehicle_paths[i];
        }
    }
    delete[] vehicle_paths;
    delete[] path_lengths;
    delete[] vehicle_positions;
    delete[] vehicle_departure_times;

    cout << "Simulation complete." << endl;
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