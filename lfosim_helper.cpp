#include "lfosim_helper.hpp"


void *get_opt_labels(void* gol_args) {
    train_lightgbm_args* args = (train_lightgbm_args *) gol_args;
    std::vector<trEntry> opt_trace;
    uint64_t totalUniqC = parseTraceFile(opt_trace, args->traces);
    uint64_t totalReqc = opt_trace.size();

    // not sure what these do. The paper doesn't talk about this in the 
    // algorithm
    uint64_t maxEjectSize = totalReqc - totalUniqC;
    uint64_t solverPar = 4;
    //-------------

    std::vector<double> utilSteps2;
    for(auto & it: opt_trace) {
        if(it.size > args->cache_size) {
            it.hasNext = false;
        }
        if(it.hasNext) {
            assert(it.utility>=0);
            utilSteps2.push_back(it.utility);
        }
    }

    std::sort(utilSteps2.begin(), utilSteps2.end(),std::greater<double>());

    std::vector<double> utilSteps;
    utilSteps.push_back(1); // max util as start
    uint64_t curEjectSize = 0;
    // LOG("ejSize",maxEjectSize,trace.size(),utilities.size());
    assert(maxEjectSize>0);
    for(auto & it: utilSteps2) {
        curEjectSize++;
        if(curEjectSize >= maxEjectSize/2 && (it != *(--(utilSteps.end())) ) ) {
            utilSteps.push_back(it);
            //DEBUG
            LOG("utilStep",it,0,curEjectSize);
            curEjectSize = 0;
        }
    }
    utilSteps.push_back(0); // min util as end
    utilSteps2.clear();
    utilSteps2.shrink_to_fit();

    long double curCost=0, curHits, overallHits;
    uint64_t integerHits = 0;
    size_t effectiveEjectSize=0;
    
    // LNS iteration steps
    for(size_t k=0; k+2<utilSteps.size(); k++) {

        // set step's util boundaries
        const double minUtil = utilSteps[k+2];
        const double maxUtil = utilSteps[k];

        std::cerr << "k1. " << k << " lU " << minUtil << " uU " << maxUtil
                  << " cC " << curCost << " cH " << curHits << " cR " << effectiveEjectSize
                  << " oH " << overallHits << " oR " << totalReqc  << " iH " << integerHits << std::endl;


        // create MCF digraph with arc utilities in [minUtil,maxUtil]
        SmartDigraph g; // mcf graph
        SmartDigraph::ArcMap<int64_t> cap(g); // mcf capacities
        SmartDigraph::ArcMap<double> cost(g); // mcf costs
        SmartDigraph::NodeMap<int64_t> supplies(g); // mcf demands/supplies
        effectiveEjectSize = createMCF(g, opt_trace, args->cache_size, 
                                       cap, cost, supplies, minUtil, maxUtil);

        std::cerr << "k2. " << k << " lU " << minUtil << " uU " << maxUtil
                  << " cC " << curCost << " cH " << curHits << " cR " << effectiveEjectSize
                  << " oH " << overallHits << " oR " << totalReqc  << " iH " << integerHits << std::endl;


        // solve this MCF
        SmartDigraph::ArcMap<uint64_t> flow(g);
        curCost = solveMCF(g, cap, cost, supplies, flow, solverPar);

        std::cerr << "k3. " << k << " lU " << minUtil << " uU " << maxUtil
                  << " cC " << curCost << " cH " << curHits << " cR " << effectiveEjectSize
                  << " oH " << overallHits << " oR " << totalReqc  << " iH " << integerHits << std::endl;


        // write DVAR to trace
        curHits = 0;
        overallHits = 0;
        integerHits = 0;
        for(uint64_t i=0; i<opt_trace.size(); i++) {
            if(opt_trace[i].active) {
                opt_trace[i].dvar = 1.0L - flow[g.arcFromId(opt_trace[i].arcId)]/static_cast<long double>(opt_trace[i].size);
                opt_trace[opt_trace[i].nextSeen].hit = opt_trace[i].dvar;
                curHits += opt_trace[i].dvar;
            }
            LOG("dv",i,opt_trace[i].dvar,opt_trace[i].size);
            assert(opt_trace[i].dvar >= 0 && opt_trace[i].dvar<=1);
            overallHits += opt_trace[i].dvar;
            if(opt_trace[i].dvar > 0.99) {
                integerHits++;
            }
        }
    }


    pthread_exit(NULL);
}

void *get_features(void* gf_args) {
    train_lightgbm_args* args = (train_lightgbm_args *) gf_args;

    LFOTrainUtil lfoTrainUtil(args->traces, args->cache_type, args->cache_size);
    auto features = lfoTrainUtil.getFeatureVectors();
    args->features = features;  // This is where I'm storing the features.

    pthread_exit(NULL);
}

void *run_model(void* rm_args) {
    
}

void *train_lightgbm(void *tl_args) {

    train_lightgbm_args* args = (train_lightgbm_args *) tl_args; 
    // If there are no traces from the previous run, then we can exit. 
    if (args->traces.empty()) {
        pthread_exit(NULL);
    }

    pthread_t train_threads[TRAIN_THREADS];
    pthread_create(&train_threads[0], NULL, get_opt_labels, tl_args);
    pthread_create(&train_threads[1], NULL, get_features, tl_args);

    pthread_join(train_threads[0], NULL);
    pthread_join(train_threads[1], NULL);

    // train the light gbm now. 

    
    pthread_exit(NULL);
    
}

std::vector<SimpleRequest> get_traces(std::ifstream & infile, 
                                      size_t count_per_epoch) {
    int counter = 1;
    long long t, id, size;
    std::vector<SimpleRequest> requestList;
    while(infile >> t >> id >> size && counter++ <= count_per_epoch)
    {
        SimpleRequest req = SimpleRequest(id, size, t);
        requestList.push_back(req);
    }
    return requestList;
}

void run_lfo_sim(const char* path, const std::string cache_type, const uint64_t cache_size) {  
    pthread_t threads[MAIN_THREADS];
    size_t count_per_epoch = 100;
    size_t epoch = 0;
    std::vector<SimpleRequest> traces, prev_traces;

    std::ifstream fstream;

    fstream.open(path);

    while(true) {
        std::vector<SimpleRequest> traces = get_traces(fstream, count_per_epoch);

        if (traces.empty()) {
            break;
        }

        // create the args for the various thread calls. 
        run_model_args rm_args(traces, epoch);
        train_lightgbm_args tl_args(cache_type, cache_size);
        if (!prev_traces.empty()) {
            tl_args.set_traces(prev_traces);
        }
        
        pthread_create(&threads[0], NULL, run_model, &rm_args);         // This is going to the LightGBM Model
        pthread_create(&threads[1], NULL, train_lightgbm, &tl_args);    // This is going to train the new one.  
        
        // join all the threads. 
        for (size_t i = 0; i < MAIN_THREADS; i++) {
            pthread_join(threads[i], NULL);
        }

        cout << tl_args.features.size() << std::endl;

        // To replace the old lightgbm model with the new one. 
        //TODO: code here..

        prev_traces = traces;
    }
    fstream.close();
}