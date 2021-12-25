#include "argparse.hpp"

#include <iostream>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options;

#define ARG_VALUE_INT po::value<int>()
#define ARG_VALUE_STR po::value<std::string>()

/**
 * Builds the options descriptions
 * */
po::options_description
init_options()
{
    po::options_description description("Options");

    description.add_options()
        (ARG_HELP, "Show help options")
        (ARG_ARCHIVE, "Creates a compressed archive of the output images. This optios is the same as running `tar czf` on the output dir.")
        (ARG_MAX_ZOOM, ARG_VALUE_INT->default_value(DEFAULT_MAX_ZOOM), "The maximum zoom level, must be larger than or equal to 0 and more than --min-zoom.")
        (ARG_MIN_ZOOM, ARG_VALUE_INT->default_value(DEFAULT_MIN_ZOOM), "The minimum zoom level, must be larger than or equal to 0 and less than --max-zoom.")
        (ARG_TILE_DIM, ARG_VALUE_INT->default_value(DEFAULT_TILE_DIM), "The dimension of the generated tiles")
        (ARG_INPUT_FILE, ARG_VALUE_STR->required(), "Input file in .svg format")
        (ARG_OUTPUT_DIR, ARG_VALUE_STR->required(), "Name of the target directory where output should be stored")
        ;
    return description;
}


void exit_with_help(const po::options_description& description) {
    std::cout << description << std::endl;
    exit(1);
}

/**
 * Take an constant and options of the form "long-option,short"
 * and returns the "long-option"
 * */
std::string get_long_option(const char* option) {
    char* option_copy = strdup(option);
    std::string long_option = strtok(option_copy, ",");
    free(option_copy);
    return long_option;
}

/**
 * Checks if the options exists on the value map
 * */
bool check(const char* option, const po::variables_map* vm) {
    return vm->count(get_long_option(option));
}

Options populate_options(const po::variables_map* vm) {
    Options options;

    options.archive = vm->count(get_long_option(ARG_ARCHIVE)) > 0;
    options.max_zoom = (*vm)[get_long_option(ARG_MAX_ZOOM)].as<int>();
    options.min_zoom = (*vm)[get_long_option(ARG_MIN_ZOOM)].as<int>();
    options.tile_dim = (*vm)[get_long_option(ARG_TILE_DIM)].as<int>();
    options.input_file = (*vm)[get_long_option(ARG_INPUT_FILE)].as<std::string>();
    options.output_dir = (*vm)[get_long_option(ARG_OUTPUT_DIR)].as<std::string>();

    return options;
}

/**
 * Validates the program options
 * @returns true if the options are valid
 * */
bool validate_options(const Options& options) {
    if (options.min_zoom < 0) {
        std::cerr << "Error: " << get_long_option(ARG_MIN_ZOOM) << " must be greather than or equal to 0.";
        return false;
    }
    if (options.max_zoom < 0) {
        std::cerr << "Error: " << get_long_option(ARG_MAX_ZOOM) << " must be greather than or equal to 0.";
        return false;
    }
    if (options.max_zoom < options.min_zoom) {
        std::cerr << "Error: " << get_long_option(ARG_MAX_ZOOM) << " must be greather than or equal to " << get_long_option(ARG_MIN_ZOOM);
        return false;
    }
    return true;
}

/**
 * Parse the command line input and stores it into an Options struct
 * */
Options
parse_args(int argc, char* argv[]) 
{
    po::options_description description = init_options();

    po::positional_options_description p;
    p.add(get_long_option(ARG_INPUT_FILE).c_str(), 1);
    p.add(get_long_option(ARG_OUTPUT_DIR).c_str(), 1);

    po::variables_map vm;

    po::store(po::command_line_parser(argc, argv)
        .options(description)
        .positional(p)
        .run(), vm);

    if (check(ARG_HELP, &vm)) {
        exit_with_help(description);
    }

    try
    {
        // check for missing arguments or other errors
        po::notify(vm);
    } 
    catch (const po::error &err)
    {
        std::cerr << err.what() << std::endl;
        exit_with_help(description);
    }

    Options options = populate_options(&vm);
    if (!validate_options(options)) {
        exit_with_help(description);
    }

    return options;
}