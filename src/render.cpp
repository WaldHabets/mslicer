#include "render.hpp"

#include <cstdlib>

/**
 * Load svg into an rsvg_handle
 * Passes ownership of the handle upwards
 */
RsvgHandle * load_svg(
  char const * path
) {
  GFile * file = g_file_new_for_path(path);
  RsvgHandle * handle = rsvg_handle_new_from_gfile_sync(
    file, 
    RSVG_HANDLE_FLAGS_NONE, 
    NULL, 
    NULL
  );

  g_object_unref(file);

  return handle;
}


/**
 * Write surface to .png
 * Does not take ownership of the surface
 */
bool write_to_png(
  cairo_surface_t * surface, 
  char const * path
) {
  return cairo_surface_write_to_png(surface, path) != CAIRO_STATUS_SUCCESS;
}

bool write_to_jpg(
  cairo_surface_t * surface,
  char const * path
) {
  // should probably make use of cairo_surface_write_to_png_stream
}

bool render(
  char const * in_path,
  char const * out_path,
  Viewport viewport,
  unsigned int output_dim
) {
  GError *error = NULL;
  RsvgHandle * svg = load_svg(in_path);

  /* Create a Cairo image surface and a rendering context for it */

  cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, output_dim, output_dim);
  cairo_t *cr = cairo_create (surface);

  if (!rsvg_handle_render_document(svg, cr, &viewport, &error)) {
    return 1;
  }

  write_to_png(surface, out_path);

  // Free memory
  cairo_destroy (cr);
  cairo_surface_destroy (surface);
  g_object_unref(svg);
  return 0;
}
