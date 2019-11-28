cmake_minimum_required(VERSION 3.15)
project(webcachesim)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(caches)

add_executable(webcachesim
        caches/adaptsize_const.h
        caches/cache_object.h
        caches/gd_variants.cpp
        caches/gd_variants.h
        caches/lru_variants.cpp
        caches/lru_variants.h
        bench.h
        cache.h
        random_helper.cpp
        random_helper.h
        request.h
        webcachesim.cpp
        caches/LFO/lfo-feature.h
        caches/LFO/OptimizationGoal.h
        caches/LFO/LFOTrainUtil.h
        caches/LFO/LFOCache.cpp
        caches/LFO/LFOCache.h)

add_executable(lfosim
        caches/adaptsize_const.h
        caches/cache_object.h
        caches/gd_variants.cpp
        caches/gd_variants.h
        caches/lru_variants.cpp
        caches/lru_variants.h
        caches/LFO/lfo-feature.h
        caches/LFO/LFOTrainUtil.h
        caches/LFO/OptimizationGoal.h
        bench.h
        cache.h
        random_helper.cpp
        random_helper.h
        request.h
        lfosim.cpp
        caches/LFO/LFOCache.cpp
        caches/LFO/LFOCache.h
        lfosim_helper.cpp
        caches/LFO/LFOCache.cpp
        caches/LFO/LFOCache.h
        caches/LFO/LightGBMHelper.h
        caches/OPTU/parse_trace.cpp
        caches/OPTU/parse_trace.h
        caches/OPTU/solve_mcf.cpp
        caches/OPTU/solve_mcf.h
        LightGBM/c_api.h caches/LFO/LightGBMHelper.cpp)

find_library(LIGHTGBM_LIB lib_lightgbm.dylib LightGBM)
target_link_libraries(lfosim ${LIGHTGBM_LIB})


