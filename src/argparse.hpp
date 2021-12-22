#ifndef GUARD_ARGPARSE_HPP
#define GUARD_ARGPASRE_HPP

#include <string>

#define ARG_HELP "help,?"
#define ARG_MAX_ZOOM "max-zoom"
#define ARG_MIN_ZOOM "min-zoom"
#define ARG_INPUT_FILE "input-file,i"
#define ARG_OUTPUT_DIR "output-dir,o"

#define DEFAULT_MIN_ZOOM 0
#define DEFAULT_MAX_ZOOM 2

struct options {
    int min_zoom;
    int max_zoom;
    std::string input_file;
    std::string output_dir;
} typedef Options;

Options parse_args(int argc, char* argv[]);

#endif // GUARD_ARGPARSE_HPP