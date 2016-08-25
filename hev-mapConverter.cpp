
/* Standard Includes */
#include <cstdbool>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>

/* Custom Include */
#include "cube2sphere-converter.h"
#include "sphere2cube-converter.h"

/* Standard Namespace */
using namespace std;

/* Notes:
 * 1. If converting to a cube map, to a paraboloidal map, or to an octahedral map, the output
 *    height will be set equal to the output width. If converting to a spherical map, the 
 *    output height will be set to (9*width)/16 in order preserve a 16:9 width-to-height ratio.
 * 
 * 2. If the conversion is to or from a dual paraboloid or octahedral map, then the final 
 *    argument (outfile2) is necessary. If converting to or from a spherical map, then
 *    the the final argument should be omitted. 
 */
 
void printUsage() {
  cout << "Error. Correct Usage: ./hev-mapConverter ['convToCube' or 'convFromCube'] "; 
  cout << "['equirectangular' or 'paraboloid' or 'octahedral'] [# of pixel samples] ";
  cout << "['uniform' or 'jittered'] [cube-left.png] [cube-right.png] [cube-back.png] ";
  cout << "[cube-front.png] [cube-floor.png] [cube-ceiling.png] [output width] ";
  cout << "[projection-file-1.png] [(if necessary) projection-file-2.png]" << endl;
  exit(1);
}

int main(int argc, char** argv) 
{  
  /* Arugments Check I*/
  if(argc != 13 && argc != 14) printUsage();
  
  /* Type of Conversion */
  bool toCube;
  if(strcmp(argv[1], "convToCube") == 0) toCube = true;
  if(strcmp(argv[1], "convFromCube") == 0) toCube = false;
  
  /* Type of Projection */
  int projectionType;
  if(strcmp(argv[2], "equirectangular") == 0) projectionType = 0;
  else if(strcmp(argv[2], "paraboloid") == 0) projectionType = 1;
  else if(strcmp(argv[2], "octahedral") == 0) projectionType = 2;
  else printUsage();
  
  /* Arguments Check II*/
  if((projectionType != 0) && (argc == 13)) printUsage();
  
  /* Number of Samples */
  char *buf1; int numSamples = (int) strtol(argv[3], &buf1, 10);
  if((strlen(argv[3]) == 0) || (strlen(buf1) != 0)) printUsage();
  
  /* Sampling Method */
  bool useJittered;
  if(strcmp(argv[4], "uniform") == 0) useJittered = false;
  else if(strcmp(argv[4], "jittered") == 0) useJittered = true;
  else printUsage();
  
  /* Cube Map Filenames */
  vector<string> cubeMap(6);
  for(int i = 0; i < 6; i++){
    cubeMap[i] = string(argv[i+5]);
  }

  /* Output Dimensions */
  char *buf2; int yDim, xDim = (int) strtol(argv[11], &buf2, 10);
  if((strlen(argv[11]) == 0) || (strlen(buf2) != 0)) printUsage();  
  if((!toCube) && (projectionType == 0)) yDim = (9*(xDim))/16;
  else yDim = xDim; /* Image Must be Square so xDim == yDim */
  
  /* Output Files */
  string outFile2, outFile1 = string(argv[12]);
  if(projectionType != 0) outFile2 = string(argv[13]);
  else outFile2 = string(argv[12]); /* Irrelevant */
  
  /* Conversion */
  bool valid;
  if(!toCube) {
    Cube2SphereConverter converter(cubeMap, numSamples, useJittered, valid);
    if(valid) converter.convert(outFile1, outFile2, xDim, yDim, projectionType);
  } else {
    Sphere2CubeConverter converter(outFile1, outFile2, cubeMap, projectionType,
				   xDim, yDim, numSamples, useJittered, valid);
    if(valid) converter.convert();
  }
  if(!valid){
    cout << "Conversion Failed: One or more source files couldn't be read." << endl;
  }
}
