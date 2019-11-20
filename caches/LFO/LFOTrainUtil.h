//
// Created by Rishab Ketan Doshi on 11/18/19.
//

#ifndef WEBCACHESIM_LFOTRAINUTIL_H
#define WEBCACHESIM_LFOTRAINUTIL_H

#include <cache.h>
#include "request.h"
#include "lfo-feature.h"
#include "vector"
#include "unordered_map"

class LFOTrainUtil {
private:
    vector<SimpleRequest> _reqs;
    vector<uint64_t> getUpdatedList(SimpleRequest r, LFOFeature lfoFeature);
    void createFeatures(vector<SimpleRequest> reqs);
    uint64_t getFreeBytes(SimpleRequest r);
    //map from objectId to list of LFO features
    unordered_map<uint64_t, vector<LFOFeature>> _requestToFeatureMap;

    unique_ptr<Cache> _initialCache;//cache used for first epoch feature calculations

public:
    void train();
    LFOTrainUtil(vector<SimpleRequest> reqs, string cacheType, uint64_t cacheSize): _reqs(reqs){
        _initialCache =  (Cache::create_unique(cacheType));
        _initialCache->setSize(cacheSize);
    }
};


#endif //WEBCACHESIM_LFOTRAINUTIL_H
