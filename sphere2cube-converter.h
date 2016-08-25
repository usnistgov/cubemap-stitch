
#ifndef SPHERE2CUBE_CONVERTER_H_
#define SPHERE2CUBE_CONVERTER_H_

#include <vector>
#include <string>
#include <iostream>
#include <cstdbool>
#include <fstream>
#include "point.h"
#include "byte.h"

class Sphere2CubeConverter
{
 public:
  /* Usage Notes:
   *   source1: Positive Image | source2: Negative Image 
   *   Dest Vector Order: [left] [right] [back] [front] [floor] [ceiling] 
   */
  Sphere2CubeConverter(std::string source1, std::string source2, std::vector<std::string> dest, 
		       int tex, int out_w, int out_h, int numSamples, bool type, bool &valid);
  ~Sphere2CubeConverter();
  void convert();
  /* Fields */
  int input_width, input_height, input_bpp;
  int output_width, output_height, N;
  byte* rgb_data1;
  byte* rgb_data2;
  byte* out_data[6];
  bool samplingType; 
  int texture;
  std::string source1;
  std::string source2;
  std::vector<std::string> outputFiles;
  /* Helper Methods */
  void generateRay(Point3D &p_out, Point2D p_in, int facenum); 
  void getPointSpherical(int &i, int &j, Point3D ray);
  void getPointParabolic(int &i, int &j, bool &pos, Point3D ray);
  void getPointOctahedral(int &i, int &j, bool &pos, Point3D ray);
  void readVals(byte *ptr, int &i, int &j, byte &r, byte &g, byte &b);
  void getRgbFromPoint(int facenum, Point2D p_in, byte &r, byte &g, byte &b);
  void populatePixel(int facenum, int i, int j, byte &r, byte &g, byte &b);
};

#endif
