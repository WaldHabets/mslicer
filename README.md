![Banner](img/mslicer-banner.png)

mSlicer is a command-line utility to export a vector drawing to rasterized [tiles](https://wiki.openstreetmap.org/wiki/Tiles). These square bitmap graphics can susequently be displayed in a grid arrangement by tools such as [Leaflet.js](https://leafletjs.com/) or [OpenLayers](https://openlayers.org/) to show a map or verry large image.

mSlicer differers from tools such as [mapnik](https://mapnik.org/) in that it takes a vector (.svg) drawing and that it slices that drawing to generate a static set of tiles at multiple zoom levels. mSlicer uses a multithreaded approuch to generate its tiles concurrently.

# Usage

⚠️ The slicer is designed to make full use of all availabe processing cores. Be aware that it will eat up all processing power.

| Argument | Function |
|---|---|
| -?, --help               | Show help options | 
| -a, --archive            | Creates a compressed archive of the output images. This optios is the same as running `tar czf` on the output dir.| 
| -v, --verbose            | Show error messages |
| -f, --format (=jpg)      | Format of the outputted tiles, either png or jpg. |
| -z, --zoom               | Overwrite --max-zoom and --min-zoom to the zoom level. Outputs tiles for a single zoom level. |
| --max-zoom (=2)          | The maximum zoom level, must be larger than or equal to 0 and more than --min-zoom. |
| --min-zoom (=0)          | The minimum zoom level, must be larger than or equal to 0 and less than --max-zoom. |
| --tile-dim (=512)        | The dimension of the generated tiles |
| -w, --input-width (=0)   | Set the width of the input image. If neither --input-width or --input-dim is set or --input-width=0, the program will attempt to derive the width from the input image. |
| -h, --input-height (=0)  | Set the height of the input image. If neither --input-height or --input-dim is set or --input-height=0, the program will attempt to derive the height from the input image. |
| --input-dim arg          | Set the dimension of the input image: overrides --input-width and --input-height. |
| -i, --input-file         | Input file in .svg format |
| -o, --output-dir         | Name of the target directory where output should be stored |



# Dependencies
List of program dependencies

## Libraries
- OpenMP
- Boost

## External programs
The tool depends on a few external programs to properly function. There are plans to remove these depedencies somewhere in the future.

- Inkscape 
- Convert _Currently required due to an issue with inkscape not properly exporting to .jpg_
