//
// Created by Rishab Ketan Doshi on 11/24/19.
//

#ifndef WEBCACHESIM_LFOCACHE_H
#define WEBCACHESIM_LFOCACHE_H


#include "cache.h"
#include "caches/cache_object.h"
#include <list>
#include "lfo-feature.h"
#include "LightGBMHelper.h"
#include <queue>

typedef std::unordered_map<IdType, CacheObject> lfoCacheMapType;
typedef double dvarType;

class LFOCache : Cache {
private:
    struct GreaterCacheObject {
        bool operator()(CacheObject const& p1, CacheObject const& p2)
        {
            // return "true" if "p1" is ordered
            // before "p2", for example:
            return p1.dvar > p2.dvar;
        }
    };

protected:

    // map to find objects in list
    lfoCacheMapType _cacheMap;

    //min pq to decide which cacheobject to evict
    std::priority_queue<CacheObject, vector<CacheObject>, GreaterCacheObject> _cacheObjectMinpq;

    LightGBMHelper _gbmHelper;

    dvarType _threshold;


public:
    LFOCache(dvarType threshold)
    {
        _threshold = threshold;
    }
    virtual ~LFOCache()
    {
    }

    virtual bool lookup(SimpleRequest* req, LFOFeature* lfoFeature);
    virtual void admit(SimpleRequest* req, LFOFeature* lfoFeature);
    virtual void evict(SimpleRequest* req);
    virtual void evict();
    virtual SimpleRequest* evict_return();
    virtual bool shouldAdmit(SimpleRequest *req, LFOFeature *lfoFeature);

};


#endif //WEBCACHESIM_LFOCACHE_H
