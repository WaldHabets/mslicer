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
#include <chrono>
// c headers
#include <ctime>
#include <csignal>
// project headers
#include "argparse.hpp"
#include "filesystem.hpp"
#include "progress_tracker.hpp"
#include "xml.hpp"
#include "render.hpp"

#define PNG ".png "
#define JPG ".jpg "
#define WEBP ".webp "
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
#define CONVERT_TO_WEBP(FILE) "convert " << FILE << PNG << FILE << WEBP
#define FORMAT_FLOAT(FLOAT) std::fixed << std::setprecision(0) << FLOAT

#define TO_SECONDS(NANOSECONDS) std::fixed << std::setprecision(0) << int(NANOSECONDS / 1000000000) % 60 << "s"
#define TO_MINUTES(NANOSECONDS) std::fixed << std::setprecision(0) << int(NANOSECONDS / 60000000000) % 60 << "m"
#define TO_HOURS(NANOSECONDS) std::fixed << std::setprecision(0) << NANOSECONDS / 3600000000000 << "h"

#define FORMAT_TIME(NANOSECONDS) TO_HOURS(NANOSECONDS) << TO_MINUTES(NANOSECONDS) << TO_SECONDS(NANOSECONDS)

int execute(std::stringstream const & command) {
    return system(command.str().c_str());
}

void convert_to_jpg(std::string const & file) {
    std::stringstream command;
    command << CONVERT_TO_JPG(file) << NO_OUTPUT;
    command << " && " << REMOVE_PNG(file) << NO_OUTPUT << NO_ERROR << std::endl;
    execute(command);
}

void convert_to_webp(std::string const & file) {
    std::stringstream command;
    command << CONVERT_TO_WEBP(file) << NO_OUTPUT;
    command << " && " << REMOVE_PNG(file) << NO_OUTPUT << NO_ERROR << std::endl;
    execute(command);
}

std::string remove_trailing_slash(std::string dir) {
    if (dir.size() > 0 && dir.back() == '/') {
        dir.pop_back();
    }
    return dir;
}

void create_tarball_of(std::string const & dir) {
    std::stringstream command;
    command << "tar czf " << dir << ".tar.gz ";
    command << dir << std::endl;
    execute(command);
}

std::string format_active_zoom(double const zoom, double const max) {
    std::stringstream stream;
    stream << zoom << "/" << max;
    return stream.str();
}

std::string format_progress(double const progress, double const max, double const percent) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(0);
    stream << progress << " / " << max;
    stream << " ( " << percent << "%)";
    return stream.str();
}

std::string format_time(double const nanoseconds) {
    std::stringstream stream;
    stream << FORMAT_TIME(nanoseconds);
    return stream.str();
}


volatile bool ABORT = false;

void interrupt_handler(sig_atomic_t s) {
    ABORT = true;
    std::cout << "Gracefully shutting down. Please wait a while..." << std::endl;
}

void setup_interrupt_handler() {
    struct sigaction sig_interrupt_handler;
    
    sig_interrupt_handler.sa_handler = interrupt_handler;
    sigemptyset(&sig_interrupt_handler.sa_mask);
    sig_interrupt_handler.sa_flags = 0;
    
    sigaction(SIGINT, &sig_interrupt_handler, NULL);
}


int main(int argc, char* argv[])
{
    // Attempt to catch an interrupt event (aka ctrl-c)
    // in order to set the ABORT flag
    // so that the system can gracefully shut down
    setup_interrupt_handler();


    Options options = parse_args(argc, argv);

    std::string const OUTPUT_DIR = options.output_dir;
    std::string const INPUT_FILE = options.input_file;

    SvgDimension const dim = parse_dimension(INPUT_FILE);

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

    const int full_image_width = options.input_width * (1/(options.x_end - options.x_start));
    const int full_image_height = options.input_height * (1/(options.y_end - options.y_start));

    // Pre Calculation to help determine time
    int total_chunks = 0;
    for (int z = options.min_zoom; z <= options.max_zoom; ++z)
    {
        const int chunksize_x = full_image_width / pow(2, z);
        const int chunksize_y = full_image_height / pow(2, z);
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
        if (ABORT) continue;

        // const int chunksize = TOTAL_MAP_DIM / pow(2, z);
        // const int num_chunks = TOTAL_MAP_DIM / chunksize;
        // const float total = num_chunks * num_chunks;
        // int progress = 0;

        
        const int chunksize_x = full_image_width / pow(2, z);
        const int chunksize_y = full_image_height / pow(2, z);
        const int num_chunks_x = options.input_width / chunksize_x;
        const int num_chunks_y = options.input_height / chunksize_y;
        const int num_chunks_x_full_image = full_image_width / chunksize_x;
        const int num_chunks_y_full_image = full_image_height / chunksize_y;
        const int x_name_offset = num_chunks_x_full_image * options.x_start;
        const int y_name_offset = num_chunks_y_full_image * options.y_start;

        const float total = num_chunks_x * num_chunks_y;

        const double canvas_x = options.tile_dim * num_chunks_x;
        const double canvas_y = options.tile_dim * num_chunks_y;
        std::cout << "canvas size: " << (int)canvas_x << ":" << (int)canvas_y << std::endl;

        tracker.tick_zoom(total);

        #pragma omp parallel for collapse(2) shared(ABORT)
        for (int y = 0; y < num_chunks_y; ++y)
        {

            for (int x = 0; x < num_chunks_x; ++x)
            { 
                if (ABORT) continue;
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
                path_builder << OUTPUT_DIR << Z_DIR(z) << x_name_offset + x << "_" << y_name_offset + y; // extension gets appended at later stage

                const std::string FILE = path_builder.str();

                Viewport port = {
                    .x = -x*options.tile_dim,
                    .y = -y*options.tile_dim,
                    .width = canvas_x,
                    .height = canvas_y,
                };
                bool error = render(INPUT_FILE.c_str(), (FILE + ".png").c_str(), port, options.tile_dim);

                if (!error && options.format.compare("jpg") == 0)
                    convert_to_jpg(FILE);
                else if (!error && options.format.compare("webp") == 0)
                    convert_to_webp(FILE);
                else if (error)
                    std::cerr << "Render error occured, conversion skipped" << std::endl;
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

    if (!ABORT) {
        std::cout << "Slicing completed in " << FORMAT_TIME(tracker.get_elapsed_time()) << std::endl << std::endl;

        if (options.archive)
            create_tarball_of(OUTPUT_DIR);

        std::cout << "DONE" << std::endl;
    } else {
        std::cout << "ABORTED" << std::endl;
    }

    return 0;
}
