#include <fstream>
#include <string>
#include <regex>
#include "caches/LFO/LFOTrainUtil.h"
#include "caches/lru_variants.h"
#include "caches/gd_variants.h"
#include "request.h"

using namespace std;

int main (int argc, char* argv[])
{

    // output help if insufficient params
//    if(argc < 4) {
//        cerr << "lfosim traceFile cacheType cacheSizeBytes [cacheParams]" << endl;
//        return 1;
//    }

    // trace properties
//    const char* path = argv[1];
    const char* path = "test.tr";

    // create cache
//    const string cacheType = argv[2];
    const string initialCacheType = "LRU";


    // configure cache size
//    const uint64_t cache_size  = std::stoull(argv[3]);
    const uint64_t cacheSize  = 10000;

    // parse cache parameters
//    regex opexp ("(.*)=(.*)");
//    cmatch opmatch;
//    string paramSummary;
//    for(int i=4; i<argc; i++) {
//        regex_match (argv[i],opmatch,opexp);
//        if(opmatch.size()!=3) {
//            cerr << "each cacheParam needs to be in form name=value" << endl;
//            return 1;
//        }
//        webcache->setPar(opmatch[1], opmatch[2]);
//        paramSummary += opmatch[2];
//    }

    ifstream infile;
    long long t, id, size;

    cerr << "running..." << endl;

    infile.open(path);
    std::vector<SimpleRequest> requestList;
    while(infile >> t >> id >> size)
    {
        SimpleRequest req = SimpleRequest(id, size, t);
        requestList.push_back(req);
    }
    LFOTrainUtil lfoTrainUtil(requestList, initialCacheType, cacheSize);
    auto featureVectors = lfoTrainUtil.getFeatureVectors();

    infile.close();

   

    return 0;
}
