#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <fstream>
using namespace std;

// Structure for Nodes (Intersections)
struct Nodes {
    string name;  // Name of the intersection
    int timing;

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
    void createNode(const string& name, int timings) {
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
            // Check for duplicate edges
            for (int i = 0; i < numEdges; i++) {
                if (streets[i].from == fromNode && streets[i].to == toNode) {
                    cout << "Edge from '" << from << "' to '" << to << "' already exists. Updating weight to " << weight << "." << endl;
                    streets[i].weight = weight; // Update the weight of the existing edge
                    return;
                }
            }

            // If no duplicate, add the new edge
            if (numEdges == edgeCapacity) {
                resizeEdges();
            }
            streets[numEdges++] = Edges(fromNode, toNode, weight);
            cout << "Edge from '" << from << "' to '" << to << "' with weight " << weight << " has been added." << endl;
        } else {
            cout << "Error: One or both nodes not found!" << endl;
        }
    }

    // Input nodes from user
    void inputNodes() {
        cout << "Enter node names (type -1 to stop):" << endl;

        // Dynamically allocate nodes array
        string name;
        int timings;
        while (true) {
            cout << "Enter node name: ";
            cin >> name;
            if (name == "-1") break;
            cout<<"Enter GreennTime: ";
            cin>>timings;
            

            createNode(name,timings);  // Call the createNode function to add a node
            ofstream file("traffic_signal_timings.csv", std::ios::app);
            file << name <<',' << timings<< endl; 
            file.close();
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
            ofstream file("road_network.csv", std::ios::app);
            file << from << "," << to << "," << weight << endl; 
            file.close();
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

    void DeleteNode(const string& name) {
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
                i++;
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
            for (int i = nodeIndex; i < numNodes - 1; i++) {
                intersections[i] = intersections[i + 1];
            }
            numNodes--;
        }

        // Update files
        updateNodeFile(name);
        updateEdgeFile(name);

        cout << "Node " << name << " and its associated edges have been deleted." << endl;
}

    void traffic_signal_timings(){
        ifstream file1("traffic_signal_timings.csv");
        string node;
        int timings;
        if (file1.good()) {
            string header;
            getline(file1,header);
        }
        
        while(file1.good()){
            getline(file1, node, ',');  // Get the first node (before comma)
            file1>>timings;
            file1.ignore(__LONG_MAX__,'\n');
            if(node != ""){
                createNode(node,timings);
            }
        }


    }

    // Load the road network from a CSV file
    void road_Network() {
        ifstream file("road_network.csv");
        string from_node, to_node;
        int weight;

        // Skip the header line (if there's one)
        if (file.good()) {
            string header;
            getline(file, header);

        }

        while (file.good()) {
            // Read node1, node2, and weight in a loop
            getline(file, from_node, ',');  // Get the first node (before comma)
            getline(file, to_node, ',');  // Get the second node (after comma)
            file >> weight;             // Get the weight (after the comma)
            
            // Skip the newline character after weight
            file.ignore(__LONG_MAX__,'\n');

            if (!from_node.empty() && !to_node.empty()) {
                createEdges(from_node, to_node, weight); 

                // Create the edge between the nodes
            }
        }

        file.close();  // Ensure to close the file after processing
}


void DeleteEdge(const string& fromNode, const string& toNode, int weight) {
    // Find and remove the edge from the graph
    int i = 0;
    while (i < numEdges) {
        if (streets[i].from->name == fromNode && streets[i].to->name == toNode && streets[i].weight == weight) {
            // Shift elements to remove the edge
            for (int j = i; j < numEdges - 1; j++) {
                streets[j] = streets[j + 1];
            }
            numEdges--;
            cout << "Edge " << fromNode << " -> " << toNode << " (Weight: " << weight << ") has been deleted from the graph." << endl;
            break;
        } else {
            i++;
        }
    }

    // Update the edge file
    updateEdgeFile(fromNode);
}

    void updateEdgeFile(const string& nodeName) {
        ifstream infile("road_network.csv");
        ofstream outfile("temp.csv");

        string from, to;
        int weight;

        bool headerRead = false;
        
        while (infile.good()) {
             if (!headerRead) {
                string header;
                getline(infile, header);  // Read the header and write it to the temp file.
                outfile << header << endl;
                headerRead = true;
            }
            // Read the edge data (from node, to node, weight)
            getline(infile, from, ',');
            getline(infile, to, ',');
            infile >> weight;
            infile.ignore(__LONG_MAX__, '\n');  // Skip the rest of the line.

            // Skip edges that involve the deleted node (either as 'from' or 'to')
            if (from == nodeName ) {
                getline(infile, to, ',');
                infile >> weight;
                continue;  // Skip the current edge and move to the next one
            }else if(to == nodeName) {
                infile >> weight;
            }
            else {
                outfile << from << "," << to << "," << weight << endl;  // Write the edge to the temp file
            }
        }

        infile.close();
        outfile.close();

        // Replace the original file with the updated file
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
                getline(infile, header);  // Read the header and write it to the temp file.
                outfile << header << endl;
                headerRead = true;
            }
            
            getline(infile, node, ',');
            if (node.empty()) continue;  // Skip any empty lines.
            
            infile >> timings;
            infile.ignore(__LONG_MAX__, '\n');  // Skip the rest of the line.

            // Only write the node if it is not the one being deleted
            if (node != nodeName) {
                outfile << node << "," << timings << endl;
            }
        }

        infile.close();
        outfile.close();

        // Replace the original file with the updated file
        remove("traffic_signal_timings.csv");
        rename("temp.csv", "traffic_signal_timings.csv");
    }




};

// Main function with interactive menu
int main() {
    Graph graph;
    bool running = true;
    cout<<"Reading File Data..."<<endl;
    graph.traffic_signal_timings();
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
