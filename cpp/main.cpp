/***************************************************************************************************
 *
 * main.cpp
 * @author Ben Witzen
 * @date Feb 27, 2017
 *
 * Part of my solution for Google's Hashcode 2017 challenge, submitted during the extended round.
 *
 * Entry point of program. Handles file I/O, sets up initial objects, and calls the computation.
 *
 * Compile with: make runner a=example
 * Run with ./run infile [passes] > outfile
 *
 **************************************************************************************************/

#include <iostream>
#include <stdio.h>

#include "youtube.h"
#include "algorithms/compute.h"

int main(int argc, char **argv) {
  using namespace std;
  
  // usage instructions
  if (argc < 3) {
    cerr << "Usage: " << argv[0] << " infile outfile [passes]" << endl;
    return 0;
  }

  // get amount of passes
  size_t passes = argc > 3 ? atoi(argv[3]) : 1;

  // notify of settings
  cerr << "Algorithm: " << ALGORITHM_NAME << endl;
  cerr << "Passes:    " << passes << endl;
  
  // open file
  FILE *in = fopen(argv[1], "r");
  if (!in) {
    cerr << "Cannot open " << argv[1] << endl;
    return 0;
  }
  
  // read infile, header
  size_t videos_amt, endpoints_amt, requests_amt, caches_amt, caches_size;
  fscanf(in, "%lu %lu %lu %lu %lu\n", &videos_amt, &endpoints_amt, &requests_amt, &caches_amt,
                                 &caches_size);

  // read infile, video sizes
  size_t requests_size[videos_amt];
  for (size_t i = 0; i < videos_amt; i++)
    fscanf(in, "%lu", &requests_size[i]);

  // create caches
  Cache *caches[caches_amt];
  for (size_t i = 0; i < caches_amt; i++)
    caches[i] = new Cache(caches_size);

  // read infile, create endpoints and link endpoints <-> caches
  Endpoint *endpoints[endpoints_amt];
  for (size_t endp = 0; endp < endpoints_amt; endp++) {
    size_t latency, caches_amt;
    fscanf(in, "%lu %lu\n", &latency, &caches_amt);
    endpoints[endp] = new Endpoint(latency);
    for (size_t i = 0; i < caches_amt; i++) {
      size_t id, latency;
      fscanf(in, "%lu %lu\n", &id, &latency);
      endpoints[endp]->add_cache(caches[id], latency);
    }
  }
  
  // read infile, create video requests and assign them to endpoints
  for (size_t i = 0; i < requests_amt; i++) {
    size_t request, endpoint, weight;
    fscanf(in, "%lu %lu %lu\n", &request, &endpoint, &weight);
    Request *n = new Request(request, requests_size[request], weight);
    endpoints[endpoint]->add_request(n);
  }
  
  fclose(in);
  cerr << "Infile has been read. Starting computation..." << endl;
  
  // invoke computation; call each endpoint once per pass
  float calls_amt = passes * endpoints_amt;
  for (size_t p = 0; p < passes; p++)
    for (size_t i = 0; i < endpoints_amt; i++)
      compute(endpoints[i], p);

  cerr << "Computation done, writing outfile." << endl;

  // write outfile
  FILE *out = fopen(argv[2], "w");
  fprintf(out, "%lu\n", caches_amt);
  for (size_t i = 0; i < caches_amt; i++)
    caches[i]->print_raw(out);
  fclose(out);
  
  // cleanup
  for (size_t i = 0; i < endpoints_amt; i++)
    delete endpoints[i];
  for (size_t i = 0; i < caches_amt; i++)
    delete caches[i];
}