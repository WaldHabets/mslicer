#ifndef GUARD_XML_HPP
#define GUARD_XML_HPP

#include <string>

struct dim {
    float width;
    float height;
} typedef Dimension;

class MissingPropertyException
    : public std::exception {

};

Dimension parse_dimension(const std::string& path);


#endif  // GUARD_XML_HPP