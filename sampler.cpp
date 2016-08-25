
/* Standard Includes */
#include <iostream>
#include <cstdlib>
#include <cstdbool>
#include <cmath>
#include <ctime>

/* Header */
#include "sampler.h"

using namespace std;
 
Sampler::Sampler(int usr_N) {
  N = usr_N; 
  n = (int) sqrt(usr_N);
  srand48(time(NULL));
}

void Sampler::getSamples(vector<Point2D> &points, int pix_i, int pix_j, bool option) {
  if(!option) {
    /* Uniform Sampling Over Pixel */
    generateCanonicalArrangement(points, false);
    scalePoints(points, pix_i, pix_j);
  } else {
    /* Jittered Over Pixel */
    generateCanonicalArrangement(points, true);
    shuffleArrangement(points);
    scalePoints(points, pix_i, pix_j);
  }
}

void Sampler::generateCanonicalArrangement(vector<Point2D> &points, bool option) {
  /* Producing the Canonical Arrangement */
  for(int j = 0; j < n; j++) {
    for(int i = 0; i < n; i++) {
      if(!option) {
	/* Uniform sampling over the pixel */
	points[j*n+i].x = ((double) (i+1)) * (((double) 1)/((double) (n+1)));
	points[j*n+i].y = ((double) (j+1)) * (((double) 1)/((double) (n+1)));
      } else  {
	/* Jittered sampling over the pixel */
	points[j*n + i].x = (((double) i) + (((double) j) + drand48()) / n) / n;
	points[j*n + i].y = (((double) j) + (((double) i) + drand48()) / n) / n;
      }
    }
  }
}

void Sampler::shuffleArrangement(vector<Point2D> &points) {
  /* Shuffling the canonical arrangement via correlated multi-jittered sampling */
  for(int j = 0; j < n; j++) {
    int k = j + (drand48() * (n - j));
    for(int i = 0; i < n; i++) {
      std::swap(points[j*n + i].x, points[k*n + i].x);
    }
  }
  for(int i = 0; i < n; i++) {
    int k = i + (drand48() * (n - i));
    for(int j = 0; j < n; j++) {
      std::swap(points[j*n + i].y, points[j*n + k].y);
    }
  }
}

void Sampler::scalePoints(vector<Point2D> &points, int pix_i, int pix_j) {
  for(int i = 0; i < N; i++) {
    points[i].x += (double) pix_i;
    points[i].y += (double) pix_j;
  }
}
