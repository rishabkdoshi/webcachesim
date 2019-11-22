#include <fstream>
#include <map>
#include <cassert>
#include <unordered_map>
#include <tuple>
#include <cmath>
#include "parse_trace.h"

void parseTraceFile(std::vector<trEntry> & opt_trace, 
                    std::vector<SimpleRequest> traces, 
                    uint64_t & byteSum) {

    uint64_t time, id, size, reqc=0;
    std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t> lastSeen;

    for(auto trace : traces) {
        time = trace.getTimestamp();
        id = trace.getId();
        size = trace.getSize();
        const auto idsize = std::make_pair(id,size);
        if(size > 0 && lastSeen.count(idsize)>0) {
            opt_trace[lastSeen[idsize]].hasNext = true;
            const uint64_t volume = (reqc-lastSeen[idsize]) * size;
            opt_trace[lastSeen[idsize]].volume = volume;
        }
        opt_trace.emplace_back(size);
        byteSum += size;
        lastSeen[idsize]=reqc++;
    }
}
