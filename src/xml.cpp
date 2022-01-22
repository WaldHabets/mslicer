#include "xml.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace boost;
namespace pt = boost::property_tree;

Dimension parse_dimension(const std::string& path) {
    Dimension dim;
    dim.height = 0;
    dim.width = 0;

    pt::ptree root;
    pt::read_xml(path, root);

    const pt::ptree& svg = root.get_child("svg");

    return dim;
}