#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <omp.h> // compile with -fopenmp
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <errno.h>
#include <string.h>
#include <iomanip>
#include "argparse.hpp"
#include "xml.hpp"

#define TOTAL_MAP_DIM 7424
#define PNG ".png "
#define JPG ".jpg "
#define NO_OUTPUT " >/dev/null"
#define NO_ERROR " 2>/dev/null"

// Inkscape
#define INKSCAPE "inkscape "
#define OPTION_AREA "--export-area="
#define OPTION_WIDTH(DIM) "--export-width=" << DIM << " "
#define OPTION_HEIGHT(DIM) "--export-height=" << DIM << " "
#define OPTION_EXPORT_TO(FILE) "--export-filename=" << FILE << PNG

// Other
#define Z_DIR(z) "/" << z << "/" 
#define REMOVE_PNG(FILE) "rm " << FILE << PNG
#define CONVERT_TO_JPG(FILE) "convert " << FILE << PNG << FILE << JPG
#define FORMAT_FLOAT(FLOAT) std::fixed << std::setprecision(2) << FLOAT

struct stat st = {0};

void execute(const std::stringstream& command) {
    system(command.str().c_str());
}

void convert_to_jpg(const std::string& file) {
    std::stringstream command;
    command << CONVERT_TO_JPG(file) << NO_OUTPUT;
    command << " && " << REMOVE_PNG(file) << NO_OUTPUT << NO_ERROR << std::endl;
    execute(command);
}

void create_tarball_of(const std::string& dir) {
    std::stringstream command;
    command << "tar czf " << dir << ".tar.gz ";
    command << dir << std::endl;
    execute(command);
}

/**
 * Creates the target directory if it does not yet exists
 * */
void create_dir_if_missing(const std::string& dir, bool verbose = true) {
    const char* directory = dir.c_str();
    if (stat(directory, &st) == -1)
    {
        int res = mkdir(directory, S_IRWXU);

        if (verbose && res != 0)
        {
            std::cerr << strerror(errno) << std::endl;
        }
    }
}

int main(int argc, char* argv[])
{

    Options options = parse_args(argc, argv);

    const std::string OUTPUT_DIR = options.output_dir;
    const std::string INPUT_FILE = options.input_file;

    const SvgDimension dim = parse_dimension(INPUT_FILE);

    if (options.input_width == 0 && dim.width == PROPERTY_IS_MISSING) {
        std::cerr << "Aborting: Property 'width' is missing on the file and not passed as an argument" << std::endl;
    } else if (options.input_width == 0) {
        options.input_width = dim.width;
    }

    if (options.input_height == 0 && dim.height == PROPERTY_IS_MISSING) {
        std::cerr << "Aborting: Property 'height' is missing on the file and not passed as an argument" << std::endl;
    } else if (options.input_height == 0) {
        options.input_height = dim.height;
    }

    create_dir_if_missing(OUTPUT_DIR);

    for (int z = options.min_zoom; z <= options.max_zoom; ++z)
    {
        std::stringstream z_dir;

        z_dir << OUTPUT_DIR << Z_DIR(z);

        create_dir_if_missing(z_dir.str());
    }

    for (int z = options.min_zoom; z <= options.max_zoom; ++z)
    {
        // const int chunksize = TOTAL_MAP_DIM / pow(2, z);
        // const int num_chunks = TOTAL_MAP_DIM / chunksize;
        // const float total = num_chunks * num_chunks;
        // int progress = 0;

        const int chunksize_x = options.input_width / pow(2, z);
        const int chunksize_y = options.input_height / pow(2, z);
        const int num_chunks_x = options.input_width / chunksize_x;
        const int num_chunks_y = options.input_height / chunksize_y;
        const float total = num_chunks_x * num_chunks_y;
        int progress = 0;

        #pragma omp parallel for collapse(2)
        for (int y = 0; y < num_chunks_y; ++y)
        {

            for (int x = 0; x < num_chunks_x; ++x)
            { 
                #pragma omp critical
                {
                    ++progress;
                    std::cout
                        << "Working layer " << z << " of " << options.max_zoom << "...\t"
                        << progress << "/" << total
                        << " (" << FORMAT_FLOAT(progress / total * 100) << "%)\t\r"
                        << std::flush;
                }

                std::stringstream path_builder;
                path_builder << OUTPUT_DIR << Z_DIR(z) << x << "_" << y; // extension gets appended at later stage

                const std::string FILE = path_builder.str();

                const int x_origin = x*chunksize_x;
                const int y_origin = y*chunksize_y;

                const int x_end = x_origin + chunksize_x;
                const int y_end = y_origin + chunksize_y;

                std::stringstream cmd;

                cmd << INKSCAPE;
                cmd << INPUT_FILE << " ";
                cmd << OPTION_AREA << x_origin << ":" << y_origin << ":" << x_end << ":" << y_end << " ";
                cmd << OPTION_WIDTH(options.tile_dim) << OPTION_HEIGHT(options.tile_dim) << OPTION_EXPORT_TO(FILE);
                cmd << NO_OUTPUT;
                if (!options.verbose)
                    cmd << NO_ERROR << std::endl;
                else
                    cmd << std::endl;

                execute(cmd);

                // convert and remove
                // We manually convert due to a bug in inkscape trowing
                // ** (org.inkscape.Inkscape:87826): ERROR **: 15:43:29.594: 
                //      unhandled exception (type unknown) in signal handler
                // when we add jpg as the file extension to --export-filename
                if (options.format.compare("jpg") == 0)
                    convert_to_jpg(FILE);
            }
        }

        std::cout
            << "Working layer " << z << " of " << options.max_zoom << "...\tDone                         "
            << std::endl;
    }

    if (options.archive)
        create_tarball_of(OUTPUT_DIR);

    return 0;
}
