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
            LFOFeature newLfoFeature(r, getUpdatedTimeGapList(r, prevFeature), getFreeBytes());
            it->second.push_back(newLfoFeature);
            
            _features.push_back(newLfoFeature.getFeatureVector().second);
        }else{
            vector<LFOFeature> featureList;
            vector<uint64_t> newTimeGapList;
            LFOFeature lfoFeature(r,newTimeGapList, getFreeBytes());
            (featureList).push_back(lfoFeature);
            _requestToFeatureMap.insert({r.getId(),featureList});

            _features.push_back(lfoFeature.getFeatureVector().second);
        }
    }

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
    // vector<vector<uint64_t>> featureVectors;

    // for(auto it = _requestToFeatureMap.begin(); it != _requestToFeatureMap.end(); ++it){
    //     auto LFOFeatureList = it->second;

    //     for(auto feature = LFOFeatureList.begin(); feature != LFOFeatureList.end(); ++feature){
    //         featureVectors.push_back((*feature).getFeatureVector());
    //     }
    // }
    return _features;
}

void LFOTrainUtil::hit(lfoCacheMapType::const_iterator it, uint64_t size) {
     _cacheList.splice(_cacheList.begin(), _cacheList, it->second);
}

bool LFOTrainUtil::lookup(SimpleRequest* req) {
    CacheObject obj(req);
    // _cacheMap defined in class LRUCache in lru_variants.h 
    auto it = _cacheMap.find(obj);
    if (it != _cacheMap.end()) {
        hit(it, obj.size);
        return true;
    }
    return false;
}

bool LFOTrainUtil::lookup(std::vector<uint64_t> ofeature, IdType id) {
}

void LFOTrainUtil::admit(SimpleRequest* req) {

}

void LFOTrainUtil::admit(std::vector<std::vector<uint64_t>> ofeature) {
    // Run the model here and then check it. 
}

void LFOTrainUtil::evict(SimpleRequest* req) {

}