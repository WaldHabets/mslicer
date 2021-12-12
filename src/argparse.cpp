#include <iostream>

void find_key() {

}

void print_help() {
    std::cout
        << "options <input_file> <output_dir> [options]"
        << std::endl;
}

void parse(int argc, char* argv[]) {
    if (argc < 3) {
        print_help();
        exit(0);
    }
}