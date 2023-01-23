#include "xml.hpp"
#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace boost;
namespace pt = boost::property_tree;

/**
 * Parses the floating value of the specified attribute
 * If the attribute cannot 
 * @param root
 * The root of the svg file
 * @param attribute
 * The attribute path pointing at the floating value
 * */
float parse_non_null_positive_float_attr(const pt::ptree& root, const std::string attribute) {
    try {
        const std::string dim_string = root.get<std::string>(attribute);
        const float dim = strtof(dim_string.c_str(), NULL);
        if (dim == 0) {
            return PROPERTY_IS_MISSING;
        }
        return dim;
    } catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>) {
        return PROPERTY_IS_MISSING;
    }
}

SvgDimension parse_dimension(const std::string& path) {
    SvgDimension dim;
    dim.height = 0;
    dim.width = 0;

    pt::ptree root;
    pt::read_xml(path, root);

    dim.width = parse_non_null_positive_float_attr(root, "svg.<xmlattr>.width");
    dim.height = parse_non_null_positive_float_attr(root, "svg.<xmlattr>.height");

    return dim;
}

void set_display(const std::string& path, const std::string& id, bool display) {
    pt::ptree root;
    pt::read_xml(path, root);

    const pt::ptree& groups = root.get_child("g");

    // BOOST_FOREACH(pt::value_type& group, groups) 
    // {
    //     const pt::ptree& attributes = group.second.get_child("<xmlattr>");
    //     const std::string group_id = group.second.get<std::string>("<xmlattr>.id");

    //     if (group_id.compare(id) == 0) {
    //         std::string display_style = display ? "display:inline" : "display:none";
    //         try {
    //             const std::string style = group.second.get<std::string>("<xmlattr>.style");

    //         } catch (boost::wrapexcept<boost::property_tree::ptree_bad_path>) {
    //             const std::string style = display ? "display:inline" : "display:none";
                
    //         }
    //     }
    // }
}
