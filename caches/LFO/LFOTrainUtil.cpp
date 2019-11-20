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
            LFOFeature newLfoFeature(r, getUpdatedList(r, prevFeature),getFreeBytes(r));
            it->second.push_back(newLfoFeature);
        }else{
            vector<LFOFeature> featureList;
            vector<uint64_t> newTimeGapList;
            LFOFeature lfoFeature(r,newTimeGapList,getFreeBytes(r));
            (featureList).push_back(lfoFeature);
            _requestToFeatureMap.insert({r.getId(),featureList});
        }
    }
}

uint64_t LFOTrainUtil::getFreeBytes(SimpleRequest r){
    auto freeBytes = _initialCache->getFreeBytes();
    if(!_initialCache->lookup(&r)) {
        _initialCache->admit(&r);
    }
    return freeBytes;
}

vector<uint64_t> LFOTrainUtil::getUpdatedList(SimpleRequest r, LFOFeature lfoFeature){
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

void LFOTrainUtil::train() {
    createFeatures(_reqs);
    cout << "Done" << endl;
}