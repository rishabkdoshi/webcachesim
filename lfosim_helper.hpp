#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <iomanip>
#include "request.h"
#include "caches/LFO/LFOTrainUtil.h"
#include "caches/OPTU/parse_trace.h"
#include "caches/OPTU/solve_mcf.h"
#include "cache.h"
#include "caches/lru_variants.h"
#include "caches/LFO/LFOCache.h"

#define MAIN_THREADS 2
#define TRAIN_THREADS 2

std::vector<SimpleRequest> get_traces(const char* path);
void run_lfo_sim(const char* path, const std::string cache_type, const uint64_t cache_size);