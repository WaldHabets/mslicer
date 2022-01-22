#define BOOST_TEST_MODULE xml test

#include <boost/test/unit_test.hpp>
#include "../src/xml.hpp"

#define GOOD_SVG_FILE "./data/mslicer.svg"
#define MISSING_WIDTH_FILE "./data/mslicer_missing_width.svg"
#define MISSING_HEIGHT_FILE "./data/mslicer/missing_height.svg"

BOOST_AUTO_TEST_CASE(dims_detection_test) {
    Dimension dim = parse_dimension(GOOD_SVG_FILE);
    BOOST_TEST(dim.width == 256);
    BOOST_TEST(dim.height == 256);
}

BOOST_AUTO_TEST_CASE(width_missing_test) {
    BOOST_CHECK_THROW(parse_dimension(MISSING_WIDTH_FILE), MissingPropertyException);
}

BOOST_AUTO_TEST_CASE(height_missing_test) {
    BOOST_CHECK_THROW(parse_dimension(MISSING_HEIGHT_FILE), MissingPropertyException);
}