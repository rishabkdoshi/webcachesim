//
// Created by Rishab Ketan Doshi on 11/18/19.
//

#include "LFOTrainUtil.h"
#include "vector"

void LFOTrainUtil::createFeatures(vector<SimpleRequest> reqs) {

    for(SimpleRequest r : reqs){
        auto it = _requestToFeatureMap.find(r.getId());

        if(it != _requestToFeatureMap.end()){
            vector<LFOFeature> featureList = it->second;
            int size = featureList.size();
            LFOFeature prevFeature = featureList.at(size - 1);
            LFOFeature newLfoFeature(r, getUpdatedTimeGapList(r, prevFeature), _trainingCache->getFreeBytes());
            getCacheHitOrMiss(r) ? newLfoFeature.setLabel(CACHE_HIT) : newLfoFeature.setLabel(CACHE_MISS);
            it->second.push_back(newLfoFeature);
        }else{
            vector<LFOFeature> featureList;
            vector<uint64_t> newTimeGapList;
            LFOFeature lfoFeature(r,newTimeGapList, _trainingCache->getFreeBytes());
            getCacheHitOrMiss(r) ? lfoFeature.setLabel(CACHE_HIT) : lfoFeature.setLabel(CACHE_MISS);
            (featureList).push_back(lfoFeature);
            _requestToFeatureMap.insert({r.getId(),featureList});
        }
    }
}

bool LFOTrainUtil::getCacheHitOrMiss(SimpleRequest r){
    auto cacheHit = _trainingCache->lookup(&r);
    auto shouldAdmit = _trainingCache->shouldAdmit(&r);

    if((!cacheHit) && shouldAdmit) {
        _trainingCache->admit(&r);
    }
    return cacheHit;
}

vector<uint64_t> LFOTrainUtil::getUpdatedTimeGapList(SimpleRequest r, LFOFeature lfoFeature){
    vector<uint64_t> oldTimeGapList = lfoFeature.getTimeGapList();
    uint64_t diff = r.getTimestamp() - lfoFeature.getTimestamp();
    vector<uint64_t> newTimeGapList;

    for (vector<uint64_t>::iterator it=oldTimeGapList.begin(); it != oldTimeGapList.end(); ++it){
        newTimeGapList.push_back((diff+(*it)));
    }

    //last request
    newTimeGapList.push_back(diff);

    if(newTimeGapList.size() > 50){
        newTimeGapList.erase(newTimeGapList.begin());
    }

    return newTimeGapList;
}

vector<vector<uint64_t>> LFOTrainUtil::getFeatureVectors() {
    vector<vector<uint64_t>> featureVectors;

    for(auto it = _requestToFeatureMap.begin(); it != _requestToFeatureMap.end(); ++it){
        auto LFOFeatureList = it->second;

        for(auto feature = LFOFeatureList.begin(); feature != LFOFeatureList.end(); ++feature){
            featureVectors.push_back((*feature).getFeatureVector());
        }
    }

    cout << "Done creating feature vectors" << endl;
}