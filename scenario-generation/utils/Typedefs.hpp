#ifndef DATALOADERS_TYPEDEFS_HPP
#define DATALOADERS_TYPEDEFS_HPP

#include <string>
#include <vector>
#include <map>
#include <set>
#include <utility>

#include "NormalDistributions.hpp"

// Config
using Section              = std::string;
using Option               = std::string;
using Value                = std::string;
using SectionEntry         = std::pair<Option, Value>;
using SectionMap           = std::map<Option, Value>;
using ConfigEntry          = std::pair<Section, SectionMap>;
using ConfigMap            = std::map<Section, SectionMap>;

//General
using Filename             = std::string;
using Index                = int;
using Description          = std::string;
using Probability          = double; 
using ProbabilityList      = std::vector<Probability>;
using Coordinates          = std::vector<int>;

// SmartSPEC Concepts
using PersonID             = int;
using EventID              = int;
using SpaceID              = int;
using SensorID             = int;
using MetaPersonID         = int;
using MetaEventID          = int;
using MetaSensorID         = int;

using PersonIDIndexMap     = std::map<PersonID, Index>;
using EventIDIndexMap      = std::map<EventID, Index>;
using SpaceIDIndexMap      = std::map<SpaceID, Index>;
using SensorIDIndexMap     = std::map<SensorID, Index>;
using MetaPersonIDIndexMap = std::map<MetaPersonID, Index>;
using MetaEventIDIndexMap  = std::map<MetaEventID, Index>;

using PersonIDList         = std::vector<PersonID>;
using EventIDList          = std::vector<EventID>;
using SpaceIDList          = std::vector<SpaceID>;
using SensorIDList         = std::vector<SensorID>;
using MetaEventIDList      = std::vector<MetaEventID>;
using MetaPersonIDList     = std::vector<MetaPersonID>;

using Coverage             = SpaceIDList; 
using MinCap               = int;
using MaxCap               = int;
using MinCapDistr          = Normal<MinCap>;
using MaxCapDistr          = Normal<MaxCap>;
using CapRange             = std::pair<MinCap, MaxCap>;
using PersonCapRange       = std::map<MetaPersonID, CapRange>; 
using EventAffinity        = std::map<MetaEventID, Probability>;
using CapRangeDistr        = std::pair<MinCapDistr, MaxCapDistr>;
using PersonCapRangeDistr  = std::map<PersonID, CapRangeDistr>;

// SpacesGraph
using SrcDest              = std::pair<SpaceID, SpaceID>;
using SpaceIDSet           = std::set<SpaceID>;
using SrcDestIndexMap      = std::map<SrcDest, Index>;

// Sensors Observations
using PersonIDSet          = std::set<PersonID>;

#endif // DATALOADERS_TYPEDEFS_HPP
