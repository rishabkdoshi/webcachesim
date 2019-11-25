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
    LFOTrainUtil(){

    }

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
