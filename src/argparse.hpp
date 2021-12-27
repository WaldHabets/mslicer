#ifndef GUARD_ARGPARSE_HPP
#define GUARD_ARGPASRE_HPP

#include <string>

#define ARG_HELP "help,?"
#define ARG_ARCHIVE "archive,a"
#define ARG_FORMAT "format,f"
#define ARG_MAX_ZOOM "max-zoom"
#define ARG_MIN_ZOOM "min-zoom"
#define ARG_TILE_DIM "tile-dim"
#define ARG_INPUT_FILE "input-file,i"
#define ARG_OUTPUT_DIR "output-dir,o"

#define DEFAULT_FORMAT "jpg"
#define DEFAULT_MIN_ZOOM 0
#define DEFAULT_MAX_ZOOM 2
#define DEFAULT_TILE_DIM 512

struct options {
    bool archive;
    std::string format;
    int min_zoom;
    int max_zoom;
    int tile_dim;
    std::string input_file;
    std::string output_dir;
} typedef Options;

Options parse_args(int argc, char* argv[]);

#endif // GUARD_ARGPARSE_HPP