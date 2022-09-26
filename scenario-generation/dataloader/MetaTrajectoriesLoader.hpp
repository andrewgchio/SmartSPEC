#ifndef DATALOADER_METATRAJECTORIESLOADER_HPP
#define DATALOADER_METATRAJECTORIESLOADER_HPP

#include <iostream>
#include <cstdlib>
#include <random>
#include <algorithm>
#include <utility>

#include "../include/rapidjson/document.h"

#include "../model/Space.hpp"
#include "../model/Trajectory.hpp"
#include "../model/MetaTrajectory.hpp"

#include "../dataloader/SpacesLoader.hpp"

#include "../utils/Typedefs.hpp"
#include "../utils/DateUtils.hpp"
#include "../utils/IOUtils.hpp"
#include "../utils/Graph.hpp"
#include "../utils/RandomGenerator.hpp"
#include "../utils/NormalDistributions.hpp"

namespace {
    
    const static Time secs15{15};
    
    Time randTimeSecs15() {
        static NormalTime n{"00:00:15", "00:00:05"};
        return n.sample();
    }
    
}

class MetaTrajectoriesLoader {
public:

    MetaTrajectoriesLoader();
    explicit MetaTrajectoriesLoader(SpacesLoader& cl, const Filename& cache);
    MetaTrajectoriesLoader(
            const Filename& fname, 
            const Filename& cache,
            SpacesLoader& cl);

    const Trajectory& getPath(
            SpaceID s, 
            SpaceID t, 
            bool useCache=false, 
            bool useShortest=false);

    friend std::ostream& operator<<(
            std::ostream& oss, 
            const MetaTrajectoriesLoader& mt);

private:

    TimeList estTime(const SpaceIDList& sl) const;
    int manhattan(const Coordinates& c1, const Coordinates& c2) const;

    SrcDestIndexMap loc;
    std::vector<MetaTrajectory> entries;

    std::map<SrcDest, std::pair<Index, Index>> cache;

    SpacesGraph g;

    SpacesLoader cl;

};

MetaTrajectoriesLoader::MetaTrajectoriesLoader() {}

MetaTrajectoriesLoader::MetaTrajectoriesLoader(
        SpacesLoader& cl, 
        const Filename& cache) 
: g{cl,cache}, cl{cl} {}

MetaTrajectoriesLoader::MetaTrajectoriesLoader(
        const Filename& fname, const Filename& cache, SpacesLoader& cl) 
: g{cl,cache}, cl{cl} {
    if (fname == "none")
        return;

    std::cout << "... Reading MetaTrajectories file" << std::endl;

    rapidjson::Document doc;
    openJSON(fname, doc);
    entries.reserve(doc.Size());

    for (const rapidjson::Value& v : doc.GetArray()) {
        MetaTrajectory e;
        e.sd = SrcDest{v["StartSpaceID"].GetInt(), v["EndSpaceID"].GetInt()};

        Trajectory te;
        for (const rapidjson::Value& x : v["SpaceID"].GetArray())
            te.traj.push_back(x.GetInt());
        for (const rapidjson::Value& x : v["Delta"].GetArray())
            te.delta.push_back(Time(x.GetString()));

        auto it = loc.find(e.sd);
        if (it == loc.end()) {
            loc[e.sd] = entries.size();
            e.trajs.push_back(te);
            entries.push_back(e);
        }
        else {
            entries[it->second].trajs.push_back(te);
        }
    }
}

const Trajectory& MetaTrajectoriesLoader::getPath(
        SpaceID s, 
        SpaceID t, 
        bool useCache, 
        bool useShortest) {
    SrcDest sd{s,t};
    auto eit = loc.find(sd);
    // if no path exists or useShortest, then use shortest path
    if (useShortest || eit == loc.end()) { 
        MetaTrajectory e;
        e.sd = sd;
        loc[e.sd] = entries.size();
        const SpaceIDList& sl = g.shortestPath(s,t);
        TimeList tl = estTime(sl);
        e.trajs.push_back(Trajectory{sl,tl});
        entries.push_back(e);
        return entries[entries.size()-1].trajs[0];
    }

    if (useCache) {
        auto it = cache.find(sd);
        if (it != cache.end()) { 
            const MetaTrajectory& e = entries[it->second.first];
            return e.trajs[it->second.second];
        }
    }

    Index entryIdx = loc[sd];
    const MetaTrajectory& e = entries[entryIdx];
    Index trajIdx = randInt(e.trajs.size()-1);

    cache[sd] = std::make_pair(entryIdx, trajIdx);
    return e.trajs[trajIdx];
}

TimeList MetaTrajectoriesLoader::estTime(const SpaceIDList& sl) const {
    /*
    TimeList tl(sl.empty() ? 0 : sl.size()-1);
    std::generate(tl.begin(), tl.end(), randTimeSecs15);
    std::cout << sl << std::endl;
    std::cout << tl << std::endl;
    return tl;
    */

    TimeList tl;
    for (int i = 0; i < sl.size()-1; ++i) {
        int d = manhattan(cl[sl[i]].coords, cl[sl[i+1]].coords);
        tl.push_back(NormalTime{Time{d*5}, Time{d*1}}.sample());
    }
    return tl;
}

int MetaTrajectoriesLoader::manhattan(
        const Coordinates& c1, 
        const Coordinates& c2) const {
    return abs(c1[0]-c2[0]) + abs(c1[1]-c2[1]) + abs(c1[2]-c2[2]);
}

std::ostream& operator<<(std::ostream& oss, const MetaTrajectoriesLoader& mt) {
    oss << "MetaTrajectories:" << std::endl;
    for (const MetaTrajectory& e : mt.entries) {
        oss << "MetaTrajectory("
            << "src/dest=" << e.sd << ", ";
        auto it = e.trajs.begin();
        oss << "(spaces=" << it->traj << ", "
            << "delta=" << it->delta << ")";
        for (; it != e.trajs.end(); ++it) {
            oss << ", (spaces=" << it->traj << ", "
                << "delta=" << it->delta << ")";
        }
        oss << ")" << std::endl;
    }
    return oss;
}

#endif // DATALOADER_METATRAJECTORIESLOADER_HPP
