#ifndef UTILS_IOUTILS_HPP
#define UTILS_IOUTILS_HPP

#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <unistd.h>
#include <sys/stat.h>

#include "../include/rapidjson/document.h"
#include "../include/rapidjson/filereadstream.h"
#include "../include/rapidjson/filewritestream.h"
#include "../include/rapidjson/writer.h"

#include "Typedefs.hpp"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// File IO

// Return true if the filename exists
bool checkFileExists(const Filename& fname) {
    struct stat buf;
    return stat(fname.c_str(), &buf) == 0;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// JSON IO Utils
namespace {

// Buffer size
const int BUF = 65536;

} // end namespace

// For convenience 
namespace rj = rapidjson;

// Opens a JSON file and parses it into the document stream
void openJSON(const Filename& fname, rj::Document& d) {
    FILE* fp = fopen(fname.c_str(), "rb");
    if (fp == NULL) {
        std::cerr << "Invalid file " << fname << std::endl;
        std::exit(1);
    }

    char buf[BUF];
    rj::FileReadStream frs(fp, buf, sizeof(buf));
    d.ParseStream(frs);
    fclose(fp);
}

// Dumps the JSON file in the document stream into the file 
void dumpJSON(const Filename& fname, rj::Document& d) {
    FILE* fp = fopen(fname.c_str(), "wb");
    if (fp == NULL) {
        std::cerr << "Invalid file " << fname << std::endl;
        std::exit(1);
    }

    char buf[BUF];
    rj::FileWriteStream fws(fp, buf, sizeof(buf));
    rj::Writer<rj::FileWriteStream> writer(fws);
    d.Accept(writer);
    fclose(fp);
}

// Parses an int from the rapidjson value
int parseInt(const rj::Value& v, const char* m) {
    if (v.HasMember(m))
        return v[m].GetInt();

    std::cerr << "JSON object cannot parse int for " << m << std::endl;
    std::exit(1);
}

// Parses an int from the rapidjson value, or returns the default option
int parseInt(const rj::Value& v, const char* m, int o) 
{ return v.HasMember(m) ? v[m].GetInt() : o; }

// Parses a double from the rapidjson value
double parseDouble(const rj::Value& v, const char* m) {
    if (v.HasMember(m))
        return v[m].GetDouble();

    std::cerr << "JSON object cannot parse double for " << m << std::endl;
    std::exit(1);
}

// Parses a double from the rapidjson value, or returns the default option
double parseDouble(const rj::Value& v, const char* m, double o) 
{ return v.HasMember(m) ? v[m].GetDouble() : o; }

// Parses a string from the rapidjson value
std::string parseStr(const rj::Value& v, const char* m) {
    if (v.HasMember(m))
        return v[m].GetString();

    std::cerr << "JSON object cannot parse string for " << m << std::endl;
    std::exit(1);
}

// Parses a string from the rapidjson value, or returns the default option
std::string parseStr(const rj::Value& v, const char* m, const std::string& o) 
{ return v.HasMember(m) ? v[m].GetString() : o; }

// Parses an int array from the rapidjson value
std::vector<int> parseIntArr(const rj::Value& v, const char* m) {
    if (v.HasMember(m)) {
        std::vector<int> arr;
        arr.reserve(v[m].Size());

        for (const rj::Value& x : v[m].GetArray())
            arr.push_back(x.GetInt());

        return arr;
    }

    std::cerr << "JSON object cannot parse int array for " << m << std::endl;
    std::exit(1);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// operator<< for various types

// Pretty print a vector
template<class T>
std::ostream& operator<<(std::ostream& oss, const std::vector<T>& vec) {
    oss << "[";
    if (!vec.empty()) {
        typename std::vector<T>::const_iterator it = vec.begin();
        oss << *it++;
        for (; it != vec.end(); ++it)
            oss << ", " << *it;
    }
    oss << "]";
    return oss;
}

// Pretty print a pair
template<class T1, class T2>
std::ostream& operator<<(std::ostream& oss, const std::pair<T1, T2>& p) {
    oss << "(" << p.first << ", " << p.second << ")";
    return oss;
}

// Pretty print a tuple
template<class T1, class T2, class T3>
std::ostream& operator<<(std::ostream& oss, const std::tuple<T1,T2,T3>& t) {
    oss << "(" 
        << std::get<0>(t) << ","
        << std::get<1>(t) << ","
        << std::get<2>(t) 
        << ")";
    return oss;
}

// Pretty print a map
template<class K, class V>
std::ostream& operator<<(std::ostream& oss, const std::map<K,V>& m) {
    oss << "{";
    if (!m.empty()) {
        typename std::map<K,V>::const_iterator it = m.begin();
        oss << it->first << " : " << it->second;
        for (; it != m.end(); ++it)
            oss << ", " << it->first << " : " << it->second;
    }
    oss << "}";
    return oss; 
}

#endif // UTILS_IOUTILS_HPP
