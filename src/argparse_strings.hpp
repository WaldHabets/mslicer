#ifndef GUARD_ARGPARSE_STRINGS_HPP
#define GUARD_ARGPARSE_STRINGS_HPP

#define MSG_ARG_HELP            "Show help options"
#define MSG_ARG_ARCHIVE         "Creates a compressed archive of the output images. This optios is the same as running `tar czf` on the output dir."
#define MSG_ARG_VERBOSE         "Show error messages"
#define MSG_ARG_FORMAT          "Format of the outputted tiles, either png or jpg."
#define MSG_ARG_ZOOM            "Overwrite --max-zoom and --min-zoom to the zoom level. Outputs tiles for a single zoom level."
#define MSG_ARG_MAX_ZOOM        "The maximum zoom level, must be larger than or equal to 0 and more than --min-zoom."
#define MSG_ARG_MIN_ZOOM        "The minimum zoom level, must be larger than or equal to 0 and less than --max-zoom."
#define MSG_ARG_TILE_DIM        "The dimension of the generated tiles"
#define MSG_ARG_INPUT_WIDTH     "Set the width of the input image. If neither --input-width or --input-dim is set or --input-width=0, the program will attempt to derive the width from the input image."
#define MSG_ARG_INPUT_HEIGHT    "Set the height of the input image. If neither --input-height or --input-dim is set or --input-height=0, the program will attempt to derive the height from the input image."
#define MSG_ARG_INPUT_DIM       "Set the dimension of the input image: overrides --input-width and --input-height."
#define MSG_ARG_INPUT_FILE      "Input file in .svg format"
#define MSG_ARG_OUTPUT_DIR      "Name of the target directory where output should be stored"

#endif // GUARD_ARGPARSE_STRINGS_HPP