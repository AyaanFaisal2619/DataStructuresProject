#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// Structure for Nodes (Intersections)
struct Nodes {
    string name;  // Name of the intersection

    Nodes() : name("") {}
    Nodes(string n) : name(n) {}
};

// Structure for Edges (Streets)
struct Edges {
    Nodes* from;  // Pointer to the starting node
    Nodes* to;    // Pointer to the ending node
    int weight;   // Weight of the edge (e.g., distance or time)

    Edges() : from(nullptr), to(nullptr), weight(0) {}
    Edges(Nodes* f, Nodes* t, int w) : from(f), to(t), weight(w) {}
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

    void resizeNodes() {
        // Double the capacity when the array is full
        nodeCapacity *= 2;
        Nodes* temp = new Nodes[nodeCapacity];
        for (int i = 0; i < numNodes; i++) {
            temp[i] = intersections[i];  // Copy the existing nodes
        }
        delete[] intersections;  // Free the old array
        intersections = temp;  // Point to the new array
    }

    void resizeEdges() {
        // Double the capacity when the array is full
        edgeCapacity *= 2;
        Edges* temp = new Edges[edgeCapacity];  // Create a new array for edges
        for (int i = 0; i < numEdges; i++) {  // Copy the existing edges
            temp[i] = streets[i];
        }
        delete[] streets;  // Free the old array
        streets = temp;  // Point to the new array
    }

public:
    // Constructor
    Graph() : intersections(nullptr), streets(nullptr), numNodes(0), numEdges(0), nodeCapacity(10), edgeCapacity(10) {
        intersections = new Nodes[nodeCapacity];
        streets = new Edges[edgeCapacity];
    }

    // Destructor
    ~Graph() {
        delete[] intersections;
        delete[] streets;
    }

    // Find a node by name
    Nodes* findNode(const string& name) {
        for (int i = 0; i < numNodes; i++) {
            if (intersections[i].name == name) {
                return &intersections[i];
            }
        }
        return nullptr;
    }

    // Create a new node
    void createNode(const string& name) {
        // Check if the node already exists
        if (findNode(name) != nullptr) {
            cout << "Node with the name '" << name << "' already exists!" << endl;
            return;
        }

        // Dynamically expand the node array if needed
        if (numNodes == nodeCapacity) {
            resizeNodes();
        }

        // Create a new node and add it to the array
        intersections[numNodes++] = Nodes(name);
        cout << "Node '" << name << "' has been added." << endl;
    }

    // Create an edge between two nodes
    void createEdges(const string& from, const string& to, int weight) {
        Nodes* fromNode = findNode(from);
        Nodes* toNode = findNode(to);
        if (fromNode && toNode) {
            if (numEdges == edgeCapacity) {
                resizeEdges();
            }
            streets[numEdges++] = Edges(fromNode, toNode, weight);
        } else {
            cout << "Error: One or both nodes not found!" << endl;
        }
    }

    // Input nodes from user
    void inputNodes() {
        cout << "Enter node names (type -1 to stop):" << endl;

        // Dynamically allocate nodes array
        string name;
        while (true) {
            cout << "Enter node name: ";
            cin >> name;
            if (name == "-1") break;

            createNode(name);  // Call the createNode function to add a node
        }
    }

    // Input edges from user
    void inputEdges() {
        cout << "Enter edges in the format: from to weight (type -1 to stop):" << endl;

        string from, to;
        int weight;
        while (true) {
            cout << "Enter edge (from to weight): ";
            cin >> from;
            if (from == "-1") break;
            cin >> to >> weight;

            createEdges(from, to, weight);  // Call the createEdges function to add an edge
        }
    }

    // Display the graph
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

    // Delete a node and its associated edges
    void DeleteNode(const string& name) {
        // Find the node
        Nodes* node = findNode(name);
        if (node == nullptr) {
            cout << "Node not found!" << endl;
            return;
        }

        // Remove edges connected to the node
        int i = 0;
        while (i < numEdges) {
            if (streets[i].from == node || streets[i].to == node) {
                // Shift elements to remove the edge
                for (int j = i; j < numEdges - 1; j++) {
                    streets[j] = streets[j + 1];
                }
                numEdges--;
            } else {
                i++; // Only increment if we don't remove the edge (to avoid skipping)
            }
        }

        // Remove the node from the intersections array
        int nodeIndex = -1;
        for (int i = 0; i < numNodes; i++) {
            if (intersections[i].name == name) {
                nodeIndex = i;
                break;
            }
        }

        if (nodeIndex != -1) {
            // Shift nodes to remove the deleted node
            for (int i = nodeIndex; i < numNodes - 1; i++) {
                intersections[i] = intersections[i + 1];
            }
            numNodes--;
        }

        cout << "Node " << name << " and its associated edges have been deleted." << endl;
    }

    // Load the road network from a CSV file
    void road_Network() {
    ifstream file("road_network.csv");
    string node1, node2;
    int weight;

    // Skip the header line (if there's one)
    if (file.good()) {
        string header;
        getline(file, header);
    }

    while (file.good()) {
        // Read node1, node2, and weight in a loop
        getline(file, node1, ',');  // Get the first node (before comma)
        getline(file, node2, ',');  // Get the second node (after comma)
        file >> weight;             // Get the weight (after the comma)
        
        // Skip the newline character after weight
        file.ignore(__LONG_MAX__,'\n');

        if (!node1.empty() && !node2.empty()) {
            createNode(node1);  // Create the first node if not already created
            createNode(node2);  // Create the second node if not already created
            createEdges(node1, node2, weight);  // Create the edge between the nodes
        }
    }

    file.close();  // Ensure to close the file after processing
}
};

// Main function with interactive menu
int main() {
    Graph graph;
    bool running = true;
    cout<<"Reading File Data..."<<endl;
    graph.road_Network();
    cout<<"File read successfully"<<endl;
    while (running) {
        cout << "\n=== Graph Menu ===" << endl;
        cout << "1. Add Nodes" << endl;
        cout << "2. Add Edges" << endl;
        cout << "3. Display Graph" << endl;
        cout << "4. Delete Node" << endl;
        cout << "5. Exit" << endl;
        cout << "Choose an option: ";
        int choice;
        cin >> choice;

        switch (choice) {
            case 1:
                graph.inputNodes();
                break;
            case 2:
                graph.inputEdges();
                break;
            case 3:
                graph.displayGraph();
                break;
            case 4: {
                cout << "Enter the name of the node to delete: ";
                string nodeName;
                cin >> nodeName;
                graph.DeleteNode(nodeName);
                break;
            }
            case 5:
                running = false;
                cout << "Exiting..." << endl;
                break;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    }

    return 0;
}
