/***************************************************************************************************
 *
 * youtube.cpp
 * @author Ben Witzen
 * @date Feb 27, 2017
 *
 * Part of my solution for Google's Hashcode 2017 challenge, submitted during the extended round.
 *
 * Function descriptions can be found in youtube.h
 *
 **************************************************************************************************/

#include "youtube.h"

// Request class

size_t Request::instances = 0;

Request::Request(size_t video_id, size_t video_size, size_t weight) {
  this->id = instances++;
  this->video_id = video_id;
  this->video_size = video_size;
  this->weight = weight;
}

Request::~Request(void) {
}

void Request::print(void) {
  using namespace std;
  cerr << "REQUEST #" << id;
  cerr << " (video: " << video_id << " |";
  cerr << " size: " << video_size << " |";
  cerr << " weight: " << weight << " |";
  cerr << " score: " << this->get_score() << ")" << endl;
}

size_t Request::get_id(void) {
  return id;
}

size_t Request::get_video_id(void) {
  return video_id;
}

size_t Request::get_video_size(void) {
  return video_size;
}

size_t Request::get_weight(void) {
  return weight;
}

float Request::get_score(void) {
  return weight / (float) video_size;
}

void Request::merge_with(Request *request) {
  // sanity check
  if (video_id != request->get_video_id())
    return;
  
  // cumulate weight and delete absorbed object
  weight += request->get_weight();
  delete request;
}

// Cache class

size_t Cache::instances = 0;

Cache::Cache(size_t capacity) {
  this->id = instances++;
  this->capacity = capacity;
  this->used = 0;
}

Cache::~Cache(void) {
  for (size_t i = 0; i < videos.size(); i++)
    delete videos[i];
  videos.clear();
}

void Cache::print(void) {
  using namespace std;
  cerr << "CACHE #" << id;
  cerr << " (usage: " << used << "/" << capacity << " |";
  cerr << " videos: " << videos.size() << ")" << endl;
}

size_t Cache::get_id(void) {
  return id;
}

size_t Cache::get_remaining_space(void) {
  return capacity - used;
}

bool Cache::push_video(Endpoint *endpoint, Request *video) {
  // merge new video with old object, if it already occurs in this Cache
  for (size_t i = 0; i < videos.size(); i++) {
    if (video->get_video_id() == videos[i]->get_video_id()) {
      endpoint->pull_request_by_id(video->get_video_id());
      videos[i]->merge_with(video);
      return true;
    }
  }
  
  // check if remaining capacity allows for adding this video  
  if (used + video->get_video_size() > capacity)
    return false;
  
  // move video from endpoint to cache
  used += video->get_video_size();
  videos.push_back(video);
  endpoint->pull_request_by_id(video->get_video_id());

  return true;
}

void Cache::print_raw(FILE *f) {
  using namespace std;
  fprintf(f, "%lu", id);
  for (size_t v = 0; v < videos.size(); v++)
    fprintf(f, " %lu", videos[v]->get_video_id());
  fprintf(f, "\n");
}

// Endpoint class

size_t Endpoint::instances = 0;

Endpoint::Endpoint(size_t latency) {
  this->id = instances++;
  this->datacenter_latency = latency;
}

Endpoint::~Endpoint(void) {
  for (size_t i = 0; i < requests.size(); i++)
    delete requests[i];
  caches.clear();
  caches_latency.clear();
}

void Endpoint::print(void) {
  using namespace std;
  cerr << "ENDPOINT #" << id;
  cerr << " (latency: " << datacenter_latency;
  cerr << " caches: " << caches.size() << " |";
  cerr << " requests: " << requests.size() << ")" << endl;
}

size_t Endpoint::get_id(void) {
  return id;
}

size_t Endpoint::get_datacenter_latency(void) {
  return datacenter_latency;
}

Request *Endpoint::pull_request_by_index(size_t index) {
  if (index >= requests.size())
    return NULL;
  Request *tmp = requests[index];
  requests.erase(requests.begin() + index);
  return tmp;
}

Request *Endpoint::pull_request_by_id(size_t id) {
  for (size_t i = 0; i < requests.size(); i++)
    if (requests[i]->get_video_id() == id) {
      Request *tmp = requests[i];
      requests.erase(requests.begin() + i);
      return tmp;
    }
  return NULL;
}

size_t Endpoint::get_stored_requests(Request ***requests) {
  size_t requests_count = this->requests.size();
  
  // check if there are any requests
  if (requests_count == 0) {
    requests = NULL;
    return 0;
  }

  // copy over all pointers from vector
  Request **list = new Request *[requests_count];
  for (size_t i = 0; i < requests_count; i++)
    list[i] = this->requests[i];
  
  *requests = list;
  return requests_count;
}

size_t Endpoint::get_connected_caches(Cache ***caches) {
  size_t caches_count = this->caches.size();
  
  // check if there are any caches
  if (caches_count == 0) {
    caches = NULL;
    return 0;
  }

  // copy over all pointers from vector
  Cache **list = new Cache *[caches_count];
  for (size_t i = 0; i < caches_count; i++)
    list[i] = this->caches[i];
  
  *caches = list;
  return caches_count;
}

size_t Endpoint::get_caches_latencies(size_t **latencies) {
  size_t caches_count = this->caches.size();
  
  // check if there are any caches
  if (caches_count == 0) {
    latencies = NULL;
    return 0;
  }

  // copy over all pointers from vector
  size_t *list = new size_t[caches_count];
  for (size_t i = 0; i < caches_count; i++)
    list[i] = this->caches_latency[i];
  
  *latencies = list;
  return caches_count;
}

void Endpoint::add_cache(Cache *ref, size_t latency) {
  if (caches.empty()) {
    caches.push_back(ref);
    caches_latency.push_back(latency);
    return;
  }

  // keep caches ordered from fastest to slowest
  size_t slot = 0;
  for (; slot < caches.size(); slot++)
    if (caches_latency[slot] > latency)
      break;
  
  caches.insert(caches.begin() + slot, ref);
  caches_latency.insert(caches_latency.begin() + slot, latency);
}

void Endpoint::add_request(Request *request) {
  if (requests.empty()) {
    requests.push_back(request);
    return;
  }

  // check if request can be merged
  // if so, purge the old stored request and offer the merged request for sorting
  for (size_t i = 0; i < requests.size(); i++) {
    if (requests[i]->get_video_id() == request->get_video_id()) {
      request->merge_with(requests[i]);
      requests.erase(requests.begin() + i);
      break;
    }
  }
  
  // keep requests ordered from highest to lowest score
  size_t slot = 0;
  for (; slot < requests.size(); slot++)
    if (requests[slot]->get_score() <= request->get_score())
      break;
  
  requests.insert(requests.begin() + slot, request);
}

