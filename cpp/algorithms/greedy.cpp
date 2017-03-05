//
// greedy.cpp
//   requires  ~20 passes for me_at_the_zoo.in
//   requires ~250 passes for kittens.in
//   requires ~550 passes for videos_worth_sharing.in
//
// A very simple compute function. Each endpoint gets to place one video of their choice in one
// cache of their choice. The choice is always "greedy" -- they always choose to place their highest
// scoring video in the fastest available cache. Each endpoint ignores the needs of the other end-
// points completely.
//

#include "compute.h"

void compute(Endpoint *e, size_t pass) {
  // get caches
  Cache **caches;
  size_t caches_count = e->get_connected_caches(&caches);
  
  // get requests
  Request **requests;
  size_t requests_count = e->get_stored_requests(&requests);
  
  // push one video (the highest scoring one) to one cache  
  if (requests_count > 0) {
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