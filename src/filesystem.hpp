#ifndef GUARD_FILESYSTEM_HPP
#define GUARD_FILESYSTEM_HPP

#include <sys/stat.h>
//#include <errno.h>
#include <string.h>
#include <string>
#include <sstream>
#include <iostream>

#define Z_DIR(z) "/" << z << "/" 

/**
 * Creates the target directory if it does not yet exists
 * */
void create_dir_if_missing(std::string const & dir, bool verbose = true);


/**
 * Initializes the output dirs with the following structure
 * dir/
 * ├─ min_zoom
 * ├─ ...
 * └─ max_zoom
 * */
void initialize_output_dirs(std::string const & dir, int const min_zoom, int const max_zoom);


#endif // GUARD_FILESYSTEM_HPP