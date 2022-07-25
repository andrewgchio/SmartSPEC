#ifndef UTILS_GRAPH_HPP
#define UTILS_GRAPH_HPP

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <utility>
#include <limits>

#include "../dataloader/SpacesLoader.hpp"

#include "Typedefs.hpp"
#include "IOUtils.hpp"

namespace {

// A map of the distance to go to a space 
typedef std::map<SpaceID, double> Dist;

// A map of the previous space to reach a space
typedef std::map<SpaceID, SpaceID> Prev;

} // end namespace

class SpacesGraph {
public: 
    
    // Constructors
    SpacesGraph();
    SpacesGraph(SpacesLoader& cl, const Filename& cache);

    // Queries
    const SpaceIDSet& getV() const;
    const std::map<SpaceID, SpaceIDSet>& getE() const;
    const SpaceIDList& shortestPath(SpaceID s, SpaceID t) const;

    // Modifiers
    void addNode(SpaceID s);
    void addEdge(SpaceID s, SpaceID t);
    void cacheAllShortestPaths();

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const SpacesGraph& g);

private:

    // Private helper methods
    void cacheShortestPath(SpaceID s);
    std::pair<Dist, Prev> dijkstra(SpaceID s) const;
    std::pair<Dist, Prev> bfs(SpaceID s) const;
    bool loadSpacesCache();
    void writeSpacesCache();
    
    // A map of shortest path between a src/dest
    std::map<SrcDest, SpaceIDList> paths;

    // The set of vertices (space ids)
    SpaceIDSet V;

    // The set of edges (edge list)
    std::map<SpaceID, SpaceIDSet> E;

    // Represents whether the paths are cached between src/dest
    bool cache;

    // Name of the paths-cache file
    Filename fcache;

    // SpacesLoader, to read coordinates
    SpacesLoader cl;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructors

// Default Constructor; initialize V with the outside space id
SpacesGraph::SpacesGraph() 
: cache{true} {
    V.insert(0); // outside SpaceID
}

// Construct the spaces graph from the given spaces, and cache it
SpacesGraph::SpacesGraph(SpacesLoader& cl, const Filename& cache) 
: cache{true}, fcache{cache}, cl{cl} {
    V.insert(0); // outside SpaceID
    for (const Space& c : cl) {
        addNode(c.id);
        for (SpaceID n : c.neighbors)
            addEdge(c.id, n);
    }
    cacheAllShortestPaths();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Return the set of vertices 
const SpaceIDSet& SpacesGraph::getV() const { return V; }

// Return the set of edges
const std::map<SpaceID, SpaceIDSet>& SpacesGraph::getE() const { return E; }

// Return the shortest path between a src/dest.
const SpaceIDList& SpacesGraph::shortestPath(SpaceID s, SpaceID t) const {
    // Shortest paths must be cached first
    if (!cache) {
        std::cerr << "SpacesGraph Error: cache paths first" << std::endl;
        std::exit(1);
    }
    
    // Try to find a path between the src/dest
    auto path = paths.find(std::make_pair(s,t));
    if (path == paths.end()) {
        std::cerr << "SpacesGraph Error: no path found between " 
                << s << " and " << t << std::endl;
        std::exit(1);
    }

    // Return cached path
    return path->second;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Modifiers

// Add a node to the space graph
void SpacesGraph::addNode(SpaceID s) {
    cache = false;
    V.insert(s);
}

// Add an edge to the space graph
void SpacesGraph::addEdge(SpaceID s, SpaceID t) {
    cache = false;
    V.insert(s);
    V.insert(t);
    E[s].insert(t);
}

// Cache all shortest paths into the file
void SpacesGraph::cacheAllShortestPaths() {
    if (!loadSpacesCache()) {
        for (SpaceID s : V) {
            cacheShortestPath(s);
        }
    }
    cache = true;
    writeSpacesCache();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Print the space graph and corresponding shortest paths
std::ostream& operator<<(std::ostream& oss, const SpacesGraph& g) {
    oss << "Graph(" << std::endl;
    for (SpaceID s : g.V)
        for (SpaceID t : g.V)
            oss << "  " << s << "->" << t << ": " 
                << g.shortestPath(s,t) << std::endl;
    oss << ")";
    return oss;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Private Helpers

// Use Dijkstra's algorithm to find the shortest paths between nodes
void SpacesGraph::cacheShortestPath(SpaceID s) {
    std::pair<Dist, Prev> DP = dijkstra(s);
    for (SpaceID t : V) {
        SrcDest sd(s,t);
        SpaceIDList path;
        while (s != t) {
            path.push_back(t);
            auto prev = DP.second.find(t);
            if (prev != DP.second.end())
                t = prev->second;
            else {
                path.clear();
                break;
            }
        }

        path.push_back(s);
        if (path.empty())
            path.push_back(t);
        else
            std::reverse(path.begin(), path.end());
        paths[sd] = path;
    }
}

// Dijkstra's algorithm
std::pair<Dist, Prev> SpacesGraph::dijkstra(SpaceID s) const {
    Dist D;
    Prev P;
    for (SpaceID v : V)
        D[v] = std::numeric_limits<int>::max();
    D[s] = 0;
    SpaceIDSet Q{V};
    while (!Q.empty()) {
        SpaceID u = *std::min_element(Q.begin(), Q.end(),
                    [&](SpaceID i, SpaceID j){ return D[i] < D[j]; });
        Q.erase(u);
        std::map<SpaceID, SpaceIDSet>::const_iterator it = E.find(u);
        if (it == E.end())
            continue;
        for (SpaceID v : it->second) {
            double edgedist = cl.dist(u,v);
            if (D[u] + edgedist < D[v]) {
                D[v] = D[u] + edgedist;
                P[v] = u;
            }
        }
    }
    return std::make_pair(D,P);
}

// Breadth first search
std::pair<Dist, Prev> SpacesGraph::bfs(SpaceID s) const {
    // Mark all vertices as not visited
    Prev P;
    Dist D;
    for (SpaceID v : V)
        D[v] = -1;

    // Create a queue for BFS; mark current node and pushback to queue
    D[s] = 0;
    SpaceIDList queue{s};

    while (!queue.empty()) {
        // Dequeue vertex from queue and print it
        SpaceID u = queue.front();
        queue.erase(queue.begin());

        // Get all adj vertices of curr; mark unvisited vertices and enqueue
        std::map<SpaceID, SpaceIDSet>::const_iterator it = E.find(u);
        if (it == E.end())
            continue;
        for (SpaceID v : it->second) {
            if (D[v] == -1) {
                D[v] = D[u] + 1;
                P[v] = u;
                queue.push_back(v);
            }
        }
    }
    return std::make_pair(D,P);
}

// Read the fcache file and load in the cached paths
bool SpacesGraph::loadSpacesCache() {
    if (!checkFileExists(fcache))
        return false;
    std::cout << "reading cache file" << std::endl;
    std::string src,dest,path;
    for (std::ifstream file{fcache}; std::getline(file, src, ',');) {
        std::getline(file, dest, ',');
        SrcDest sd{std::stoi(src), std::stoi(dest)};

        std::getline(file, path);
        char cline[path.length()+1];
        std::strcpy(cline, path.c_str());
        char* tok = std::strtok(cline, ";");
        SpaceIDList sl;
        while (tok) {
            sl.push_back(std::atoi(tok));
            tok = std::strtok(NULL, ";");
        }
        paths[sd] = sl;
    }
    return true;
}

// Save the computed paths into fcache
void SpacesGraph::writeSpacesCache() {
    if (fcache == "")
        return;
    std::ofstream file(fcache);
    for (const std::pair<SrcDest, SpaceIDList>& e : paths) {
        file << e.first.first << "," << e.first.second << ",";

        if (!e.second.empty()) {
            SpaceIDList::const_iterator it = e.second.begin();
            file << *it++;
            for (; it != e.second.end(); ++it)
                file << ";" << *it;
        }
        file << "\n";
    }
}

#endif // UTILS_GRAPH_HPP
