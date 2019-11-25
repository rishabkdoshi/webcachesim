//
// Created by Rishab Ketan Doshi on 11/24/19.
//

#ifndef WEBCACHESIM_LFOCACHE_H
#define WEBCACHESIM_LFOCACHE_H


#include <cache.h>
#include <caches/cache_object.h>
#include <list>

typedef std::list<CacheObject>::iterator ListIteratorType;
typedef std::unordered_map<CacheObject, ListIteratorType> lfoCacheMapType;

class LFOCache : Cache {

protected:
    // list for recency order
    // std::list is a container, usually, implemented as a doubly-linked list
    std::list<CacheObject> _cacheList;
    // map to find objects in list
    lfoCacheMapType _cacheMap;

    virtual void hit(lfoCacheMapType::const_iterator it, uint64_t size);

public:
    LFOCache()
            : Cache()
    {
    }
    virtual ~LFOCache()
    {
    }

    virtual bool lookup(SimpleRequest* req);
    virtual void admit(SimpleRequest* req);
    virtual void evict(SimpleRequest* req);
    virtual void evict();
    virtual SimpleRequest* evict_return();
    virtual bool shouldAdmit();

};


#endif //WEBCACHESIM_LFOCACHE_H
