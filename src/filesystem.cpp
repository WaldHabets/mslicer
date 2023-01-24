#include "filesystem.hpp"

struct stat status = {0};

void initialize_output_dirs(std::string const & dir, int const min_zoom, int const max_zoom) 
{
    create_dir_if_missing(dir);

    for (int z = min_zoom; z <= max_zoom; ++z)
    {
        std::stringstream z_dir;

        z_dir << dir << Z_DIR(z);

        create_dir_if_missing(z_dir.str());
    }
}

void create_dir_if_missing(std::string const & dir, bool verbose) 
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
