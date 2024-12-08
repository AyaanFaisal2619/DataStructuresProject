# Detailed Report: Traffic Simulation Program

## 1. Introduction

This report presents a detailed analysis of a traffic simulation program implemented in C++. The program models a traffic network using graph theory, where intersections are represented as nodes and streets as edges. It simulates vehicle movement, traffic signal operations, and handles dynamic events such as road closures and congestion management.

## 2. System Architecture

### 2.1 Classes and Their Responsibilities

- **Nodes**: Represents intersections with attributes like name and green light duration.
- **Edges**: Represents streets with attributes like start node, end node, weight (distance/time), capacity, and active status.
- **Graph**: Manages the collection of nodes and edges, providing methods for adding, deleting, and querying the graph structure. It also implements algorithms like BFS and Dijkstra's for pathfinding.
- **Vehicles**: Inherits from `Graph` and manages vehicle data, including their routes and priorities. It extends simulation capabilities with traffic flow management and congestion handling.

### 2.2 Key Data Structures

- **Nodes Structure**:
  - `name`: Name of the intersection.
  - `timing`: Green light duration at the intersection.
- **Edges Structure**:
  - `from`: Pointer to the starting node.
  - `to`: Pointer to the ending node.
  - `weight`: Distance or time associated with the edge.
  - `capacity`: Maximum number of vehicles the edge can handle.
  - `isActive`: Boolean indicating if the edge is currently passable.
- **VehiclesNode Structure**:
  - `ID`: Unique identifier for the vehicle.
  - `edge`: Pointer to the edge representing the vehicle's route.
  - `priorityLevel`: Priority level of the vehicle (e.g., "High" for emergency vehicles).

## 3. Graph Operations

### 3.1 Node and Edge Management

- **Adding Nodes and Edges**: The program allows users to add nodes (intersections) and edges (streets) through interactive input or by reading from CSV files.
- **Deleting Nodes and Edges**: Users can delete nodes and edges, with the program updating associated data structures and files accordingly.

### 3.2 Pathfinding Algorithms

- **BFS (Breadth-First Search)**: Used to find the shortest path in unweighted graphs.
- **Dijkstra's Algorithm**: Computes the shortest path in weighted graphs, considering edge weights as distances or travel times.

### 3.3 Congestion Management

- **Vehicle Counting**: Tracks the number of vehicles on each edge to determine congestion status.
- **Road Closure**: Allows simulation of road closures by deactivating edges.

## 4. Vehicles Management

### 4.1 Vehicle Addition and Prioritization

- **Adding Vehicles**: Vehicles can be added manually or read from CSV files, with attributes like `ID`, start node, end node, and priority level.
- **Priority Levels**: Vehicles with higher priority (e.g., emergency vehicles) are given preferential treatment in simulation.

### 4.2 Route Calculation

- **Dijkstra's Algorithm for Vehicles**: Computes the shortest path for each vehicle from its start to end node.
- **Alternative Pathfinding**: If the primary path is congested, the program attempts to find an alternative path using BFS.

### 4.3 Traffic Simulation

- **Simulation Loop**: Models the movement of vehicles over time, considering traffic light phases and potential waiting times.
- **Real-Time Updates**: Simulates real-time traffic conditions, including vehicle movements, waiting at traffic lights, and handling congestion.

## 5. File Handling

### Reading from CSV Files

- **traffic_signal_timings.csv**: Contains node names and their green light durations.
- **road_network.csv**: Contains edges with their start node, end node, and weights.
- **vehicles.csv and emergency_vehicles.csv**: Contains vehicle data, including IDs, start and end nodes, and priority levels.

### Writing to CSV Files

- The program appends new nodes and edges to the respective CSV files during runtime.

## 6. User Interface

- **Menu-Driven Interface**: Provides options for various operations like adding nodes/edges, displaying the graph, deleting nodes/edges, calculating routes, adding vehicles, and initiating simulations.
- **Interactive Simulation**: Users can trigger simulations and observe vehicle movements and traffic conditions in real-time.

## 7. Simulation Details

### 7.1 Traffic Light Simulation

- **Traffic Light Phases**: Simulates traffic light operations based on node timings, affecting vehicle movement.
- **Waiting Time Calculation**: Determines the waiting time for vehicles encountering red lights.

### 7.2 Congestion Handling

- **Congestion Detection**: Monitors vehicle counts on edges to detect congestion.
- **Alternative Routing**: Attempts to find alternative paths using BFS when congestion is detected on the primary path.

### 7.3 Emergency Vehicle Handling

- **Priority Handling**: Allows emergency vehicles to bypass waiting times at traffic lights and find the shortest path quickly.

## 8. Future Enhancements

- **Real-Time Data Integration**: Incorporate real-time traffic data for more accurate simulations.
- **Advanced Traffic Rules**: Implement additional traffic rules like lane restrictions, speed limits, and turn restrictions.
- **GUI Development**: Develop a graphical user interface for visualizing the traffic network and simulation results.
- **Scalability Improvements**: Optimize algorithms and data structures for handling larger traffic networks.

## 9. Conclusion

This traffic simulation program provides a comprehensive framework for modeling and simulating traffic flow in a network of intersections and streets. It effectively handles various scenarios, including congestion management, road closures, and priority-based vehicle movement. With potential enhancements, it can serve as a valuable tool for urban planning and traffic management studies.
