#ifndef GUARD_RENDER_HPP
#define GUARD_RENDER_HPP

#include <librsvg/rsvg.h>

typedef RsvgRectangle Viewport;

RsvgHandle * load_svg(char const * path);

bool render(char const * in_path, char const * out_path, Viewport viewport, unsigned int output_dim);

#endif // GUARD_RENDER_HPP