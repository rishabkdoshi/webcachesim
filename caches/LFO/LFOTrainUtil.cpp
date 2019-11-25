//
// Created by Rishab Ketan Doshi on 11/18/19.
//

#include "LFOTrainUtil.h"
#include "vector"

LFOFeature LFOTrainUtil::getLFOFeature(SimpleRequest r) {

        auto it = _requestToFeatureMap.find(r.getId());

        if(it != _requestToFeatureMap.end()){
            vector<LFOFeature> featureList = it->second;
            int size = featureList.size();
            LFOFeature prevFeature = featureList.at(size - 1);
            LFOFeature newLfoFeature(r, getUpdatedTimeGapList(r, prevFeature), getFreeBytes());
            it->second.push_back(newLfoFeature);
            return newLfoFeature;
        }else{
            vector<LFOFeature> featureList;
            vector<uint64_t> newTimeGapList;
            LFOFeature lfoFeature(r,newTimeGapList, getFreeBytes());
            (featureList).push_back(lfoFeature);
            _requestToFeatureMap.insert({r.getId(),featureList});
            return lfoFeature;
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

SimpleRequest* LFOTrainUtil::evict_return()
{
    // evict least popular (i.e. last element)
    return NULL;
}

void LFOTrainUtil::evict()
{
    evict_return();
}