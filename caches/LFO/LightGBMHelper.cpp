//
// Created by Rishab Ketan Doshi on 11/25/19.
//

#include "lfo-feature.h"
#include "LFOCache.h"
#include <LightGBM/c_api.h>
#include "LightGBMHelper.h"
//typedef double dvarType;

double LightGBMHelper::getUtility(const double *featureVector, const int featureLength){
        double default_val;
        int64_t predictionsLength;
        double * predictions;

        int hasPredicted = LGBM_BoosterPredictForMatSingleRow(boosterHandle, featureVector, C_API_DTYPE_FLOAT64,
                                                              featureLength, 1, C_API_PREDICT_RAW_SCORE, -1, "", &predictionsLength, predictions);

        if (predictionsLength != 1) {
            std::cout << "predictionsLength returned more than 1 value for input";
        }

        return *predictions;
}

void LightGBMHelper::train(const double **featureVector, const double *labels, const int numSamples, const int featureLength){
        int isdataSetLoaded;

        if (dataHandle == nullptr){
            isdataSetLoaded = LGBM_DatasetCreateFromMat(featureVector, C_API_DTYPE_FLOAT64, numSamples,
                                                        featureLength, 1, "", nullptr, &dataHandle);
        } else {
            isdataSetLoaded = LGBM_BoosterResetTrainingData(dataHandle, featureVector);
        }
        if (isdataSetLoaded != 0) {
            std::cout << "Loading dataset failed\n";
        }

        LGBM_DatasetSetField(dataHandle, "label", labels, numSamples, C_API_DTYPE_FLOAT32);
        int isLearnerCreated = LGBM_BoosterCreate(dataHandle, "", &boosterHandle);

        if (isLearnerCreated != 0) {
            std::cout << "Creating learner failed\n";
        }

        for (int i=0 ; i < numIterations; i++) {
            int isFinished;
            int isUpdated = LGBM_BoosterUpdateOneIter(boosterHandle, &isFinished);
            if (isUpdated != 0) {
                std::cout << "Failed to update at iteration number " << i << "\n";
            }
            if (isFinished == 0) {
                std::cout << "No further gain, cannot split anymore\n";
                std::cout << "here";
                return;
            }
        }
}