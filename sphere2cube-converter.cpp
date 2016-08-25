
/* Standard Includes */
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <thread>

/* Custom Includes */
#include "sampler.h"
#include "stb_image.h"
#include "stb_image_write.h"
#include "sphere2cube-converter.h"

/* Standard Namespace */
using namespace std;

Sphere2CubeConverter::Sphere2CubeConverter(string source1, string source2, vector<string> dest, int tex,
					       int out_w, int out_h, int numSamples, bool type, bool &valid)
{
  /* Read in RGB Data */
  rgb_data1 = stbi_load(source1.c_str(), &input_width, &input_height, &input_bpp, 3);
  if(tex != 0) rgb_data2 = stbi_load(source2.c_str(), &input_width, &input_height, &input_bpp, 3);
  valid = true; if(rgb_data1 == NULL || (rgb_data2 == NULL && tex != 0)) {valid = false;}
  
  /* Read in Fields */
  output_height = out_h;
  output_width = out_w;
  samplingType = type;
  outputFiles = dest;  
  N = numSamples;
  texture = tex;
  
  /* Allocate Space for Output Data */
  for(int i = 0; i < 6; i++) {
    out_data[i] = (byte *) malloc(out_w * out_h * 3);
  }
}

Sphere2CubeConverter::~Sphere2CubeConverter() 
{
  stbi_image_free(rgb_data1);
  if(texture != 0) stbi_image_free(rgb_data2);
  for(int i = 0; i < 6; i++) free(out_data[i]);
}

void Sphere2CubeConverter::generateRay(Point3D &p_out, Point2D p_in, int facenum) 
{
  double i = (2 * p_in.x)/output_width - 1;
  double j = (2 * p_in.y)/output_height - 1;
  if (facenum == 0) {
    /* Left Face */
    p_out.z = -1;
    p_out.x = i;
    p_out.y = j;
  } else if (facenum == 1) {
    /* Right Face */
    p_out.z = 1;
    p_out.x = -i;
    p_out.y = j;
  } else if (facenum == 2) {
    /* Back Face */
    p_out.x = -1;
    p_out.z = -i;
    p_out.y = j;
  } else if(facenum == 3) {
    /* Front Face */
    p_out.x = 1;
    p_out.z = i;
    p_out.y = j;
  } else if(facenum == 4) {
    /* Floor Face */
    p_out.y = 1;
    p_out.x = i;
    p_out.z = j;
  } else {
    /* Ceiling Face */
    p_out.y = -1;
    p_out.x = i;
    p_out.z = -j;
  }
  double mag = sqrt((p_out.x * p_out.x) + (p_out.y * p_out.y) + (p_out.z * p_out.z));
  p_out.x = p_out.x/mag; p_out.y = p_out.y/mag; p_out.z = p_out.z/mag;
}

void Sphere2CubeConverter::getPointSpherical(int &i, int &j, Point3D ray) 
{
  double phi = asin(ray.y);
  double theta = atan2(ray.z, ray.x);
  double x = theta/M_PI;
  double y = (2*phi)/M_PI;
  i = (int) (((x + 1.0) * input_width)/2);
  j = (int) (((y + 1.0) * input_height)/2);
}

void Sphere2CubeConverter::getPointOctahedral(int &i, int &j, bool &pos, Point3D ray) 
{
  double xp, yp, zp;
  xp = ray.x / (abs(ray.x) + abs(ray.y) + abs(ray.z));
  yp = ray.y / (abs(ray.x) + abs(ray.y) + abs(ray.z));
  zp = ray.z / (abs(ray.x) + abs(ray.y) + abs(ray.z));
  double x, z;
  if(ray.y >= 0) {
    x = xp - zp;
    z = xp + zp;
    pos = true;
  } else {
    x = zp - xp;
    z = zp + xp;
    pos = false;
  }
  i = (int) (((x + 1.0) * input_width)/2);
  j = (int) (((z + 1.0) * input_height)/2);
}

void Sphere2CubeConverter::getPointParabolic(int &i, int &j, bool &pos, Point3D ray) 
{
  double x, y;
  if(ray.z < 0) {
    x = ray.x / (1 - ray.z);
    y = ray.y / (1 - ray.z);
    pos = false;
  } else {
    x = ray.x / (1 + ray.z);
    y = ray.y / (1 + ray.z);
    pos = true;
  }
  i = (int) (((x + 1.0) * input_width)/2);
  j = (int) (((y + 1.0) * input_height)/2);
}

void Sphere2CubeConverter::readVals(byte *ptr, int &i, int &j, byte &r, byte &g, byte &b) 
{
  if(i == input_width) i = input_width-1;
  if(j == input_height) j = input_height-1;
  size_t offset = 3*(i + (j*input_width));
  r = *(ptr + offset);
  g = *(ptr + offset + 1);
  b = *(ptr + offset + 2);
}

void Sphere2CubeConverter::getRgbFromPoint(int facenum, Point2D p_in, byte &r, byte &g, byte &b) 
{
  Point3D ray; int i, j; bool pos; 
  generateRay(ray, p_in, facenum);
  if(texture == 0) getPointSpherical(i, j, ray);
  if(texture == 1) getPointParabolic(i, j, pos, ray);
  if(texture == 2) getPointOctahedral(i, j, pos, ray);
  if(pos || texture == 0) readVals(rgb_data1, i, j, r, g, b);
  else readVals(rgb_data2, i, j, r, g, b); 
}

void Sphere2CubeConverter::populatePixel(int facenum, int i, int j, byte &r, byte &g, byte &b)
{
  Sampler s(N); vector<Point2D> points(N);
  s.getSamples(points, i, j, samplingType);
  double sumR = 0, sumG = 0, sumB = 0;
  for(int k = 0; k < N; k++) {
    getRgbFromPoint(facenum, points[k], r, g, b);
    sumR += (r/255.0); sumG += (g/255.0); sumB += (b/255.0);
  }
  r = (byte) (255.0*(sumR/N));
  g = (byte) (255.0*(sumG/N));
  b = (byte) (255.0*(sumB/N));
}

void Sphere2CubeConverter::convert()
{
  /* Initialize */
  byte r, g, b; size_t offset; 
  /* Iterate over all output pixels */
  for(int k = 0; k < 6; k++) {
    for(int j = 0; j < output_height; j++) {
      for(int i = 0; i < output_width; i++) {
	populatePixel(k, i, j, r, g, b);
	offset = 3*(i + (j * output_width));
	*(out_data[k] + offset + 0) = r;
	*(out_data[k] + offset + 1) = g;
	*(out_data[k] + offset + 2) = b;
      }
    }
  }
  /* Finally, write to the files */
  for(int i = 0; i < 6; i++) {
    stbi_write_png(outputFiles[i].c_str(), output_width, output_height, 3, out_data[i], output_width*3);
  }
}
