#include "xml.hpp"
#include <string>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace boost;
namespace pt = boost::property_tree;

SvgDimension parse_dimension(const std::string& path) {
    SvgDimension dim;
    dim.height = 0;
    dim.width = 0;

    pt::ptree root;
    pt::read_xml(path, root);

    bool missing_width = false;
    bool missing_height = false;

    std::string height_string = "";
    std::string width_string = "";

    try {
        width_string = root.get<std::string>("svg.<xmlattr>.width");
        dim.width = strtof(width_string.c_str(), NULL);
        if (dim.width == 0) {
            dim.width = PROPERTY_IS_MISSING;
        }
    } catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>) {
        dim.width = PROPERTY_IS_MISSING;
    }

    try {
        height_string = root.get<std::string>("svg.<xmlattr>.height");
        dim.height = strtof(width_string.c_str(), NULL);
        if (dim.width == 0) {
            dim.height = PROPERTY_IS_MISSING;
        }
    } catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>) {
        dim.height = PROPERTY_IS_MISSING;
    }

    if (missing_width || missing_height) {
        throw new MissingPropertyException(missing_width, missing_height);
    }
    

    std::cout << height_string << " - " << width_string << std::endl;

    return dim;
}