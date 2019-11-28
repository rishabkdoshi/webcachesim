#include "lfosim_helper.hpp"

std::vector<double> get_opt_decisions(std::vector<SimpleRequest> traces,
                                        uint64_t cache_size) {

    
    std::vector<trEntry> opt_trace;
    uint64_t totalUniqC = parseTraceFile(opt_trace, traces);
    uint64_t totalReqc = opt_trace.size();

//    cout << opt_trace.size() << std::endl;
//    cout << traces.size() << std::endl;
//    cout << totalUniqC << std::endl;
//    cout << totalReqc << std::endl;

    // not sure what these do. The paper doesn't talk about this in the 
    // algorithm
    uint64_t maxEjectSize = totalReqc - totalUniqC;
    uint64_t solverPar = 4;
    //-------------

    std::vector<double> utilSteps2;
    for(auto & it: opt_trace) {
        if(it.size > cache_size) {
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

        // std::cerr << "k1. " << k << " lU " << minUtil << " uU " << maxUtil
        //           << " cC " << curCost << " cH " << curHits << " cR " << effectiveEjectSize
        //           << " oH " << overallHits << " oR " << totalReqc  << " iH " << integerHits << std::endl;


        // create MCF digraph with arc utilities in [minUtil,maxUtil]
        SmartDigraph g; // mcf graph
        SmartDigraph::ArcMap<int64_t> cap(g); // mcf capacities
        SmartDigraph::ArcMap<double> cost(g); // mcf costs
        SmartDigraph::NodeMap<int64_t> supplies(g); // mcf demands/supplies
        effectiveEjectSize = createMCF(g, opt_trace, cache_size, 
                                       cap, cost, supplies, minUtil, maxUtil);

        // std::cerr << "k2. " << k << " lU " << minUtil << " uU " << maxUtil
        //           << " cC " << curCost << " cH " << curHits << " cR " << effectiveEjectSize
        //           << " oH " << overallHits << " oR " << totalReqc  << " iH " << integerHits << std::endl;


        // solve this MCF
        SmartDigraph::ArcMap<uint64_t> flow(g);
        curCost = solveMCF(g, cap, cost, supplies, flow, solverPar);

        // std::cerr << "k3. " << k << " lU " << minUtil << " uU " << maxUtil
        //           << " cC " << curCost << " cH " << curHits << " cR " << effectiveEjectSize
        //           << " oH " << overallHits << " oR " << totalReqc  << " iH " << integerHits << std::endl;


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

    std::vector<double> opt_decisions;
    for (auto trace : opt_trace) {
        opt_decisions.push_back(trace.dvar);
    }

    return opt_decisions;

}

void retrain_model(std::vector<uint64_t> opt_decisions,
                   std::vector<std::vector<uint64_t>> o_features) {
    cout << "Here we will retrain the model. " << std::endl;
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


int run_model(std::ifstream& fstream,
               size_t num_traces,
               std::vector<std::vector<double>> & prev_o_features,
               void* cache,
               LFOTrainUtil lfoTrainUtil,
               bool run_lfo,
               vector<SimpleRequest> & prev_traces) {
    
    uint64_t time, size, id;
    size_t hits = 0;
    uint64_t counter = 0;
    SimpleRequest* req = new SimpleRequest(0, 0);
    while (!fstream.eof() && fstream >> time >> id >> size && ++counter <= num_traces) {
        SimpleRequest sr = SimpleRequest(id, size, time);
        prev_traces.push_back(sr);
        req = &sr;
        vector<double> o_feature;
        if (!run_lfo) {
            LRUCache* lru_cache = (LRUCache *) cache;
            o_feature = lfoTrainUtil.getLFOFeature(*req, lru_cache->getFreeBytes()).getFeatureVector();
            if (lru_cache->lookup(req)) {
                hits++;
            } else {
                lru_cache->admit(req);
            }
        } else {
            LFOCache *lfo_cache = (LFOCache *) cache;
            auto lfoFeature = lfoTrainUtil.getLFOFeature(*req, lfo_cache->getFreeBytes());
            o_feature = lfoFeature.getFeatureVector();
            if (lfo_cache->lookup(req, &lfoFeature)) {
                hits++;
            } else {
                lfo_cache->admit(req, &lfoFeature);
            }
        }
        prev_o_features.push_back(o_feature);
    }

    return hits;

}


void run_lfo_sim(const char* path, const std::string cache_type, const uint64_t cache_size) {  
    pthread_t threads[MAIN_THREADS];
    size_t batch_size = 1000;
    size_t epoch = 0;
    std::vector<std::vector<double>> prev_o_features;
    std::vector<SimpleRequest> prev_traces;
    LFOTrainUtil lfoTrainUtil;
    bool run_lfo = false;

    LFOCache lfo_cache(0.5);
    lfo_cache.setSize(cache_size);

    std::ifstream fstream;
    fstream.open(path);

    void* cache = new LRUCache();
    ((LRUCache *)cache)->setSize(cache_size);

    while(true) {

        size_t hits = run_model(fstream, batch_size, prev_o_features, cache, lfoTrainUtil, run_lfo, prev_traces);
        lfoTrainUtil.reset();

        cout << epoch << ": " << double(hits)/batch_size << std::endl;

        if (!prev_o_features.empty()) {
            auto opt_decisions = get_opt_decisions(prev_traces, cache_size);
            lfo_cache.re_train_model(opt_decisions, prev_o_features);
            if (!run_lfo) {
                cache = &lfo_cache;
                run_lfo = true;
            }
            prev_o_features.clear();
        }
        // exit(0);
        ++epoch;
}
    fstream.close();
}