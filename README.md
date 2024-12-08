# Traffic Simulation System Documentation

## Table of Contents

1. [Introduction](#introduction)
2. [Class Structure](#class-structure)
   - [Graph Class](#graph-class)
   - [Vehicles Class](#vehicles-class)
3. [Data Structures](#data-structures)
   - [Nodes](#nodes)
   - [Edges](#edges)
   - [VehiclesNode](#vehiclesnode)
4. [Functionality](#functionality)
   - [Graph Operations](#graph-operations)
   - [Vehicles Operations](#vehicles-operations)
   - [Simulation](#simulation)
5. [File Handling](#file-handling)
6. [Usage](#usage)
7. [Conclusion](#conclusion)

---

## Introduction

This documentation provides an in-depth explanation of the C++ code for a traffic simulation system. The system models intersections (nodes), roads (edges), and vehicles to simulate traffic flow, signal timing, and congestion management.

---

## Class Structure

### Graph Class

The Graph class represents the road network as a graph with nodes (intersections) and edges (roads). It includes methods for adding, deleting, and managing nodes and edges, as well as performing graph algorithms like BFS and Dijkstra's algorithm.

#### Member Functions:

- *Constructor and Destructor:*
  cpp
  Graph() : intersections(nullptr), streets(nullptr), numNodes(0), numEdges(0), nodeCapacity(10), edgeCapacity(10), vehicleCounts(nullptr), hashTableSize(100) {
      intersections = new Nodes[nodeCapacity];
      streets = new Edges[edgeCapacity];
      vehicleCounts = new int[hashTableSize];
      for (int i = 0; i < hashTableSize; i++) vehicleCounts[i] = 0;
  }

  ~Graph() {
      delete[] intersections;
      delete[] streets;
      delete[] vehicleCounts;
  }
  

- *Node Management:*
  cpp
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
  

- *Edge Management:*
  cpp
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
  

- *Graph Algorithms:*
  cpp
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
  

- *Congestion Management:*
  cpp
  void incrementVehicleCount(const string& fromName, const string& toName) {
      int index = hashFunction(fromName, toName);
      vehicleCounts[index]++;
  }

  void decrementVehicleCount(const string& fromName, const string& toName) {
      int index = hashFunction(fromName, toName);
      if (vehicleCounts[index] > 0) {
          vehicleCounts[index]--;
      }
  }

  bool isCongested(const string& fromName, const string& toName) {
      int index = hashFunction(fromName, toName);
      return vehicleCounts[index] >= streets[findEdgeIndex(fromName, toName)].capacity;
  }
  

- *Traffic Light Management:*
  cpp
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
  

- *File Handling:*
  cpp
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
  

### Vehicles Class

The Vehicles class extends the Graph class and manages vehicle data, including their routes and priorities. It includes methods for adding vehicles, calculating routes, and simulating vehicle movement.

#### Member Functions:

- *Constructor and Destructor:*
  cpp
  Vehicles() : Graph(), vehicle(new VehiclesNode[10]), capacity(10), numVehicles(0) {}

  ~Vehicles() {
      delete[] vehicle;
  }
  

- *Vehicle Management:*
  cpp
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
      string id, Start, End, p_level;
      while (true) {
          if (numVehicles == capacity) {
              IncreaseNumVehicles();
          }
          cout << "Enter Vehicle ID, Start Intersection, End Intersection and Priority_Level : ";
          cin >> id >> Start >> End >> p_level;
          if (id == "-1") {
              break;
          }
          createVehicles(id, Start, End, p_level);
          if (p_level != "High") {
              ofstream file("vehicles.csv", std::ios::app);
              file << id << "," << Start << "," << End << endl;
              file.close();
          } else {
              ofstream file1("emergency_vehicles.csv", std::ios::app);
              file1 << id << "," << Start << "," << End << "," << p_level << endl;
              file1.close();
          }
      }
  }
  

- *Route Calculation:*
  cpp
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
  

- *Simulation:*
  cpp
  void simulate() {
      int red_duration = 30;
      int max_simulation_time = 300;

      Nodes*** vehicle_paths = new Nodes**[numVehicles];
      int* path_lengths = new int[numVehicles];

      for (int i = 0; i < numVehicles; i++) {
          int distance;
          Nodes** path = dijkstra(vehicle[i].edge->from->name, vehicle[i].edge->to->name, distance);
          if (path == nullptr) {
              cerr << "No path found for vehicle " << vehicle[i].ID << endl;
              vehicle_paths[i] = nullptr;
              path_lengths[i] = 0;
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
                  cerr << "Invalid edge for vehicle " << vehicle[i].ID << endl;
                  continue;
              }
              int arrival_time = vehicle_departure_times[i] + edge_weight;
              if (arrival_time > current_simulation_time) {
                  continue;
              }
              if (vehicle[i].priorityLevel == "High") {
                  vehicle_positions[i]++;
                  vehicle_departure_times[i] = arrival_time + edge_weight;
                  cout << "Emergency Vehicle " << vehicle[i].ID << " moves to " << next_node->name
                            << " at time " << current_simulation_time << " seconds." << endl;
              } else {
                  if (!is_light_green(next_node, arrival_time, red_duration)) {
                      int wait_time = calculate_wait_time(next_node, arrival_time, red_duration);
                      vehicle_departure_times[i] += wait_time;
                      cout << "Vehicle " << vehicle[i].ID << " is waiting at " << next_node->name
                                << " for " << wait_time << " seconds." << endl;
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
                              cerr << "No alternative path found for vehicle " << vehicle[i].ID << endl;
                          }
                      } else {
                          vehicle_positions[i]++;
                          vehicle_departure_times[i] = arrival_time + edge_weight;
                          cout << "Vehicle " << vehicle[i].ID << " moves to " << next_node->name
                                    << " at time " << current_simulation_time << " seconds." << endl;
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

      for (int i = 0; i < numVehicles; i++) {
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
  

- *File Handling:*
  cpp
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
  

---

## Data Structures

### Nodes

- *Attributes:*
  - name: Name of the intersection.
  - timing: Green light timing for the intersection.

cpp
struct Nodes {
    string name;
    int timing;

    Nodes() : name(""), timing(0) {}
    Nodes(string n, int t) : name(n), timing(t) {}
};


### Edges

- *Attributes:*
  - from: Pointer to the starting node.
  - to: Pointer to the ending node.
  - weight: Time or distance for traversing the road.
  - capacity: Maximum number of vehicles allowed on the road.
  - isActive: Boolean indicating if the road is operational.

cpp
struct Edges {
    Nodes* from;
    Nodes* to;
    int weight;
    int capacity;
    bool isActive;

    Edges() : from(nullptr), to(nullptr), weight(0), capacity(3), isActive(true) {}
    Edges(Nodes* f, Nodes* t, int w) : from(f), to(t), weight(w), capacity(3), isActive(true) {}
};


### VehiclesNode

- *Attributes:*
  - ID: Unique identifier for the vehicle.
  - edge: Pointer to the edge the vehicle is currently on.
  - priorityLevel: Priority level of the vehicle (e.g., "High" for emergency vehicles).

cpp
struct VehiclesNode {
    string ID;
    Edges* edge;
    string priorityLevel;

    VehiclesNode() : edge(NULL), ID("") {}
    VehiclesNode(string id, Edges* e, string p) : ID(id), edge(e), priorityLevel(p) {}
};


---

## Functionality

### Graph Operations

- *Node and Edge Creation:*
  - Nodes and edges are created and stored in arrays, with capacities doubling when needed.
  
- *Path Finding:*
  - BFS is used for finding a basic path between two nodes.
  - Dijkstra's algorithm is used for finding the shortest path based on edge weights.

- *Congestion Handling:*
  - Vehicle counts on each road are tracked using a hash table.
  - Roads are marked as congested if the vehicle count exceeds the capacity.

- *Traffic Light Simulation:*
  - Traffic lights are simulated based on the green and red durations specified for each node.
  - Vehicles wait at red lights based on their arrival time.

### Vehicles Operations

- *Vehicle Addition:*
  - Vehicles are added with an ID, start node, end node, and priority level.
  - Vehicle data is read from CSV files.

- *Route Calculation:*
  - Routes are calculated using Dijkstra's algorithm and displayed for each vehicle.

- *Simulation:*
  - Vehicles move through the network based on their routes and traffic conditions.
  - Emergency vehicles bypass congestion and red lights.
  - The simulation runs in real-time, with a maximum simulation time.

### Simulation

- *Vehicle Movement:*
  - Vehicles move from their start node to the end node based on the calculated routes.
  - Movement is simulated with delays based on edge weights and traffic signals.

- *Accident and Road Closure Simulation:*
  - Roads can be blocked and reopened during the simulation.
  - Blocked roads are handled by finding alternative paths using BFS.

---

## File Handling

- *CSV Files:*
  - traffic_signal_timings.csv: Contains node names and green light timings.
  - road_network.csv: Contains road data with from node, to node, and weight.
  - vehicles.csv: Contains general vehicle data with ID, start node, and end node.
  - emergency_vehicles.csv: Contains emergency vehicle data with ID, start node, end node, and priority level.

- *File Operations:*
  - Data is read from CSV files during initialization.
  - Data is appended to CSV files when adding nodes, edges, and vehicles.
  - Files are updated when nodes or edges are deleted.

---

## Usage

1. *Initialization:*
   - The program reads node, edge, and vehicle data from CSV files during startup.

2. *Main Menu:*
   - Users can perform various operations such as adding nodes, edges, vehicles, deleting nodes/edges, calculating routes, and simulating traffic.

3. *Simulation:*
   - The simulation runs in real-time, displaying vehicle movements and waiting times.
   - Users can simulate accidents by blocking roads and reopening them.

4. *Exit:*
   - The program exits gracefully, freeing all allocated memory.

---

## Conclusion

This traffic simulation system models a real-world road network with intersections, roads, and vehicles. It provides a comprehensive set of features for managing traffic flow, signal timing, and congestion, with a focus on real-time simulation and user interaction. The system is designed to handle dynamic changes in the network, such as road closures and the introduction of emergency vehicles.
