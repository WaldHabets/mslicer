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

#define TOTAL_MAP_DIM 7424
#define PNG ".png "
#define JPG ".jpg "
#define NO_OUTPUT " >/dev/null 2>/dev/null"

// Arguments
#define ARG_MAX_LAYERS 3

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
    command << " && " << REMOVE_PNG(file) << NO_OUTPUT << std::endl;
    execute(command);
}

void create_tarball_of(const std::string& dir) {
    std::stringstream command;
    command << "tar czf " << dir << ".tar.gz ";
    command << dir << std::endl;
    execute(command);
}

int main(int argc, char* argv[])
{

    Options options = parse_args(argc, argv);

    const std::string OUTPUT_DIR = options.output_dir;
    const std::string INPUT_FILE = options.input_file;

    if (stat(OUTPUT_DIR.c_str(), &st) == -1)
    {
        std::cout << OUTPUT_DIR << " does not exist" << std::endl;
        std::cout << "Creating new directory..." << std::endl;

        int res = mkdir(OUTPUT_DIR.c_str(), S_IRWXU);

        if (res != 0)
        {
            std::cout << strerror(errno) << std::endl;
        }
    }

    for (int z = options.min_zoom; z <= options.max_zoom; ++z)
    {

        std::stringstream dir;

        dir << OUTPUT_DIR << Z_DIR(z);

        if (stat(dir.str().c_str(), &st) == -1)
	    {
            int res = mkdir(dir.str().c_str(), S_IRWXU);

            if (res != 0)
            {
                std::cout << strerror(errno) << std::endl;
            }
        }
    }

    for (int z = options.min_zoom; z <= options.max_zoom; ++z)
    {
        const int chunksize = TOTAL_MAP_DIM / pow(2, z);
        const int num_chunks = TOTAL_MAP_DIM / chunksize;
        const float total = num_chunks * num_chunks;
        int progress = 0;

        #pragma omp parallel for collapse(2)
        for (int y = 0; y < num_chunks; ++y)
        {

            for (int x = 0; x < num_chunks; ++x)
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

                const int x_origin = x*chunksize;
                const int y_origin = y*chunksize;

                const int x_end = x_origin + chunksize;
                const int y_end = y_origin + chunksize;

                std::stringstream cmd;

                cmd << INKSCAPE;
                cmd << INPUT_FILE << " ";
                cmd << OPTION_AREA << x_origin << ":" << y_origin << ":" << x_end << ":" << y_end << " ";
                cmd << OPTION_WIDTH(options.tile_dim) << OPTION_HEIGHT(options.tile_dim) << OPTION_EXPORT_TO(FILE);
                cmd << NO_OUTPUT << std::endl;

                execute(cmd);

                // convert and remove
                convert_to_jpg(FILE);
            }
        }

        std::cout
            << "Working layer " << z << " of " << options.max_zoom << "...\tDone                         "
            << std::endl;
    }

    create_tarball_of(OUTPUT_DIR);

    return 0;
}
