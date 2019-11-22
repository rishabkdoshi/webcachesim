#include <fstream>
#include <string>
#include <regex>
#include "caches/LFO/LFOTrainUtil.h"
#include "caches/lru_variants.h"
#include "caches/gd_variants.h"
#include "request.h"
#include <pthread.h>
#include "lfosim_helper.hpp"

using namespace std;

int main (int argc, char* argv[])
{
    // output help if insufficient params
    // if(argc < 4) {
    //     cerr << "lfosim traceFile cacheType cacheSizeBytes [cacheParams]" << endl;
    //     return 1;
    // }

    // trace properties
    const char* path = "test.tr"; // argv[1];
    // create cache
    const string cache_type = "LRU"; // argv[2];
    // configure cache size
    const uint64_t cache_size  = 10000; //std::stoull(argv[3]);

    run_lfo_sim(path, cache_type, cache_size);
    
    return 0;
}
