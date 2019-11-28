//
// Created by Rishab Ketan Doshi on 11/24/19.
//

#include "LFOCache.h"
#include "lfo-feature.h"


bool LFOCache::lookup(SimpleRequest* req, LFOFeature* lfoFeature){
    auto it = _cacheMap.find(req->getId());
    if(it != _cacheMap.end()) return true;
    return false;
}

void LFOCache::admit(SimpleRequest* req, LFOFeature* lfoFeature){
    const uint64_t size = req->getSize();

    if(shouldAdmit(req,lfoFeature)){
        while (_currentSize + size > _cacheSize) {
            evict();
        }

        CacheObject obj(req);
        _cacheMap.insert({lfoFeature->getId(),obj });
        _cacheObjectMinpq.push(obj);
        _currentSize += size;
    }
}

void LFOCache::evict(SimpleRequest *req){
    throw "Random eviction not supported for LFOCache";
}

void LFOCache::evict(){
    evict_return();
}

SimpleRequest* LFOCache::evict_return(){
    if(_cacheObjectMinpq.size() > 0){
        CacheObject obj = _cacheObjectMinpq.top();
        _currentSize -= obj.size;
        _cacheMap.erase(obj.id);
        _cacheObjectMinpq.pop();
        SimpleRequest *req = new SimpleRequest(obj.id,obj.size);
        return req;
    }
    return NULL;
}

bool LFOCache::shouldAdmit(SimpleRequest *req, LFOFeature *lfoFeature) {
    const vector<featureType> &features = lfoFeature->getFeatureVector().second;
    auto featureVector = features.data(); //double*

    return _gbmHelper.getUtility(featureVector, features.size()) >= _threshold;
}