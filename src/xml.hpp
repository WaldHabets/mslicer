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

/**
 * Find the width and hight on a svg file
 * @param path
 * The root of the svg file
 * @return
 * A struct containing the width and height
 * @throws MissingPropertyException
 * if either the width or height are not defined on the svg
 * */
SvgDimension parse_dimension(const std::string& path);

/**
 * Set the style attribute "display" to either "display:inline" or "display:none" on a layer of the svg
 * Each layer should be defined as a "g" tag with the id property as a direct child of the "svg" root tag.
 * @param path
 * The root of the svg file
 * @param id
 * The id of the layer
 * @param display
 * The value of the "display" attribute
 * */
void set_display(const std::string& path, const std::string& id, bool display = true);


#endif  // GUARD_XML_HPP