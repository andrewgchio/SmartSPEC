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

typedef std::map<SpaceID, int> Dist;
typedef std::map<SpaceID, SpaceID> Prev;

} // end namespace

class SpacesGraph {
public: 
    
    SpacesGraph();
    SpacesGraph(SpacesLoader& cl, const Filename& cache);

    void addNode(SpaceID s);
    void addEdge(SpaceID s, SpaceID t);

    const SpaceIDList& shortestPath(SpaceID s, SpaceID t) const;

    void cacheAllShortestPaths(const Filename& cache);
    const SpaceIDSet& getV() const;
    const std::map<SpaceID, SpaceIDSet>& getE() const;

    friend std::ostream& operator<<(std::ostream& oss, const SpacesGraph& g);

private:

    void cacheShortestPath(SpaceID s);
    std::pair<Dist, Prev> dijkstra(SpaceID s) const;
    std::pair<Dist, Prev> bfs(SpaceID s) const;
    bool loadSpacesCache(const std::string& cachefile);
    void writeSpacesCache(const std::string& cachefile);
    
    std::map<SrcDest, SpaceIDList> paths;
    SpaceIDSet V;
    std::map<SpaceID, SpaceIDSet> E;
    bool cache;

};

SpacesGraph::SpacesGraph() 
: cache{true} {
    V.insert(0); // outside SpaceID
}

SpacesGraph::SpacesGraph(SpacesLoader& cl, const Filename& cache) 
: cache{true} {
    V.insert(0); // outside SpaceID
    for (const Space& c : cl) {
        addNode(c.id);
        for (SpaceID n : c.neighbors)
            addEdge(c.id, n);
    }
    cacheAllShortestPaths(cache);
}

void SpacesGraph::addNode(SpaceID s) {
    cache = false;
    V.insert(s);
}

void SpacesGraph::addEdge(SpaceID s, SpaceID t) {
    cache = false;
    V.insert(s);
    V.insert(t);
    E[s].insert(t);
}

const SpaceIDList& SpacesGraph::shortestPath(SpaceID s, SpaceID t) const {
    if (!cache) {
        std::cerr << "SpacesGraph Error: call cacheAllShortestPaths() first" 
                << std::endl;
        std::exit(1);
    }
    
    auto path = paths.find(std::make_pair(s,t));
    if (path == paths.end()) {
        std::cerr << "SpacesGraph Error: no path found between " 
                << s << " and " << t << std::endl;
        std::exit(1);
    }

    return path->second;
}

void SpacesGraph::cacheAllShortestPaths(const std::string& cachefile) {
    if (!loadSpacesCache(cachefile)) {
        for (SpaceID s : V) {
            cacheShortestPath(s);
        }
    }
    cache = true;
    writeSpacesCache(cachefile);
}

const SpaceIDSet& SpacesGraph::getV() const { return V; }
const std::map<SpaceID, SpaceIDSet>& SpacesGraph::getE() const { return E; }

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

void SpacesGraph::cacheShortestPath(SpaceID s) {
    std::pair<Dist, Prev> DP = bfs(s);
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
            if (D[u] + 1 < D[v]) {
                D[v] = D[u] + 1;
                P[v] = u;
            }
        }
    }
    return std::make_pair(D,P);
}

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

bool SpacesGraph::loadSpacesCache(const std::string& cachefile) {
    if (!checkFileExists(cachefile))
        return false;
    std::cout << "reading cache file" << std::endl;
    std::string src,dest,path;
    for (std::ifstream file{cachefile}; std::getline(file, src, ',');) {
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

void SpacesGraph::writeSpacesCache(const std::string& cachefile) {
    if (cachefile == "")
        return;
    std::ofstream file(cachefile);
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
