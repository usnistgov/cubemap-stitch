
#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <vector>
#include "point.h"
#include "byte.h"

class Sampler
{
 public:
  
  /* Constructor: Sampler()
   * ----------------------
   * Constructor initializes the random number generator, and populates
   * one or two internal fields necessary for computation of samples for
   * a given pixel index. Destructor doesn't need to do anything. 
   */
  Sampler(int usr_N);
  
  /* Method: getSamples()
   * --------------------
   * Returns void. Fills a client-supplied vector of N points, each of 
   * which contains the (x, y) coordinate to some point on the pixel (i, j). 
   * Uses a uniform-distribution correlated multi-jittered sampling scheme.
   * Note:
   *   option == false -> Use uniform sampling over the pixel
   *   option == true -> Use MCJ sampling over the pixel
   */
  void getSamples(std::vector<Point2D> &points, int pix_i, int pix_j, bool option);
  
 private:

  /* Private Fields */
  int N; double n;
  /* Private Helper Methods */
  void generateCanonicalArrangement(std::vector<Point2D> &points, bool option);
  void shuffleArrangement(std::vector<Point2D> &points);
  void scalePoints(std::vector<Point2D> &points, int pix_i, int pix_j);
};

#endif
