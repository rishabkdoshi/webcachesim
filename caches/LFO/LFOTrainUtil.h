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
    static vector<uint64_t> getUpdatedTimeGapList(SimpleRequest r, LFOFeature lfoFeature);
    void createFeatures(vector<SimpleRequest> reqs);
    bool getCacheHitOrMiss(SimpleRequest r);

    //map from objectId to list of LFO features
    unordered_map<uint64_t, vector<LFOFeature>> _requestToFeatureMap;

    unique_ptr<Cache> _trainingCache;//cache used for training feature calculations

public:
    vector<vector<uint64_t>> getFeatureVectors();

    LFOTrainUtil(vector<SimpleRequest> reqs, string cacheType, uint64_t cacheSize): _reqs(reqs){
        _trainingCache =  (Cache::create_unique(cacheType));
        _trainingCache->setSize(cacheSize);
        createFeatures(reqs);
    }
};


#endif //WEBCACHESIM_LFOTRAINUTIL_H
