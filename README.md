<h1 align="center">
  <img src="package/app-info/icons/128x128/dev.levz.ImageFinder.png" alt="Image Finder Icon" width="128" height="128"/>
  <br>
  Tiny Image Finder
</h1>

<p align="center"><strong>Find all images in selected path and show its previews</strong></p>

<p align="center">
  <img src="https://levz.dev/image/image-finder-1.png" alt="Main View" width="400"/>
</p>
<p><br></p>

This app recursively find and filter by name images in selected path and show its previews.

## Rationale

Application developed at one day for self educating purpose to get more familiar with QML and cmake. Also I found no fast and convenient tools on Windows, Linux and macOS to filter and browse huge quantity of image previews - the thing I do often. This app is doing just this. Search algorithm is high level and straightforward, multi threaded and well scaling. The code is open source.

## Install

Get compiled standalone installers at https://levz.dev/image-finder.

## Log Output

Enable log output:

    export QT_LOGGING_RULES="dev.levz.imagefinder=true"
    
## License

Image Finder available under [GNU General Public License version 3](https://www.gnu.org/licenses/gpl-3.0.txt).

## Build and Run

### Required

* Qt 5.15.2

### Build

Run this commands inside *Tiny Image Finder* source folder:

    cmake -DCMAKE_PREFIX_PATH=~/Qt/5.15.2/gcc_64 -S . -B build
    cmake --build build

### Run

Run this command inside *Tiny Image Finder* source folder:

    ./build/image-finder

## Known Issues

* macOS application package built with Qt 6 and installed on macOS 10.14 call native folder selection dialog that unable to select folder and package built with Qt 5 have shared library loading issue on macOS 10.14, therefore minimal supported version for macOS package set to 10.15.
