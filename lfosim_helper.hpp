#include <iostream>
#include <vector>
#include <fstream>
#include <cassert>
#include <iomanip>
#include "request.h"
#include "caches/LFO/LFOTrainUtil.h"
#include "caches/OPT-U/parse_trace.h"
#include "caches/OPT-U/solve_mcf.h"

#define MAIN_THREADS 2
#define TRAIN_THREADS 2

struct run_model_args {
    std::vector<SimpleRequest> traces;
    size_t epoch;

    run_model_args(std::vector<SimpleRequest> t, size_t e): traces(t), epoch(e) {};
};

struct train_lightgbm_args {
    std::vector<SimpleRequest> traces;
    std::string cache_type;
    uint64_t cache_size;
    vector<vector<uint64_t>> features;

    train_lightgbm_args(): traces(), cache_type("LRU"), cache_size(1000), features() {}
    train_lightgbm_args(std::string ct, uint64_t cs): cache_type(ct), cache_size(cs) {} 
    void set_traces(std::vector<SimpleRequest> t) {this->traces = t;}
};

void *get_opt_labels(void* args);
void *get_features(void* args);
void *run_model(void* traces);
void *train_lightgbm(void *arg);
std::vector<SimpleRequest> get_traces(const char* path);
void run_lfo_sim(const char* path, const std::string cache_type, const uint64_t cache_size);