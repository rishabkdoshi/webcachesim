//
// Created by Rishab Ketan Doshi on 11/18/19.
//

#ifndef WEBCACHESIM_LFOTRAINUTIL_H
#define WEBCACHESIM_LFOTRAINUTIL_H

#include "../../cache.h"
#include "request.h"
#include "lfo-feature.h"
#include "vector"
#include "unordered_map"
#include "../cache_object.h"
#include <list>


typedef std::list<CacheObject>::iterator ListIteratorType;
typedef std::unordered_map<CacheObject, ListIteratorType> lfoCacheMapType;

class LFOTrainUtil : Cache {
private:
    vector<SimpleRequest> _reqs;
    static vector<uint64_t> getUpdatedTimeGapList(SimpleRequest r, LFOFeature lfoFeature);
    void createFeatures(vector<SimpleRequest> reqs);
    bool getCacheHitOrMiss(SimpleRequest r);
    std::list<CacheObject> _cacheList;
    // map to find objects in list
    lfoCacheMapType _cacheMap;

    //map from objectId to list of LFO features
    unordered_map<uint64_t, vector<LFOFeature>> _requestToFeatureMap;
    vector<vector<uint64_t>> _features;
    virtual void hit(lfoCacheMapType::const_iterator it, uint64_t size);

public:
    vector<vector<uint64_t>> getFeatureVectors();
    virtual bool lookup(SimpleRequest* req);
    virtual bool lookup(std::vector<uint64_t> ofeature, IdType id);
    virtual void admit(SimpleRequest* req);
    virtual void admit(vector<vector<uint64_t>> ofeatures);
    virtual void evict(SimpleRequest* req);

    // LFOTrainUtil(vector<SimpleRequest> reqs, string cacheType, uint64_t cacheSize): _reqs(reqs){
    //     _trainingCache =  (Cache::create_unique(cacheType));
    //     _trainingCache->setSize(cacheSize);
    //     createFeatures(reqs);
    // }
};

static Factory<LFOTrainUtil> factoryLRU("LFO"); 


#endif //WEBCACHESIM_LFOTRAINUTIL_H
