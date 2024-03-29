#include "argparse.hpp"
#include "argparse_strings.hpp"

#include <iostream>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/parsers.hpp>

namespace po = boost::program_options;

#define ARGTYPE_INT po::value<int>()
#define ARGTYPE_STR po::value<std::string>()
#define ARGTYPE_FLOAT po::value<float>()
#define ARGTYPE_INT_DEFAULT(val) ARGTYPE_INT->default_value(val)
#define ARGTYPE_STR_DEFAULT(val) ARGTYPE_STR->default_value(val)
#define ARGTYPE_FLOAT_DEFAULT(val) ARGTYPE_FLOAT->default_value(val)
#define ARGTYPE_INT_REQUIRED ARGTYPE_INT->required()
#define ARGTYPE_STR_REQUIRED ARGTYPE_STR->required()

#define ERROR_GE_0(ARGUMENT) std::cerr << "Error: " << ARGUMENT << " must be greather than or equal to 0."

#define BETWEEN_0_AND_1(val) (val < 0 || val > 1)

/**
 * Builds the options descriptions
 * */
po::options_description
init_options()
{
    po::options_description description("Options");

    description.add_options()
        (ARG_HELP,          /* Boolean */                           MSG_ARG_HELP)
        (ARG_ARCHIVE,       /* Boolean */                           MSG_ARG_ARCHIVE)
        (ARG_VERBOSE,       /* Boolean */                           MSG_ARG_VERBOSE)
        (ARG_FORMAT,        ARGTYPE_STR_DEFAULT(DEFAULT_FORMAT),    MSG_ARG_FORMAT)
        (ARG_ZOOM,          ARGTYPE_INT,                            MSG_ARG_ZOOM)
        (ARG_MAX_ZOOM,      ARGTYPE_INT_DEFAULT(DEFAULT_MAX_ZOOM),  MSG_ARG_MAX_ZOOM)
        (ARG_MIN_ZOOM,      ARGTYPE_INT_DEFAULT(DEFAULT_MIN_ZOOM),  MSG_ARG_MIN_ZOOM)
        (ARG_TILE_DIM,      ARGTYPE_INT_DEFAULT(DEFAULT_TILE_DIM),  MSG_ARG_TILE_DIM)
        (ARG_INPUT_WIDTH,   ARGTYPE_INT_DEFAULT(DEFAULT_INPUT_DIM), MSG_ARG_INPUT_WIDTH)
        (ARG_INPUT_HEIGHT,  ARGTYPE_INT_DEFAULT(DEFAULT_INPUT_DIM), MSG_ARG_INPUT_HEIGHT)
        (ARG_INPUT_DIM,     ARGTYPE_INT,                            MSG_ARG_INPUT_DIM)
        (ARG_INPUT_FILE,    ARGTYPE_STR_REQUIRED,                   MSG_ARG_INPUT_FILE)
        (ARG_OUTPUT_DIR,    ARGTYPE_STR_REQUIRED,                   MSG_ARG_OUTPUT_DIR)
        (ARG_X_START,       ARGTYPE_FLOAT_DEFAULT(DEFAULT_START),   MSG_ARG_X_START)
        (ARG_Y_START,       ARGTYPE_FLOAT_DEFAULT(DEFAULT_START),   MSG_ARG_Y_START)
        (ARG_X_END,         ARGTYPE_FLOAT_DEFAULT(DEFAULT_END),     MSG_ARG_X_END)
        (ARG_Y_END,         ARGTYPE_FLOAT_DEFAULT(DEFAULT_END),     MSG_ARG_Y_END)
        ;
    return description;
}


void exit_with_help(po::options_description const & description) {
    std::cout << description << std::endl;
    exit(1);
}

/**
 * Take an constant and options of the form "long-option,short"
 * and returns the "long-option"
 * */
std::string get_long_option(char const * option) {
    char* option_copy = strdup(option);
    std::string long_option = strtok(option_copy, ",");
    free(option_copy);
    return long_option;
}

/**
 * Checks if the options exists on the value map
 * */
bool check(char const * option, po::variables_map const * vm) {
    return vm->count(get_long_option(option));
}

Options populate_options(po::variables_map const * vm) {
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

    options.x_start = (*vm)[get_long_option(ARG_X_START)].as<float>();
    options.y_start = (*vm)[get_long_option(ARG_Y_START)].as<float>();
    options.x_end = (*vm)[get_long_option(ARG_X_END)].as<float>();
    options.y_end = (*vm)[get_long_option(ARG_Y_END)].as<float>();

    return options;
}

/**
 * Validates the program options
 * @returns true if the options are valid
 * */
bool validate_options(Options const & options) {
    bool isValid = true;
    if (options.input_height < 0) {
        ERROR_GE_0(get_long_option(ARG_INPUT_HEIGHT));
        isValid = false;
    }
    if (options.input_width < 0) {
        ERROR_GE_0(get_long_option(ARG_INPUT_WIDTH));
        isValid = false;
    }
    if (options.min_zoom < 0) {
        ERROR_GE_0(get_long_option(ARG_MIN_ZOOM));
        isValid = false;
    }
    if (options.max_zoom < 0) {
        ERROR_GE_0(get_long_option(ARG_MAX_ZOOM));
        isValid = false;
    }
    if BETWEEN_0_AND_1(options.x_start) {
        std::cerr << "Error: value of --xstart must be between 0 and 1, current value is " << options.x_start << std::endl;
        isValid = false;
    }
    if BETWEEN_0_AND_1(options.y_start) {
        std::cerr << "Error: value of --ystart must be between 0 and 1" << std::endl;
        isValid = false;
    }
    if BETWEEN_0_AND_1(options.x_end) {
        std::cerr << "Error: value of --xend must be between 0 and 1" << std::endl;
        isValid = false;
    }
    if BETWEEN_0_AND_1(options.y_end) {
        std::cerr << "Error: value of --yend must be between 0 and 1" << std::endl;
        isValid = false;
    }
    if (options.x_start >= options.x_end) {
        std::cerr << "Error: value of --xstart (" << options.x_start << ") must be less then --xend (" << options.x_end << ")" << std::endl;
        isValid = false;
    }
    if (options.y_start >= options.y_end) {
        std::cerr << "Error: value of --ystart must be less then --yend" << std::endl;
        isValid = false;
    }
    if (options.max_zoom < options.min_zoom) {
        std::cerr << "Error: " << get_long_option(ARG_MAX_ZOOM) << " must be greather than or equal to " << get_long_option(ARG_MIN_ZOOM);
        isValid = false;
    }
    if (!(options.format.compare("jpg") || options.format.compare("png") || options.format.compare("webp"))) {
        std::cerr << "Error: " << get_long_option(ARG_FORMAT) << " must be either png, jpg or webp.";
        isValid = false;
    }
    return isValid;
}

/**
 * Parse the command line input and stores it into an Options struct
 * */
Options
parse_args(int argc, char * argv[]) 
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