//
// Created by Rishab Ketan Doshi on 11/18/19.
//

#ifndef WEBCACHESIM_LFOTRAINUTIL_H
#define WEBCACHESIM_LFOTRAINUTIL_H

#include "request.h"
#include "lfo-feature.h"
#include "vector"
#include "unordered_map"
#include <list>
#include <cache.h>


class LFOTrainUtil {

private:

    //map from objectId to list of LFO features
    static unordered_map<uint64_t, vector<LFOFeature>> _requestToFeatureMap;

    static vector<uint64_t> getUpdatedTimeGapList(SimpleRequest r, LFOFeature lfoFeature){
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

public:
<<<<<<< Updated upstream
    LFOTrainUtil(){

    }
=======
    virtual bool lookup(SimpleRequest* req);
    virtual bool lookup(std::vector<uint64_t> ofeature, IdType id);
    virtual void admit(SimpleRequest* req);
    virtual void admit(std::vector<uint64_t> ofeatures);
    virtual void evict(SimpleRequest* req);
    virtual void evict(std::vector<uint64_t> ofeature);
    virtual void evict();
    virtual SimpleRequest* evict_return();

    LFOTrainUtil() : Cache() {
    }

    LFOTrainUtil(uint64_t cache_size) : Cache() {
        setSize(cache_size);
    }

};
>>>>>>> Stashed changes

    ~LFOTrainUtil(){

    }

    void reset(){
        _requestToFeatureMap.clear();
    }

    static LFOFeature getLFOFeature(SimpleRequest r, Cache cache){
        auto it = _requestToFeatureMap.find(r.getId());

        if(it != _requestToFeatureMap.end()){
            vector<LFOFeature> featureList = it->second;
            int size = featureList.size();
            LFOFeature prevFeature = featureList.at(size - 1);
            LFOFeature newLfoFeature(r, getUpdatedTimeGapList(r, prevFeature), cache.getFreeBytes());
            it->second.push_back(newLfoFeature);
            return newLfoFeature;
        }else{
            vector<LFOFeature> featureList;
            vector<uint64_t> newTimeGapList;
//            uint64_t *a = newTimeGapList.data();
            LFOFeature lfoFeature(r,newTimeGapList, cache.getFreeBytes());
            (featureList).push_back(lfoFeature);
            _requestToFeatureMap.insert({r.getId(),featureList});
            return lfoFeature;
        }
    }
};


#endif //WEBCACHESIM_LFOTRAINUTIL_H
