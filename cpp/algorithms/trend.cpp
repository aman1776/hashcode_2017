//
// trend.cpp
//   requires 16 passes
//
// Optimized solution for the `trending_today.in` dataset. This dataset is special, since all end-
// points are connected to all caches. Furthermore, the delay between the datacenter and each end-
// point is always 600ms and the delay between each endpoint and each cache is always 100ms. This
// means the problem is simplified; distribute as many of the videos in the caches and ensure that
// each video only appears in exactly one cache. Because all of the caches have the same size, the
// simplest way to achieve this is by assigning cache space based on the size of videos, giving
// preference to videos that have a size that is also a divisor of the size of one cache, and once
// that is done, squeeze in the remaining videos. The biggest divisor is 1000 as that is the lar-
// gest video size in the dataset.
//
// This solution fits 9996 of the 10000 videos in the caches. A better solution is possible, but
// would require further manual engineering, which is outside the scope of this challenge. And it
// should be noted that a simple greedy algorithm would achieve results close to this "optimized"
// solution with just one pass.
//
// This solution scored 499.959 points (out of a possible 500.000).
//

#include "compute.h"

// once a video id has been cached once, it doesn't need to be cached again
bool already_cached[10000];

void compute(Endpoint *e, size_t pass) {
  // list of divisors
  const size_t divisors[] = {1000, 625, 500, 400, 250, 200, 125, 100, 80, 50, 40, 25, 20, 16, 10};
  
  // get caches
  Cache **caches;
  size_t caches_count = e->get_connected_caches(&caches);
  
  // get requests
  Request **requests;
  size_t requests_count = e->get_stored_requests(&requests);

  // pass 0-15: perform caching of videos with size divisible by divisors[] array
  if (pass < sizeof divisors / sizeof(size_t)) {
    size_t divisor = divisors[pass];
    
    for (size_t req = 0; req < requests_count; req++) {
      Request *v = requests[req];
      
      // reject videos that were previously cached or do not match divisor requirement
      if (already_cached[v->get_video_id()] || v->get_video_size() % divisor != 0)
        continue;
      
      // seek the next cache that has space for this video
      for (size_t c = 0; c < caches_count; c++) {
        if (caches[c]->push_video(e, v)) {
        
          // record the succesful caching of this video
          already_cached[v->get_video_id()] = true;
          break;
        }
      }
    }
  } // end of passes 0-15
  
  // pass 16: for the remaining videos requests, just try to place them, in order of appearance
  else {
    // "reversing" this for loop happens to fit +1 video!
    for (int v = requests_count - 1; v >= 0; v--) {
      // reject already cached videos
      if (already_cached[requests[v]->get_video_id()])
        continue;
      
      // seek next cache that has space for this video
      for (size_t c = 0; c < caches_count; c++)
        if (caches[c]->push_video(e, requests[v])) {
          already_cached[requests[v]->get_video_id()] = true;
          break;
        }
    }
  } // end of pass 16

  // cleanup
  if (caches_count > 0)
    delete[] caches;
  if (requests_count > 0)
    delete[] requests;
  return; 
}
