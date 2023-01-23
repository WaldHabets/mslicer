#include "filesystem.hpp"

struct stat status = {0};

void initialize_output_dirs(const std::string& dir, const int min_zoom, const int max_zoom) 
{
    create_dir_if_missing(dir);

    for (int z = min_zoom; z <= max_zoom; ++z)
    {
        std::stringstream z_dir;

        z_dir << dir << Z_DIR(z);

        create_dir_if_missing(z_dir.str());
    }
}

void create_dir_if_missing(const std::string& dir, bool verbose) 
{
    const char* directory = dir.c_str();
    if (stat(directory, &status) == -1)
    {
        int res = mkdir(directory, S_IRWXU);

        if (verbose && res != 0)
        {
            std::cerr << strerror(errno) << std::endl;
        }
    }
}
