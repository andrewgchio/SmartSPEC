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

    explicit ConfigLoader(const Filename& fname);

    friend std::ostream& operator<<(std::ostream& oss, const ConfigLoader& c);

    bool hasSectionOption(const Section& s, const Option& o) const;

    Value& operator()(const Section& s, const Option& o);
    const Value& operator()(const Section& s, const Option& o) const;

    // With a default value
    Value& operator()(const Section& s, const Option& o, Value& v);
    const Value& operator()(
            const Section& s, const Option& o, const Value& v) const;

private:

    void sectionError(const Section& s) const;
    void optionError(const Section& s, const Option& o) const;

    ConfigMap config;

};

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

bool ConfigLoader::hasSectionOption(const Section& s, const Option& o) const {
    ConfigMap::const_iterator cit = config.find(s);
    if (cit == config.end())
        return false;

    SectionMap::const_iterator sit = cit->second.find(o);
    if (sit == cit->second.end())
        return false;

    return true;
}

Value& ConfigLoader::operator()(const Section& s, const Option& o) {
    ConfigMap::iterator cit = config.find(s);
    if (cit == config.end())
        sectionError(s);

    SectionMap::iterator sit = cit->second.find(o);
    if (sit == cit->second.end())
        optionError(s, o);

    return sit->second;
}

const Value& ConfigLoader::operator()(const Section& s, const Option& o) const {
    ConfigMap::const_iterator cit = config.find(s);
    if (cit == config.end())
        sectionError(s);

    SectionMap::const_iterator sit = cit->second.find(o);
    if (sit == cit->second.end())
        optionError(s, o);

    return sit->second;
}

Value& ConfigLoader::operator()(const Section& s, const Option& o, Value& v) {
    ConfigMap::iterator cit = config.find(s);
    if (cit == config.end())
        return v;

    SectionMap::iterator sit = cit->second.find(o);
    if (sit == cit->second.end())
        return v;

    return sit->second;
}

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
// Private Helper Methods

void ConfigLoader::sectionError(const Section& s) const {
    std::cerr << "ConfigError: section `" << s << "` not found" << std::endl;
    std::exit(1);
}

void ConfigLoader::optionError(const Section& s, const Option& o) const {
    std::cerr << "ConfigError: section.option `" << s << "." << o 
              << "` not found" << std::endl;
    std::exit(1);
}

#endif // DATALOADER_CONFIG_LOADER_HPP
