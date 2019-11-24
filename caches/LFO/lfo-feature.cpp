#include "lfo-feature.h"
#include "../request.h"

LFOFeature::LFOFeature(SimpleRequest simpleRequest, 
                       vector<uint64_t> time_gap_list, 
                       uint64_t available_cache_size) {
        _id = simpleRequest.getId();
        _size = simpleRequest.getSize();
        _timestamp = simpleRequest.getTimestamp();
        _time_gap_list = time_gap_list;
        _optimizationGoal = OBJECT_HIT_RATIO;
        _available_cache_size = available_cache_size;
    }


std::pair<IdType, vector<uint64_t>> LFOFeature::getFeatureVector() const {
    vector<uint64_t> features;
    features.push_back(_size);
    features.push_back(getRetrievalCost());
    features.push_back(_available_cache_size);

    for(int i=_time_gap_list.size();i<50;i++)
        features.push_back(MISSING_TIME_GAP);

    for (auto it = _time_gap_list.begin(); it != _time_gap_list.end(); ++it){
        features.push_back(*it);
    }

    // features.push_back(_label);

    return std::make_pair(_id, features);
}