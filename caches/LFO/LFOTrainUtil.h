//
// Created by Rishab Ketan Doshi on 11/18/19.
//

#ifndef WEBCACHESIM_LFOTRAINUTIL_H
#define WEBCACHESIM_LFOTRAINUTIL_H

#include "request.h"
#include "lfo-feature.h"
#include "vector"
#include "unordered_map"

class LFOTrainUtil {
private:
    vector<SimpleRequest> _reqs;
    vector<uint64_t> getUpdatedList(SimpleRequest r, LFOFeature lfoFeature);
    void createFeatures(vector<SimpleRequest> reqs);

    //map from objectId to list of LFO features
    unordered_map<uint64_t, vector<LFOFeature>> _requestToFeatureMap;

public:
    void train();
    LFOTrainUtil(vector<SimpleRequest> reqs): _reqs(reqs){
    }
};


#endif //WEBCACHESIM_LFOTRAINUTIL_H
