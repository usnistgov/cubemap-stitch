
#ifndef CUBE2SPHERE_CONVERTER_H_
#define CUBE2SPHERE_CONVERTER_H_

#include <vector>
#include <string>
#include <iostream>
#include <cstdbool>
#include <fstream>
#include "point.h"
#include "byte.h"

class Cube2SphereConverter
{
 public:
  /* Usage Notes:
   *   Dest Vector Order: [left] [right] [back] [front] [floor] [ceiling]
   *   outFile1: Positive Image | outFile2: Negative Image
   */
  Cube2SphereConverter(std::vector<std::string> source, int numSamples, bool useJitter, bool &valid);
  ~Cube2SphereConverter();
  void convert(std::string outFile1, std::string outFile2, int xDim, int yDim, int tex);
  /* Fields */
  int input_width, input_height, input_bpp;
  int output_width, output_height, N;
  byte* rgb_data[6]; bool samplingType;
  /* Helper Methods */
  void generateRaySpherical(Point3D &p_out, Point2D p_in);
  void generateRayParaboloid(Point3D &p_out, bool zpos, Point2D p_in);
  void generateRayOctahedron(Point3D &p_out, bool ypos, Point2D p_in);
  void readVals(byte *ptr, int &i, int &j, byte &r, byte &g, byte &b);
  void getRgbFromPoint(Point2D p_in, byte &r, byte &g, byte &b, bool pos, int tex);
  void populatePixel(int i, int j, byte &r, byte &g, byte &b, bool pos, int tex);
};

#endif
