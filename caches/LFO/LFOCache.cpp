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
    const vector<featureType> &features = lfoFeature->getFeatureVector();
    auto featureVector = features.data(); //double*

    return _gbmHelper.getUtility(featureVector, features.size()) >= _threshold;
}

void LFOCache::re_train_model(std::vector<double> opt_decisions,
                    std::vector<std::vector<double>> o_features){

    size_t topSize = o_features.size();
    double sampleData[o_features.size()][o_features.at(0).size()];


    int i=0,j=0;
    for(auto o_feature: o_features){
        j=0;
//        sampleData[i] = o_feature.data();
        for(auto ele : o_feature){
            sampleData[i][j] = ele;
            j++;
        }
        i++;
    }

    _gbmHelper.train((const double **) sampleData,opt_decisions.data(), o_features.size(), o_features[0].size());
}