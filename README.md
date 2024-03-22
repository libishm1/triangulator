# triangulator
The code loads camera calibration data from an XML file, scales camera matrices and distortion coefficients based on camera resolutions, constructs projection matrices, extracts first fifteen image points, converts them for triangulation, and then triangulates the points to obtain 3D coordinates using OpenCV's triangulatePoints function

Commandline arguments

complie;  

g++ -o triangulate triangulate.cpp `'pkg-config --cflags --libs opencv4'`

run executable;

./triangulate
