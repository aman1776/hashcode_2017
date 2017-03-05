//
// less.cpp
//   requires   ~60 passes for me_at_the_zoo.in
//   requires  ~750 passes for kittens.in
//   requires ~1650 passes for videos_worth_sharing.in
// 
// This is a simple extension to the greedy compute function; endpoints that are connected to a few
// caches are given more passes (to place videos) than endpoints that are connected to a lot of
// caches.
//

#include "compute.h"

// set to 200 for kittens.in, and 1 for the others
const size_t minimum_caches = 1;
// amount of passes before threshold increases; increasing this also increases the passes needed
const size_t granularity = 3;

void compute(Endpoint *e, size_t pass) {
  // get caches
  Cache **caches;
  size_t caches_count = e->get_connected_caches(&caches);
  
  // get requests
  Request **requests;
  size_t requests_count = e->get_stored_requests(&requests);
  
  // push one video (the highest scoring one) to one cache  
  if (requests_count > 0 && caches_count <= pass / granularity + minimum_caches) {
    size_t c = 0;
    while (c < caches_count)
      if (caches[c++]->push_video(e, requests[0]))
        break;
  }
  
  // cleanup
  if (caches_count > 0)
    delete[] caches;
  if (requests_count > 0)
    delete[] requests;
  return;
}