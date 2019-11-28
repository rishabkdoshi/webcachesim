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
    unordered_map<uint64_t, vector<LFOFeature>> _requestToFeatureMap;

    vector<uint64_t> getUpdatedTimeGapList(SimpleRequest r, LFOFeature lfoFeature){
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
    LFOTrainUtil(){

    }

    void reset() {
        _requestToFeatureMap.clear();
    }

    LFOFeature getLFOFeature(SimpleRequest r, size_t cache_free_bytes){
        auto it = _requestToFeatureMap.find(r.getId());

        if(it != _requestToFeatureMap.end()){
            vector<LFOFeature> featureList = it->second;
            int size = featureList.size();
            LFOFeature prevFeature = featureList.at(size - 1);
            LFOFeature newLfoFeature(r, getUpdatedTimeGapList(r, prevFeature), cache_free_bytes);
            it->second.push_back(newLfoFeature);
            return newLfoFeature;
        }else{
            vector<LFOFeature> featureList;
            vector<uint64_t> newTimeGapList;
            LFOFeature lfoFeature(r,newTimeGapList, cache_free_bytes);
            (featureList).push_back(lfoFeature);
            _requestToFeatureMap.insert({r.getId(),featureList});
            return lfoFeature;
        }
    }
};


#endif //WEBCACHESIM_LFOTRAINUTIL_H
