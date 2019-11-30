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
    unordered_map<uint64_t, LFOFeature> _requestToPrevRLFeatureMap;

    bool useRLCacheFeatures;
    bool useExponentialTimeGap;

    uint64_t _requestsSoFar;

    vector<uint64_t> getUpdatedTimeGapList(SimpleRequest r, LFOFeature lfoFeature){
        vector<uint64_t> oldTimeGapList = lfoFeature.getTimeGapList();
        uint64_t diff = r.getTimestamp() - lfoFeature.getTimestamp();
        vector<uint64_t> newTimeGapList;

        for (vector<uint64_t>::iterator it=oldTimeGapList.begin(); it != oldTimeGapList.end(); ++it){
            newTimeGapList.push_back((diff+(*it)));
        }

        //last request
        newTimeGapList.push_back(diff);

        if(lfoFeature.isUseExponentialTimeGap()){
            if(newTimeGapList.size() > 2050){
                newTimeGapList.erase(newTimeGapList.begin());
            }
        }else{
            if(newTimeGapList.size() > 50){
                newTimeGapList.erase(newTimeGapList.begin());
            }
        }


        return newTimeGapList;
    }

    LFOFeature getLFOFeature(SimpleRequest r, size_t cache_free_bytes){
        auto it = _requestToFeatureMap.find(r.getId());

        if(it != _requestToFeatureMap.end()){
            vector<LFOFeature> featureList = it->second;
            int size = featureList.size();
            LFOFeature prevFeature = featureList.at(size - 1);
            LFOFeature newLfoFeature(r, getUpdatedTimeGapList(r, prevFeature), cache_free_bytes, useExponentialTimeGap);
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

    LFOFeature getRLFeature(SimpleRequest r, size_t cache_free_bytes){
        auto it = _requestToPrevRLFeatureMap.find(r.getId());

        _requestsSoFar+=1;

        if(it != _requestToPrevRLFeatureMap.end()){
            LFOFeature prevLFOFeature = it->second;

            LFOFeature newLfoFeature(r, getUpdatedTimeGapList(r, prevLFOFeature), cache_free_bytes,useExponentialTimeGap, useRLCacheFeatures);
            newLfoFeature.setRequestNo(_requestsSoFar);
            newLfoFeature.setOrdinalRecency( _requestsSoFar - prevLFOFeature.getRequestNo() + 1);
            newLfoFeature.setTemporalRecency(r.getTimestamp() - prevLFOFeature.getTimestamp());
            newLfoFeature.setTimesRequested(1+prevLFOFeature.getTimesRequested());
            newLfoFeature.setFrequency(newLfoFeature.getTimesRequested() / _requestsSoFar);
            newLfoFeature.calculateRhoJ();
            newLfoFeature.calculateDeltaJ();

            _requestToPrevRLFeatureMap.insert({r.getId(), newLfoFeature});

        }else{
            vector<uint64_t> newTimeGapList;
            LFOFeature lfoFeature(r, newTimeGapList, cache_free_bytes,useExponentialTimeGap, useRLCacheFeatures);
            lfoFeature.setRequestNo(_requestsSoFar);
            lfoFeature.setOrdinalRecency(0);//new request, ordinal recency is 0
            lfoFeature.setTemporalRecency(0);//new request, temporal recency is 0
            lfoFeature.setTimesRequested(1);
            lfoFeature.setFrequency(1.0/_requestsSoFar); //new request, fraction of requests so far
            lfoFeature.calculateRhoJ();
            lfoFeature.calculateDeltaJ();
            _requestToPrevRLFeatureMap.insert({r.getId(), lfoFeature});
        }
    }

public:
    LFOTrainUtil(){
        _requestsSoFar = 0;
        useRLCacheFeatures = false;
        useExponentialTimeGap = false;
    }

    void reset() {
        _requestToFeatureMap.clear();
        _requestToPrevRLFeatureMap.clear();
        _requestsSoFar = 0;
    }

    LFOFeature getFeature(SimpleRequest r, size_t cache_free_bytes){
        if(useRLCacheFeatures) return getRLFeature(r,cache_free_bytes);
        return getLFOFeature(r,cache_free_bytes);
    }

    void setUseRlCacheFeatures(bool useRlCacheFeatures) {
        useRLCacheFeatures = useRlCacheFeatures;
    }

    void setUseExponentialTimeGap(bool useExponentialTimeGap) {
        LFOTrainUtil::useExponentialTimeGap = useExponentialTimeGap;
    }
};


#endif //WEBCACHESIM_LFOTRAINUTIL_H
