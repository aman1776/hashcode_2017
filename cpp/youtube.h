/***************************************************************************************************
 *
 * youtube.h
 * @author Ben Witzen
 * @date Feb 27, 2017
 *
 * Part of my solution for Google's Hashcode 2017 challenge, submitted during the extended round.
 *
 * Defines the three objects that manage a computation.
 *
 * `Request`: At the start, this forms a request for a certain video by an endpoint. This means that
 * multiple Request objects for the same video ID can exist. Note that the terms request and video
 * are used interchangeably. This is due to the fact that Requests can be pushed into Caches, from
 * which point they start acting as a stored video on that Cache rather than a request.
 *
 * `Cache`: Caches offer an interface that allows for adding and removing videos.
 *
 * `Endpoint`: Endpoints consist of a queue of video requests and are connected to zero or more
 * Cache objects. They form the "heart" of a computation; this is the object that is directly inter-
 * acted with by the computation function. Endpoints can access the functionality of their Caches.
 * Note that if multiple Endpoints share a Cache, each of those Endpoints actually points to the
 * exact same Cache object in memory.
 *
 **************************************************************************************************/

#ifndef _YOUTUBE_H
#define _YOUTUBE_H

#include <cstddef>
#include <iostream>
#include <vector>
#include <stdio.h>

// forward declarations
class Request;
class Cache;
class Endpoint;

/**
 * Request class.
 */
class Request {
  private:
    static size_t instances;
    size_t id, video_id, video_size, weight;

  public:
    /**
     * Constructor.
     * @arg video_id Video ID of the request.
     * @arg video_size Size of the video being requested.
     * @arg weight Weight of the request.
     */
    Request(size_t video_id, size_t video_size, size_t weight);
    
    /**
     * Destructor.
     */
    ~Request(void);
    
    /**
     * Prints to stderr a human readable representation of the object. Useful for debugging.
     */
    void print(void);
    
    /**
     * @return Object ID.
     */
    size_t get_id(void);
    
    /**
     * @return Video ID of the request.
     */
    size_t get_video_id(void);
    
    /**
     * @return Size of the video being requested.
     */
    size_t get_video_size(void);
    
    /**
     * @return Weight of the request.
     */
    size_t get_weight(void);
    
    /**
     * @return Score of the request, based on the weight and size of the request.
     */
    float get_score(void);

    /**
     * Merge another video request with this one. Requires both objects to carry the same value for
     * the video_id field. The weight of both requests is added together. Once merged, it's not
     * possible to split the object to obtain the original objects.
     * @arg request The request to merge with, which is then deleted from memory.
     */
    void merge_with(Request *request);
};

/**
 * Cache class.
 */
class Cache {
  private:
    static size_t instances;
    size_t id, capacity, used;
    std::vector<Request *> videos;
  
  public:
    /**
     * Constructor.
     * @arg capacity Maximum amount of MB this cache can store.
     */
    Cache(size_t capacity);
    
    /**
     * Destructor.
     * Any Request objects stored within this Cache are also destroyed.
     */
    ~Cache(void);

    /**
     * Prints to stderr a human readable representation of the object. Useful for debugging.
     */
    void print(void);
    
    /**
     * @return Object ID.
     */
    size_t get_id(void);
    
    /**
     * @return Leftover capacity of this cache in MB.
     */
    size_t get_remaining_space(void);
    
    /**
     * Attempts moving a video from *endpoint to this Cache. Fails if this Cache doesn't have enough
     * space to accept the new video. On success, the *request object may become deallocated from
     * memory.
     * @arg endpoint The endpoint owning request.
     * @arg request The video data to push to this cache.
     * @return true on success, false on failure.
     */
    bool push_video(Endpoint *endpoint, Request *request);

    /**
     * Prints to stdout a representation of the object. This follows the submission format as was
     * provided by Google, and as such is not very human readable.
     * @arg f C-style FILE pointer to write into.
     */
    void print_raw(FILE *f);
};

/**
 * Endpoint class.
 */

class Endpoint {
  private:
    static size_t instances;
    size_t id, datacenter_latency;
    std::vector<Cache *> caches;
    std::vector<size_t> caches_latency;
    std::vector<Request *> requests;
  
  public:
    /**
     * Constructor.
     * @arg latency Latency in ms to datacenter.
     */
    Endpoint(size_t latency);
    
    /**
     * Destructor.
     * Any Request objects stored within this Endpoint are also destroyed.
     * Any Cache objects referenced by this Endpoint are left untouched.
     */
    ~Endpoint(void);
    
    /**
     * Prints to stderr a human readable representation of the object. Useful for debugging.
     */
    void print(void);

    /**
     * @return Object ID.
     */
    size_t get_id(void);

    /**
     * @return Latency in ms to datacenter.
     */
    size_t get_datacenter_latency(void);

    /**
     * Obtains a reference from the endpoint AND REMOVES IT FROM THE ENDPOINT.
     * @arg index Position of the request to remove.
     * @return Reference to the removed object, or NULL if no object removed.
     */
    Request *pull_request_by_index(size_t index);

    /**
     * Obtains a reference from the endpoint AND REMOVES IT FROM THE ENDPOINT.
     * @arg id Identifier of the request to remove.
     * @return Reference to the removed object, or NULL if no object removed.
     */
    Request *pull_request_by_id(size_t id);

    /**
     * Gets an array of pointers to all video requests lined up in this endpoint. The requests are
     * stored by their score; from highest score to lowest score.
     * Call delete[] on `requests` when it is no longer needed.
     * @arg requests Gets overwritten with an array of pointers to requests.
     * @return The length of the array pointed at by requests.
     */
    size_t get_stored_requests(Request ***requests);
    
    /**
     * Gets an array of pointers to all caches connected to this endpoint. The caches are sorted
     * by their latencies; from fastest to slowest cache.
     * Call delete[] on `caches` when it is no longer needed.
     * @arg caches Gets overwritten with an array of pointers to caches.
     * @return The length of the array pointed at by caches.
     */
    size_t get_connected_caches(Cache ***caches);
    
    /**
     * Gets an array of pointers to all cache latencies. The order of latencies matches the order
     * of caches returned by get_connected_caches(); from fastes to slowest cache.
     * Call delete[] on `latencies` when it is no longer needed.
     * @arg latencies Gets overwritten with an array of integers.
     * @return The length of the array pointed at by latencies.
     */
    size_t get_caches_latencies(size_t **latencies);

    /**
     * (Used during initialization only.) Pushes a cache reference to this endpoint, implying this
     * endpoint has a connection to said cache.
     * @arg ref Reference to cache object.
     * @arg latency Latency in ms to this cache.
     */
    void add_cache(Cache *ref, size_t latency);

    /**
     * (Used during initialization only.) Pushes a request reference to this endpoint. Note that the
     * datasets provided apparently have multiple seperate lines for video requests that request the
     * same video from the same endpoint. This function will fuse and purge the excess objects
     * accordingly.
     * @arg video Reference to the request object.
     */
    void add_request(Request *request);
};

#endif // _YOUTUBE_H
