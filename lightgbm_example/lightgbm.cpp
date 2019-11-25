#include <LightGBM/config.h>
#include <LightGBM/dataset_loader.h>
#include <LightGBM/boosting.h>
#include <LightGBM/objective_function.h>
#include <LightGBM/metric.h>
#include <LightGBM/utils/common.h>
#include <LightGBM/c_api.h>

#include <iostream>
#include <random>
#include <algorithm>

int main(int argc, char **argv)
{
    /* create example dataset */
    std::random_device rd;
    std::mt19937 gen(rd());

    // one random generator for every class
    std::vector<std::normal_distribution<>> dists = {
        std::normal_distribution<>(0, 1),
        std::normal_distribution<>(10, 1)};

    const int numSamples = 5000;
    const int numFeats = 4;
    const int numClasses = static_cast<int>(dists.size());

    std::cout << "Num classes: " << numClasses << std::endl;

    float labels[numSamples];

    for (int i=0; i< numSamples; i++) {
        labels[i] = i % numClasses;
    }

    double sampleData[numSamples][numFeats];

    for (int i=0; i < numSamples; i++) {
        for(int j=0; j < numFeats; j++) {
            const auto lbl = static_cast<int>(labels[i]);
            sampleData[i][j] = dists[lbl](gen);
        }
    }

    // Load sample dataset
    DatasetHandle dataHandle;

    int isdataSetLoaded = LGBM_DatasetCreateFromMat(sampleData, C_API_DTYPE_FLOAT64, numSamples, numFeats, 1, "", nullptr, &dataHandle);

    if (isdataSetLoaded == 0) {
        std::cout << "Dataset Loaded succesfully\n";
    }
    else{
        std::cout << "Loading dataset failed\n";
        return EXIT_FAILURE;
    }

    // Add labels to the dataset
    int addedLabels =  LGBM_DatasetSetField(dataHandle, "label", labels, numSamples, C_API_DTYPE_FLOAT32);

    // Create a Learner
    BoosterHandle boosterHandle;
    int isLearnerCreated = LGBM_BoosterCreate(dataHandle, "", &boosterHandle);

    if (isLearnerCreated == 0) {
        std::cout << "Learner created succesfully\n";
    }
    else{
        std::cout << "Creating learner failed\n";
        return EXIT_FAILURE;
    }   

    // Train for 50 iterations
    for (int i=0 ; i < 51; i++) {
        int isFinished;
        int isUpdated = LGBM_BoosterUpdateOneIter(boosterHandle, &isFinished);
        if (isUpdated != 0) {
            std::cout << "Failed to update at iteration number " << i << "\n"; 
        }
        if (isFinished == 0) {
            std::cout << "No further gain, cannot split anymore\n";
            break;
        }
    }
    std::cout << "Training done" << "\n";

    // Uncomment line to save model to a file.
    // LGBM_BoosterSaveModel(boosterHandle, 0, -1, "trained_model.txt");

    // Generate predicted data
    int numPredicts = 100;
    double predictData[numPredicts][numFeats];

    for (int i=0; i < numPredicts; i++) {
        for(int j=0; j < numFeats; j++) {
            const auto lbl = static_cast<int>(labels[i]);
            predictData[i][j] = dists[lbl](gen);
        }
    }

    // Run predictions
    int64_t predictionsLength;
    double * predictions = new double[numPredicts];

    int hasPredicted = LGBM_BoosterPredictForMat(boosterHandle, predictData, C_API_DTYPE_FLOAT64,
        numPredicts, numFeats, 1, C_API_PREDICT_RAW_SCORE, -1, "", &predictionsLength, predictions);

    std::cout << hasPredicted << '\n';

    std::cout << predictionsLength << '\n';

    if (predictionsLength != numPredicts) {
        std::cout << "Prediction array failed for some entries\n";
        return EXIT_FAILURE;
    }
    // compute error
    double err = 0;
    for (int i=0; i < numPredicts; i++)
    {
        if (round(predictions[i]) != labels[i])
        {
            err++;
        }
    }
    err /= numPredicts;
    std::cout << "Training error: " << err << std::endl;

    return EXIT_SUCCESS;
}