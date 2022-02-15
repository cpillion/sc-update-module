CXX = g++

# compiler settings
LIBS := \
	-lsc \
	-lsc_utils \
	-lsc_utils_text \
	-lhps_core


LIBPATH := \
	-L../lib/macos \
	-L../bin/macos

INC := -I../include 
CXXFLAGS := -pthread -std=c++11 -fPIC
LDFLAGS := -pthread -Wl,-rpath,'$$ORIGIN' -Wl,$(BINDIR)

SAMPLE_SRCS := $(wildcard *.cpp)
SAMPLE_OBJECTS := $(SAMPLE_SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(INC) -o $@ $<

all: libsc_sample

# authoring_samples: $(SAMPLE_OBJECTS)
# 	$(CXX) $(SAMPLE_OBJECTS) -o authoring_samples $(LIBPATH) $(LIBS) $(LDFLAGS)

libsc_sample: main.o sc_store_sample.o 
	$(CXX) -o libsc_sample main.o sc_store_sample.o $(LIBPATH) $(LIBS) $(LDFLAGS)

clean:
	rm -f *.o libsc_sample

