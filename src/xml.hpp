#ifndef GUARD_XML_HPP
#define GUARD_XML_HPP

#include <string>

#define PROPERTY_IS_MISSING -1.0f

struct dim {
    float width;
    float height;
} typedef SvgDimension;

class MissingPropertyException
    : public std::exception {
public:
    MissingPropertyException(bool missing_width, bool missing_height) {
        m_missing_width = missing_width;
        m_missing_height = missing_height;
    }

    bool isMissingWidth() { return m_missing_width; }
    bool isMissingHeight() { return m_missing_height; }

private:
    bool m_missing_width = false;
    bool m_missing_height = false;
};

SvgDimension parse_dimension(const std::string& path);


#endif  // GUARD_XML_HPP