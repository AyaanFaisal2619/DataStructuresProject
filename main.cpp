#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

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
    int numEdges;          // Number of edges

public:
    // Constructor
    Graph() : intersections(nullptr), streets(nullptr), numNodes(0), numEdges(0) {}

    // Destructor
    ~Graph() {
        delete[] intersections;
        delete[] streets;
    }

    // Input nodes from user
    void inputNodes() {
        cout << "Enter node names (type -1 to stop):" << endl;

        // Dynamically allocate nodes array
        int capacity = 10;
        intersections = new Nodes[capacity];
        string name;

        while (true) {
            cin >> name;
            if (name == "-1") break;

            if (numNodes == capacity) {
                capacity *= 2;
                Nodes* temp = new Nodes[capacity];
                for (int i = 0; i < numNodes; i++) {
                    temp[i] = intersections[i];
                }
                delete[] intersections;
                intersections = temp;
            }

            intersections[numNodes++] = Nodes(name);
        }
    }

    // Input edges from user
    void inputEdges() {
        cout << "Enter edges in the format: from to weight (type -1 to stop):" << endl;

        // Dynamically allocate edges array
        int capacity = 10;
        streets = new Edges[capacity];
        string from, to;
        int weight;

        while (true) {
            cin >> from;
            if (from == "-1") break;
            cin >> to >> weight;

            if (numEdges == capacity) {
                capacity *= 2;
                Edges* temp = new Edges[capacity];
                for (int i = 0; i < numEdges; i++) {
                    temp[i] = streets[i];
                }
                delete[] streets;
                streets = temp;
            }

            Nodes* fromNode = findNode(from);
            Nodes* toNode = findNode(to);

            if (fromNode && toNode) {
                streets[numEdges++] = Edges(fromNode, toNode, weight);
            } else {
                cout << "Error: One or both nodes not found!" << endl;
            }
        }
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
};

// Main function
int main() {
    Graph graph;

    // Input nodes and edges from the user
    graph.inputNodes();
    graph.inputEdges();

    // Display the graph
    graph.displayGraph();

    return 0;
}
