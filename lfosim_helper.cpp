#include "lfosim_helper.hpp"


void *get_opt_labels(void* gol_args) {
    cout << "[get_opt_labels] Working in thread# " << pthread_self() << std::endl;
    train_lightgbm_args* args = (train_lightgbm_args *) gol_args;
    std::vector<trEntry> opt_trace;
    uint64_t byte_sum = 0;
    parseTraceFile(opt_trace, args->traces, byte_sum);
    cacheAlg(opt_trace);
    printRes(opt_trace, byte_sum, args->cache_size);

    pthread_exit(NULL);
}

void *get_features(void* gf_args) {
    cout << "[get_features] Working in thread# " << pthread_self() << std::endl;
    train_lightgbm_args* args = (train_lightgbm_args *) gf_args;

    cout << args->cache_type << std::endl;

    LFOTrainUtil lfoTrainUtil(args->traces, args->cache_type, args->cache_size);
    auto features = lfoTrainUtil.getFeatureVectors();
    args->features = features;  // This is where I'm storing the features.

    pthread_exit(NULL);
}

void *run_model(void* rm_args) {
    
}

void *train_lightgbm(void *tl_args) {

    train_lightgbm_args* args = (train_lightgbm_args *) tl_args; 
    
    cout << "[train_lightgbm] Working in thread# " << pthread_self() << std::endl;



    // If there are no traces from the previous run, then we can exit. 
    if (args->traces.empty()) {
        pthread_exit(NULL);
    }

    pthread_t train_threads[TRAIN_THREADS];
    pthread_create(&train_threads[0], NULL, get_opt_labels, tl_args);
    pthread_create(&train_threads[1], NULL, get_features, tl_args);

    pthread_join(train_threads[0], NULL);
    pthread_join(train_threads[1], NULL);

    std::cout << args->features.size() << std::endl;

    // train the light gbm now. 

    
    pthread_exit(NULL);
    
}

std::vector<SimpleRequest> get_traces(const char* path, size_t count_per_epoch) {
    int counter = 1;
    long long t, id, size;
    std::ifstream infile;
    infile.open(path);
    std::vector<SimpleRequest> requestList;
    while(infile >> t >> id >> size)
    {
        if (counter == count_per_epoch && ++counter) break;
        SimpleRequest req = SimpleRequest(id, size, t);
        requestList.push_back(req);
    }
    infile.close();

    return requestList;
}

void run_lfo_sim(const char* path, const std::string cache_type, const uint64_t cache_size) {  
    pthread_t threads[MAIN_THREADS];
    size_t count_per_epoch = 100;
    size_t epoch = 0;
    std::vector<SimpleRequest> traces, prev_traces;

    while(true) {
        std::vector<SimpleRequest> traces = get_traces(path, count_per_epoch);

        // create the args for the various thread calls. 
        run_model_args rm_args(traces, epoch);
        train_lightgbm_args tl_args(cache_type, cache_size);
        if (!prev_traces.empty()) {
            tl_args.set_traces(prev_traces);
        }

        cout << tl_args.traces.size() << std::endl;
        cout << tl_args.cache_size << std::endl;
        cout << tl_args.cache_type << std::endl;
        
        pthread_create(&threads[0], NULL, run_model, &rm_args);         // This is going to the LightGBM Model
        pthread_create(&threads[1], NULL, train_lightgbm, &tl_args);    // This is going to train the new one.  
        
        // join all the threads. 
        for (size_t i = 0; i < MAIN_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }

        // To replace the old lightgbm model with the new one. 
        //TODO: code here..

        prev_traces = traces;
    }
}