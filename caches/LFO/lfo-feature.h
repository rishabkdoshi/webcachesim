//
// Created by Rishab Ketan Doshi on 11/18/19.
//

#ifndef WEBCACHESIM_LFO_FEATURE_H
#define WEBCACHESIM_LFO_FEATURE_H

#include <cstdint>
#include <iostream>
#include <vector>
#include <request.h>
#include "OptimizationGoal.h"
#include "alglib/src/stdafx.h"
#include "alglib/src/dataanalysis.h"

using namespace std;

const uint64_t MISSING_TIME_GAP = 0;
const uint64_t CACHE_HIT = 1;
const uint64_t CACHE_MISS = 0;
const double ALPHA = 0.5;


/**
 * Object size;
 * Most recent retrieval cost;
 * Currently free (available) bytes in the cache;
 * Time gap between consecutive requests to this object
 * */

typedef std::vector<uint64_t>::iterator timeListIteratorType;
typedef double featureType;


// Request information
class LFOFeature
{
private:
    IdType _id; // request object id
    uint64_t _size; // request size in bytes
    OptimizationGoal _optimizationGoal; //OHR or BHR
    uint64_t _timestamp;//timestamp of request corresponding to this feature
    vector<uint64_t> _time_gap_list; //Time gap between consecutive requests to this object
    uint64_t _available_cache_size;

    uint64_t _label; //admitted to cache or not

    //feature enhancement
    bool _use_exponential_time_gap;
    bool _use_rl_cache_features;

    //RL Cache related features
    double _frequency;
    uint64_t _temporal_recency;
    double _rho_j; //exponential smoothing of rj - _temporal_recency
    uint64_t _ordinal_recency;
    double _delta_j;//exponential smoothing of dj - _ordinal_recency

    uint64_t _request_no; // request no in the trace (helps in calculating ordinal recency)
    uint64_t _times_requested; //no. of times requested (helps in calculating frequency)
    vector<double> _temporal_recency_list; //used for calculating rho_j
    vector<double> _ordinal_recency_list; //used for calculating delta_j



    vector<featureType> _features;
    
    void createFeatureVector(){
        _features.push_back(_size);
        _features.push_back(getRetrievalCost());
        _features.push_back(_available_cache_size);


        if(_use_rl_cache_features){
            _features.push_back(_frequency);
            _features.push_back(_temporal_recency);
            _features.push_back(_ordinal_recency);
            _features.push_back(_rho_j);
            _features.push_back(_delta_j);
            _features.push_back((_frequency/_size));
            _features.push_back((_frequency*_size));
        }

        addTimeGapFeature();

    }

    vector<featureType> addTimeGapFeature(){
        if(_use_exponential_time_gap){
            vector<uint64_t> timeGaps;
            for(int i=1; i < 2050 && i < _time_gap_list.size(); i*=2){
                timeGaps.push_back(_time_gap_list.at(i-1));
            }

//            1,2,4,16,32,64,128,256,512,1024,2048 - size 11
            for(int i=timeGaps.size();i<12;i++){
                _features.push_back(MISSING_TIME_GAP);
            }

            _features.insert(_features.end(), timeGaps.begin(), timeGaps.end());

        }else{
            for(int i=_time_gap_list.size();i<50;i++)
                _features.push_back(MISSING_TIME_GAP);

            for (auto it = _time_gap_list.begin(); it != _time_gap_list.end(); ++it){
                _features.push_back(*it);
            }
        }
    }

public:
    // Create request
    LFOFeature(IdType id, uint64_t size, uint64_t timestamp, vector<uint64_t> time_gap_list, uint64_t available_cache_size, bool useExponentialTimeGap = false, bool useRLCacheFeatures = false)
            : _id(id),
              _size(size),
              _timestamp(timestamp),
              _time_gap_list(time_gap_list),
              _available_cache_size(available_cache_size),
              _use_exponential_time_gap(useExponentialTimeGap),
              _use_rl_cache_features(useRLCacheFeatures)
    {
        //default
        _optimizationGoal = BYTE_HIT_RATIO;
        createFeatureVector();
    }

    LFOFeature(SimpleRequest simpleRequest, vector<uint64_t> time_gap_list, uint64_t available_cache_size, bool useExponentialTimeGap = false, bool useRLCacheFeatures = false)
        : _id(simpleRequest.getId()),
        _size(simpleRequest.getSize()),
        _timestamp(simpleRequest.getTimestamp()),
        _time_gap_list(time_gap_list),
        _available_cache_size(available_cache_size),
        _use_exponential_time_gap(useExponentialTimeGap),
        _use_rl_cache_features(useRLCacheFeatures) {
        _optimizationGoal = BYTE_HIT_RATIO;
        createFeatureVector();
    }

    // Get request object id
    IdType getId() const { return _id; }
    // Get request size in bytes
    uint64_t getSize() const { return _size; }
    uint64_t getRetrievalCost() const {
        if(_optimizationGoal == BYTE_HIT_RATIO) return getSize();
        return 1;
    }
    vector<uint64_t> &getTimeGapList() { return _time_gap_list; }
    uint64_t getTimestamp() const { return _timestamp; }
    void setOptimizationGoal(OptimizationGoal optimizationGoal) { 
        _optimizationGoal = optimizationGoal;
    }
    void setTimestamp(int timestamp) { _timestamp = timestamp; }
    uint64_t getAvailableCacheSize() const { return _available_cache_size; }
    void setLabel(uint64_t label) { _label = label; }

    std::vector<featureType> getFeatureVector() const {
        return _features;
    }
    // Caching policies

    bool isUseExponentialTimeGap() const {
        return _use_exponential_time_gap;
    }

    void setUseExponentialTimeGap(bool useExponentialTimeGap) {
        _use_exponential_time_gap = useExponentialTimeGap;
    }

    bool isUseRlCacheFeatures() const {
        return _use_rl_cache_features;
    }

    void setUseRlCacheFeatures(bool useRlCacheFeatures) {
        _use_rl_cache_features = useRlCacheFeatures;
    }

    void calculateRhoJ(){
        alglib::real_1d_array x;// = str;
        x.setcontent(_temporal_recency_list.size(), &(_temporal_recency_list[0]));
        filterema(x, ALPHA);
        _rho_j = x[_temporal_recency_list.size() - 1];
    }

    void calculateDeltaJ(){
        alglib::real_1d_array x;// = str;
        x.setcontent(_ordinal_recency_list.size(), &(_ordinal_recency_list[0]));
        filterema(x, ALPHA);
        _delta_j = x[_ordinal_recency_list.size() - 1];
    }

    double getFrequency() const {
        return _frequency;
    }

    void setFrequency(double frequency) {
        _frequency = frequency;
    }

    uint64_t getTemporalRecency() const {
        return _temporal_recency;
    }

    void setTemporalRecency(uint64_t temporalRecency) {
        _temporal_recency = temporalRecency;
    }

    uint64_t getOrdinalRecency() const {
        return _ordinal_recency;
    }

    void setOrdinalRecency(uint64_t ordinalRecency) {
        _ordinal_recency = ordinalRecency;
    }

    uint64_t getRequestNo() const {
        return _request_no;
    }

    void setRequestNo(uint64_t requestNo) {
        _request_no = requestNo;
    }

    uint64_t getTimesRequested() const {
        return _times_requested;
    }

    void setTimesRequested(uint64_t timesRequested) {
        _times_requested = timesRequested;
    }
};

#endif //WEBCACHESIM_LFO_FEATURE_H
