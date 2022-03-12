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

#define ERROR_GE_0(ARGUMENT) std::cerr << "Error: " << ARGUMENT << " must be greather than or equal to 0."

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
        (ARG_VERBOSE, "Show error messages")
        (ARG_FORMAT, ARG_VALUE_STR->default_value(DEFAULT_FORMAT), "Format of the outputted tiles, either png or jpg.")
        (ARG_ZOOM, ARG_VALUE_INT, "Overwrite --max-zoom and --min-zoom to the zoom level. Outputs tiles for a single zoom level.")
        (ARG_MAX_ZOOM, ARG_VALUE_INT->default_value(DEFAULT_MAX_ZOOM), "The maximum zoom level, must be larger than or equal to 0 and more than --min-zoom.")
        (ARG_MIN_ZOOM, ARG_VALUE_INT->default_value(DEFAULT_MIN_ZOOM), "The minimum zoom level, must be larger than or equal to 0 and less than --max-zoom.")
        (ARG_TILE_DIM, ARG_VALUE_INT->default_value(DEFAULT_TILE_DIM), "The dimension of the generated tiles")
        (ARG_INPUT_WIDTH, ARG_VALUE_INT->default_value(DEFAULT_INPUT_DIM), "Set the width of the input image. If neither --input-width or --input-dim is set or --input-width=0, the program will attempt to derive the width from the input image.")
        (ARG_INPUT_HEIGHT, ARG_VALUE_INT->default_value(DEFAULT_INPUT_DIM), "Set the height of the input image. If neither --input-height or --input-dim is set or --input-height=0, the program will attempt to derive the height from the input image.")
        (ARG_INPUT_DIM, ARG_VALUE_INT, "Set the dimension of the input image: overrides --input-width and --input-height.")
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

    if (check(ARG_INPUT_DIM, vm) > 0) {
        int dim = (*vm)[get_long_option(ARG_INPUT_DIM)].as<int>();
        options.input_width = dim;
        options.input_height = dim;
    } else {
        options.input_width = (*vm)[get_long_option(ARG_INPUT_WIDTH)].as<int>();
        options.input_height = (*vm)[get_long_option(ARG_INPUT_HEIGHT)].as<int>();
    }
    options.archive = vm->count(get_long_option(ARG_ARCHIVE)) > 0;
    options.verbose = vm->count(get_long_option(ARG_VERBOSE)) > 0;
    options.format = (*vm)[get_long_option(ARG_FORMAT)].as<std::string>();
    
    if (check(ARG_ZOOM, vm) > 0) {
        int zoom = (*vm)[get_long_option(ARG_ZOOM)].as<int>();
        options.max_zoom = zoom;
        options.min_zoom = zoom;
    } else {
        options.max_zoom = (*vm)[get_long_option(ARG_MAX_ZOOM)].as<int>();
        options.min_zoom = (*vm)[get_long_option(ARG_MIN_ZOOM)].as<int>();
    }

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
    if (options.input_height < 0) {
        ERROR_GE_0(get_long_option(ARG_INPUT_HEIGHT));
        return false;
    }
    if (options.input_width < 0) {
        ERROR_GE_0(get_long_option(ARG_INPUT_WIDTH));
        return false;
    }
    if (options.min_zoom < 0) {
        ERROR_GE_0(get_long_option(ARG_MIN_ZOOM));
        return false;
    }
    if (options.max_zoom < 0) {
        ERROR_GE_0(get_long_option(ARG_MAX_ZOOM));
        return false;
    }
    if (options.max_zoom < options.min_zoom) {
        std::cerr << "Error: " << get_long_option(ARG_MAX_ZOOM) << " must be greather than or equal to " << get_long_option(ARG_MIN_ZOOM);
        return false;
    }
    if (!(options.format.compare("jpg") || options.format.compare("png"))) {
        std::cerr << "Error: " << get_long_option(ARG_FORMAT) << " must be either png or jpg.";
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