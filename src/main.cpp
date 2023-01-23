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
#include <ctime>
#include <chrono>
#include "argparse.hpp"
#include "filesystem.hpp"
#include "progress_tracker.hpp"
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
#define REMOVE_PNG(FILE) "rm " << FILE << PNG
#define CONVERT_TO_JPG(FILE) "convert " << FILE << PNG << FILE << JPG
#define FORMAT_FLOAT(FLOAT) std::fixed << std::setprecision(0) << FLOAT

#define TO_SECONDS(NANOSECONDS) std::fixed << std::setprecision(0) << int(NANOSECONDS / 1000000000) % 60 << "s"
#define TO_MINUTES(NANOSECONDS) std::fixed << std::setprecision(0) << int(NANOSECONDS / 60000000000) % 60 << "m"
#define TO_HOURS(NANOSECONDS) std::fixed << std::setprecision(0) << NANOSECONDS / 3600000000000 << "h"

#define FORMAT_TIME(NANOSECONDS) TO_HOURS(NANOSECONDS) << TO_MINUTES(NANOSECONDS) << TO_SECONDS(NANOSECONDS)

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

std::string format_active_zoom(double zoom, double max) {
    std::stringstream stream;
    stream << zoom << "/" << max;
    return stream.str();
}

std::string format_progress(double progress, double max, double percent) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(0);
    stream << progress << " / " << max;
    stream << " ( " << percent << "%)";
    return stream.str();
}

std::string format_time(double nanoseconds) {
    std::stringstream stream;
    stream << FORMAT_TIME(nanoseconds);
    return stream.str();
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

    initialize_output_dirs(OUTPUT_DIR, options.min_zoom, options.max_zoom);

    // Pre Calculation to help determine time
    int total_chunks = 0;
    for (int z = options.min_zoom; z <= options.max_zoom; ++z)
    {
        const int chunksize_x = options.input_width / pow(2, z);
        const int chunksize_y = options.input_height / pow(2, z);
        const int num_chunks_x = options.input_width / chunksize_x;
        const int num_chunks_y = options.input_height / chunksize_y;
        total_chunks += num_chunks_x * num_chunks_y;
    }


    ProgressTracker tracker;// = new ProgressTracker();
    tracker.start(total_chunks);

    std::cout << "Slicing " << INPUT_FILE << std::endl;
    std::cout << "Zoom        : " << options.min_zoom << " - " << options.max_zoom << std::endl;
    std::cout << "Total chunks: " << total_chunks << std::endl << std::endl;

    std::cout << "| Zoom                                                      | Total                             |" << std::endl;
    std::cout << "| Nr.       | Status                                        | Elapsed Time    | Estimated Time  |" << std::endl;
    std::cout << "|-----------|-----------------------------------------------|-----------------|-----------------|" << std::endl;


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

        tracker.tick_zoom(total);

        #pragma omp parallel for collapse(2)
        for (int y = 0; y < num_chunks_y; ++y)
        {

            for (int x = 0; x < num_chunks_x; ++x)
            { 
                #pragma omp critical
                {
                    tracker.tick();

                    std::cout
                        << std::setprecision(0)
                        << "| " << std::setw(9) << std::left << format_active_zoom(z, options.max_zoom)
                        << " | " << std::setw(45) << std::left << format_progress(tracker.get_zoom_progress_count(), total, tracker.get_zoom_progress_percent())
                        << " | " << std::setw(15) << std::left << format_time(tracker.get_elapsed_time())
                        << " | " << std::setw(15) << std::left << format_time(tracker.get_estimated_time()) << " |\r"
                        << std::flush;
                }

                std::stringstream path_builder;
                path_builder << OUTPUT_DIR << Z_DIR(z) << x << "_" << y; // extension gets appended at later stage

                const std::string FILE = path_builder.str();

                // Calculate the top-left coordinate
                const int x_origin = x*chunksize_x;
                const int y_origin = y*chunksize_y;

                // Calculate the bottom-right coordinate
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
            << std::setprecision(0)
            << "| " << std::setw(9) << std::left << format_active_zoom(z, options.max_zoom)
            << " | " << std::setw(45) << std::left << format_progress(total, total, 100)
            << " | " << std::setw(15) << std::left << "" << " | " << std::setw(15) << std::left << "" << " |"
            << std::endl;
    }

    tracker.tick();
    std::cout << "|-----------|-----------------------------------------------|-----------------|-----------------|" << std::endl << std::endl;
    std::cout << "Slicing completed in " << FORMAT_TIME(tracker.get_elapsed_time()) << std::endl << std::endl;

    if (options.archive)
        create_tarball_of(OUTPUT_DIR);

    std::cout << "DONE" << std::endl;

    return 0;
}
