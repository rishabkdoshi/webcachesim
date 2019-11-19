TARGET = webcachesim
OBJS += caches/lru_variants.o
OBJS += caches/gd_variants.o
OBJS += random_helper.o
OBJS += webcachesim.o
LIBS += -lm

#TODO: fix the dependencies
LFO_TARGET = lfosim
LFO_OBJS += caches/lru_variants.o
LFO_OBJS += caches/gd_variants.o
LFO_OBJS += random_helper.o
LFO_OBJS += lfosim.o
LFO_OBJS += caches/LFO/LFOTrainUtil.o

CXX = g++ #clang++ #OSX
CXXFLAGS += -std=c++11 #-stdlib=libc++ #non-linux
CXXFLAGS += -MMD -MP # dependency tracking flags
CXXFLAGS += -I./
# CXXFLAGS += -Wall -Werror
LDFLAGS += $(LIBS)
all: CXXFLAGS += -O2 # release flags
all:		$(TARGET)

debug: CXXFLAGS += -ggdb  -D_GLIBCXX_DEBUG # debug flags
debug: $(TARGET)

$(TARGET):	$(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(LFO_TARGET): $(LFO_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

DEPS = $(OBJS:%.o=%.d)
-include $(DEPS)

clean:
	-rm $(TARGET) $(OBJS) $(DEPS)
