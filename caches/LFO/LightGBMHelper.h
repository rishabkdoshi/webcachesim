//
// Created by Rishab Ketan Doshi on 11/25/19.
//

#ifndef WEBCACHESIM_LIGHTGBMHELPER_H
#define WEBCACHESIM_LIGHTGBMHELPER_H


#include "lfo-feature.h"
#include "LFOCache.h"
#include <LightGBM/c_api.h>
//typedef double dvarType;

class LightGBMHelper {

protected:
    BoosterHandle boosterHandle;
    int numIterations;
    DatasetHandle dataHandle;


public:
    double getUtility(const double *featureVector, const int featureLength);
    int train(const double **featureVector, const double *labels, const int numSamples, const int featureLength);

    LightGBMHelper(){
        dataHandle = nullptr;
        boosterHandle = nullptr;
        numIterations = 30;
    }

};


#endif //WEBCACHESIM_LIGHTGBMHELPER_H