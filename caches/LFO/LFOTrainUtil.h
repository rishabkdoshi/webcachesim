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
    static vector<uint64_t> getUpdatedTimeGapList(SimpleRequest r, LFOFeature lfoFeature);
    LFOFeature getLFOFeature(SimpleRequest r);
    bool getCacheHitOrMiss(SimpleRequest r);
    std::list<CacheObject> _cacheList;
    // map to find objects in list
    lfoCacheMapType _cacheMap;

    //map from objectId to list of LFO features
    unordered_map<uint64_t, vector<LFOFeature>> _requestToFeatureMap;
    virtual void hit(lfoCacheMapType::const_iterator it, uint64_t size);

public:
    virtual bool lookup(SimpleRequest* req);
    virtual bool lookup(std::vector<uint64_t> ofeature, IdType id);
    virtual void admit(SimpleRequest* req);
    virtual void admit(vector<vector<uint64_t>> ofeatures);
    virtual void evict(SimpleRequest* req);
    virtual void evict();
<<<<<<< Updated upstream
    virtual SimpleRequest* evict_return();

    LFOTrainUtil() : Cache() {

=======

    LFOTrainUtil(uint64_t cacheSize) {
        setSize(cacheSize);
>>>>>>> Stashed changes
    }
};

static Factory<LFOTrainUtil> factoryLFO("LFO");


#endif //WEBCACHESIM_LFOTRAINUTIL_H
