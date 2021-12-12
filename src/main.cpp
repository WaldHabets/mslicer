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

#define TOTAL_MAP_DIM 7424
#define EXPORT_DIM 500
#define PNG ".png "
#define JPG ".jpg "
#define NO_OUTPUT " >/dev/null 2>/dev/null"

// Arguments
#define ARG_INPUT_FILE 1
#define ARG_OUTPUT_DIR 2
#define ARG_MAX_LAYERS 3

// Inkscape
#define INKSCAPE "inkscape "
#define OPTION_AREA "--export-area="
#define OPTION_WIDTH "--export-width=" << EXPORT_DIM << " "
#define OPTION_HEIGHT "--export-height=" << EXPORT_DIM << " "
#define OPTION_EXPORT_TO(FILE) "--export-filename=" << FILE << PNG

// Other
#define Z_DIR(z) "/" << z << "/" 
#define REMOVE_PNG(FILE) "rm " << FILE << PNG
#define CONVERT_TO_JPG(FILE) "convert " << FILE << PNG << FILE << JPG

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

    if (argc != 4) {
        std::cout << "export <input_file> <output_dir> <layers>" << std::endl;
        exit(0);
    }

    int zoom = 0;

    try {
        zoom = std::stoi(argv[ARG_MAX_LAYERS]);
    } catch(std::invalid_argument) {
        std::cout << "Argument for <layers> cannot be converted to an integer" << std::endl;
        exit(0);
    } catch(std::out_of_range) {
        std::cout << "Argument for <layers> is to large." << std::endl;
        exit(0);
    }


    if (zoom < 0) {
	    std::cout << "Invalid zoom" << std::endl;
	    exit(0);
    }

    const std::string OUTPUT_DIR = argv[ARG_OUTPUT_DIR];
    const std::string INPUT_FILE = argv[ARG_INPUT_FILE];

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

    for (int z = 0; z <= zoom; ++z)
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

    for (int z = 0; z <= zoom; ++z)
    {
        const int chunksize = TOTAL_MAP_DIM / pow(2, z);
        const int num_chunks = TOTAL_MAP_DIM / chunksize;
        const int total = num_chunks * num_chunks;
        int progress = 0;

        #pragma omp parallel for collapse(2) shared(progress)
        for (int y = 0; y < num_chunks; ++y)
        {

            for (int x = 0; x < num_chunks; ++x)
            {
                #pragma omp critical
                {
                    ++progress;
                    std::cout << progress << "/" << total 
                              << "\t(Layer " << z << " of " << zoom << ")"
                              << std::endl;
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
                cmd << OPTION_WIDTH << OPTION_HEIGHT << OPTION_EXPORT_TO(FILE);
                cmd << NO_OUTPUT << std::endl;

                execute(cmd);

                // convert and remove
                convert_to_jpg(FILE);
            }

        }
    }

    create_tarball_of(OUTPUT_DIR);

    return 0;
}
