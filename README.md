![Banner](img/mslicer-banner.png)

mSlicer is a command-line utility to export a vector drawing to rasterized [tiles](https://wiki.openstreetmap.org/wiki/Tiles). These square bitmap graphics can susequently be displayed in a grid arrangement by tools such as [Leaflet.js](https://leafletjs.com/) or [OpenLayers](https://openlayers.org/) to show a map or verry large image.

mSlicer differers from tools such as [mapnik](https://mapnik.org/) in that it takes a vector (.svg) drawing and that it slices that drawing to generate a static set of tiles at multiple zoom levels. mSlicer uses a multithreaded approuch to generate its tiles concurrently.

# Dependencies
List of program dependencies

## Libraries
- OpenMP
- Boost

## External programs
The tool depends on a few external programs to properly function. There are plans to remove these depedencies somewhere in the future.

- Inkscape 
- Convert _Currently required due to an issue with inkscape not properly exporting to .jpg_
