
/* Standard Includes */
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <thread>

/* Custom Includes */
#include "sampler.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "cube2sphere-converter.h"

/* Standard Namespace */
using namespace std;

Cube2SphereConverter::Cube2SphereConverter(vector<string> source, int numSamples, bool useJitter, bool &valid)
{
  /* Read in RGB Data and Fields */
  samplingType = useJitter; N = numSamples;
  for(int i = 0; i < 6; i++){
    rgb_data[i] = stbi_load(source[i].c_str(), &input_width, &input_height, &input_bpp, 3);
  }
  /* Error Checking */
  valid = true;
  for(int i = 0; i < 6; i++) {
    if(rgb_data[i] == NULL) {
      valid = false;
    }
  }
}

Cube2SphereConverter::~Cube2SphereConverter() 
{
  for(int i = 0; i < 6; i++) stbi_image_free(rgb_data[i]);
}

void Cube2SphereConverter::generateRayOctahedron(Point3D &p_out, bool ypos, Point2D p_in) 
{
  double x = (2 * p_in.x)/output_width - 1;
  double z = (2 * p_in.y)/output_height - 1;
  if(ypos) {
    p_out.x = (z + x) / (2.0);
    p_out.z = (z - x) / (2.0);
    p_out.y = 1 - (abs(p_out.x) + abs(p_out.z));
  } else {
    p_out.x = (z - x) / (2.0);
    p_out.z = (z + x) / (2.0);
    p_out.y = (abs(p_out.x) + abs(p_out.z)) - 1;
  }
  double mag = sqrt((p_out.x * p_out.x) + (p_out.y * p_out.y) + (p_out.z * p_out.z));
  p_out.x = p_out.x/mag; p_out.y = p_out.y/mag; p_out.z = p_out.z/mag;
}

void Cube2SphereConverter::generateRayParaboloid(Point3D &p_out, bool zpos, Point2D p_in)
{
  p_out.x = p_in.x;
  p_out.y = p_in.y;
  if(zpos) {
    p_out.z = 0.5 - 0.5*((p_out.x * p_out.x) + (p_out.y * p_out.y));
  } else {
    p_out.z = -0.5 + 0.5*((p_out.x * p_out.x) + (p_out.y * p_out.y));
  }
  double mag = sqrt((p_out.x * p_out.x) + (p_out.y * p_out.y) + (p_out.z * p_out.z));
  p_out.x = p_out.x/mag; p_out.y = p_out.y/mag; p_out.z = p_out.z/mag;
}

void Cube2SphereConverter::generateRaySpherical(Point3D &p_out, Point2D p_in) 
{
  double x = (2 * p_in.x)/output_width - 1;
  double y = (2 * p_in.y)/output_height - 1;
  double theta = M_PI * x;
  double phi = (M_PI * y)/2;
  p_out.x = cos(phi) * cos(theta);
  p_out.y = sin(phi);
  p_out.z = cos(phi) * sin(theta);
}

void Cube2SphereConverter::readVals(byte *ptr, int &i, int &j, byte &r, byte &g, byte &b) 
{
  if(i == input_width) i = input_width-1;
  if(j == input_height) j = input_height-1;
  size_t offset = 3*(i + (j*input_width));
  r = *(ptr + offset);
  g = *(ptr + offset + 1);
  b = *(ptr + offset + 2);
}

void Cube2SphereConverter::getRgbFromPoint(Point2D p_in, byte &r, byte &g, byte &b, bool pos, int tex) 
{
  /* Texture Selection */
  Point3D p; int i, j, n;
  if(tex == 0) {
    generateRaySpherical(p, p_in);
  } else if(tex == 1) {
    p_in.x = (2 * p_in.x)/output_width - 1;
    p_in.y = (2 * p_in.y)/output_height - 1;
    double rad = sqrt((p_in.x * p_in.x) + (p_in.y * p_in.y));
    if(rad >= 1.01) {r = 255; g = 255; b = 255; return;}
    generateRayParaboloid(p, pos, p_in);
  } else {
    generateRayOctahedron(p, pos, p_in);
  }
  
  /* Projection Algorithm */
  double xMag = abs(p.x), yMag = abs(p.y), zMag = abs(p.z);
  if (xMag > yMag) {
    if (xMag > zMag) {    // x is the face
      if (p.x<0.0) { 
	n = 2;
	i = (int) (((-p.z/xMag) + 1.0)*input_width/2);
	j = (int) (((p.y/xMag) + 1.0)*input_height/2);
      } else { 
	n = 3;
	i = (int) (((p.z/xMag) + 1.0)*input_width/2);
        j = (int) (((p.y/xMag) + 1.0)*input_height/2);
      }
    } else {              // z is the face
      if (p.z<0.0) { 
	n = 0;
	i = (int) (((p.x/zMag) + 1.0)*input_width/2);
        j = (int) (((p.y/zMag) + 1.0)*input_height/2);
      } else {
	n = 1;
	i = (int) (((-p.x/zMag) + 1.0)*input_width/2);
        j = (int) (((p.y/zMag) + 1.0)*input_height/2);
      }
    }
  } else {
    if (yMag > zMag) {    // y is the face
      if (p.y<0.0) { 
	n = 5;
	i = (int) (((p.x/yMag) + 1.0)*input_width/2);
        j = (int) (((-p.z/yMag) + 1.0)*input_height/2);
      } else { 
	n = 4;
	i = (int) (((p.x/yMag) + 1.0)*input_width/2);
        j = (int) (((p.z/yMag) + 1.0)*input_height/2);
      }
    } else {              // z is the face
      if (p.z<0.0) { 
	n = 0;
	i = (int) (((p.x/zMag) + 1.0)*input_width/2);
        j = (int) (((p.y/zMag) + 1.0)*input_height/2);
      } else {
	n = 1;
	i = (int) (((-p.x/zMag) + 1.0)*input_width/2);
        j = (int) (((p.y/zMag) + 1.0)*input_height/2);
      }
    }
  }
  readVals(rgb_data[n], i, j, r, g, b);
}

void Cube2SphereConverter::populatePixel(int i, int j, byte &r, byte &g, byte &b, bool pos, int tex)
{
  Sampler s(N); vector<Point2D> points(N);
  s.getSamples(points, i, j, samplingType);
  double sumR = 0, sumG = 0, sumB = 0;
  for(int k = 0; k < N; k++) {
    getRgbFromPoint(points[k], r, g, b, pos, tex);
    sumR += (r/255.0); sumG += (g/255.0); sumB += (b/255.0);
  }
  r = (byte) (255.0*(sumR/N));
  g = (byte) (255.0*(sumG/N));
  b = (byte) (255.0*(sumB/N));
}

void Cube2SphereConverter::convert(string outFile1, string outFile2, int xDim, int yDim, int tex)
{
  /* Initialize */
  output_width = xDim; output_height = yDim; byte r, g, b; size_t offset;
  byte *out_data_neg, *out_data_pos = (byte *) malloc(output_width * output_height * 3);
  if(tex != 0) out_data_neg = (byte *) malloc(output_width * output_height * 3);
  
  /* Iterate over all output pixels */
  for(int j = 0; j < output_height; j++) {
    for(int i = 0; i < output_width; i++) {
      offset = 3*(i + (j * output_width));
      populatePixel(i, j, r, g, b, true, tex);
      *(out_data_pos + offset + 0) = r;
      *(out_data_pos + offset + 1) = g;
      *(out_data_pos + offset + 2) = b;
      if(tex != 0) {
	populatePixel(i, j, r, g, b, false, tex);
	*(out_data_neg + offset + 0) = r;
	*(out_data_neg + offset + 1) = g;
	*(out_data_neg + offset + 2) = b;
      }
    }
  }
  
  /* Finally, write to the file */
  stbi_write_png(outFile1.c_str(), output_width, output_height, 3, out_data_pos, output_width*3);
  if(tex != 0) stbi_write_png(outFile2.c_str(), output_width, output_height, 3, out_data_neg, output_width*3);
  free(out_data_pos); if(tex != 0) {free(out_data_neg);}
}

