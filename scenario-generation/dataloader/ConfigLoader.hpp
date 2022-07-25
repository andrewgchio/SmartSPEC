#ifndef DATALOADER_CONFIG_LOADER_HPP
#define DATALOADER_CONFIG_LOADER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <utility>

#include "../utils/Typedefs.hpp"

namespace {

// Pattern to recognize a Section
const std::regex SEC_PAT{"\\s*\\[\\s*(.+?)\\s*\\]\\s*"};

// Pattern to recognize a Option (= Value) inside a Section
const std::regex OPT_PAT{"\\s*(.+?)\\s*=\\s*(.+?)\\s*"};

} // end namespace

class ConfigLoader {
public:

    // Constructor
    explicit ConfigLoader(const Filename& fname);

    // Queries
    bool hasSectionOption(const Section& s, const Option& o) const;

    Value& operator()(const Section& s, const Option& o);
    const Value& operator()(const Section& s, const Option& o) const;

    Value& operator()(const Section& s, const Option& o, Value& v);
    const Value& operator()(
            const Section& s, const Option& o, const Value& v) const;

    // I/O
    friend std::ostream& operator<<(std::ostream& oss, const ConfigLoader& c);

private:

    // Private helpers to showing error messages
    void sectionError(const Section& s) const;
    void optionError(const Section& s, const Option& o) const;

    // Map to hold configuration values
    ConfigMap config;

};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Constructor

// For a given configuration file, read each line and interpret it either as a
// section header or option value in one pass. Lines matching the format for a 
// section header will create a new section, while lines matching the format
// for an option value will be saved under the current section. 
ConfigLoader::ConfigLoader(const Filename& fname) {
    std::cout << "... Reading Config file: " << fname << std::endl;
    Section sname{"others"};
    std::string line; 
    for (std::ifstream file{fname}; std::getline(file, line);) {
        std::smatch results;
        if (std::regex_match(line, results, SEC_PAT))
            sname = results[1];
        else if (std::regex_match(line, results, OPT_PAT))
            config[sname][results[1]] = results[2];
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Queries

// Returns true if the specified section and option exists, and false otherwise
bool ConfigLoader::hasSectionOption(const Section& s, const Option& o) const {
    ConfigMap::const_iterator cit = config.find(s);
    if (cit == config.end())
        return false;

    SectionMap::const_iterator sit = cit->second.find(o);
    if (sit == cit->second.end())
        return false;

    return true;
}

// Returns the value associated with the specified section and option, or
// prints an error message and exits.
Value& ConfigLoader::operator()(const Section& s, const Option& o) {
    ConfigMap::iterator cit = config.find(s);
    if (cit == config.end())
        sectionError(s);

    SectionMap::iterator sit = cit->second.find(o);
    if (sit == cit->second.end())
        optionError(s, o);

    return sit->second;
}

// Returns the value associated with the specified section and option, or
// prints an error message and exits. (const)
const Value& ConfigLoader::operator()(const Section& s, const Option& o) const {
    ConfigMap::const_iterator cit = config.find(s);
    if (cit == config.end())
        sectionError(s);

    SectionMap::const_iterator sit = cit->second.find(o);
    if (sit == cit->second.end())
        optionError(s, o);

    return sit->second;
}

// Returns the value associated with the specified section and option, or 
// uses the default value provided. 
Value& ConfigLoader::operator()(const Section& s, const Option& o, Value& v) {
    ConfigMap::iterator cit = config.find(s);
    if (cit == config.end())
        return v;

    SectionMap::iterator sit = cit->second.find(o);
    if (sit == cit->second.end())
        return v;

    return sit->second;
}

// Returns the value associated with the specified section and option, or 
// uses the default value provided. (const)
const Value& ConfigLoader::operator()(const Section& s, const Option& o, 
        const Value& v) const {
    ConfigMap::const_iterator cit = config.find(s);
    if (cit == config.end())
        return v;

    SectionMap::const_iterator sit = cit->second.find(o);
    if (sit == cit->second.end())
        return v;

    return sit->second;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// I/O

// Prints the sections and corresponding options/values saved in the
// configuration file. 
std::ostream& operator<<(std::ostream& oss, const ConfigLoader& c) {
    for (const ConfigEntry& ce : c.config) {
        oss << ce.first << std::endl;
        for (const SectionEntry& se : ce.second)
            oss << "  " << se.first << " = " << se.second << std::endl;
    }
    return oss;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Private helper methods to show error messages

// Should be called when a given section does not exist
void ConfigLoader::sectionError(const Section& s) const {
    std::cerr << "ConfigError: section `" << s << "` not found" << std::endl;
    std::exit(1);
}

// Should be called when a given option in a specified section does not exist
void ConfigLoader::optionError(const Section& s, const Option& o) const {
    std::cerr << "ConfigError: section.option `" << s << "." << o 
              << "` not found" << std::endl;
    std::exit(1);
}

#endif // DATALOADER_CONFIG_LOADER_HPP
