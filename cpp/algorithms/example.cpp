/***************************************************************************************************
 *
 * example.cpp
 * @author Ben Witzen
 * @date Feb 27, 2017
 *
 * Part of my solution for Google's Hashcode 2017 challenge, submitted during the extended round.
 *
 * The program takes Endpoints as the central point of a computation. For each pass, all of the end-
 * points seperately call the compute() function below. The endpoint object contains each video
 * request for that endpoint and a reference to each Cache object it is connected with. Once a Cache
 * is full, no further videos can be stored within it until some videos are removed. Because Caches
 * are accessed by reference, multiple Endpoints can modify the same Cache object. This means that
 * changes made by one Endpoint during one pass to a Cache will carry over to all subsequent passes
 * and Endpoints.
 *
 * To create a new solution, copy this file and rename it.
 *
 * Compile your solution with: make runner a=example
 *   with `example` being the name of your source file, without .cpp
 *
 * Run your solution with ./run infile [passes] > outfile 
 *
 **************************************************************************************************/

#include "compute.h"

void compute(Endpoint *e, size_t pass) {
  if (pass == 0)
    e->print();
  return;
}
