CXX = g++
CXXFLAGS = -Wall -O2
PICFLAGS = -fPIC

LIBNAME = mergesort
STATICLIB = lib$(LIBNAME).a
SHAREDLIB = lib$(LIBNAME).so

LIBSRC = Merge.cpp Mergesort.cpp PrintArray.cpp
LIBOBJ = Merge.o Mergesort.o PrintArray.o
PICOBJ = Merge.pic.o Mergesort.pic.o PrintArray.pic.o

TARGET_STATIC = main_static
TARGET_SHARED = main_shared

.PHONY: all clean static shared

all: static shared

static: $(TARGET_STATIC)

shared: $(TARGET_SHARED)

$(TARGET_STATIC): main.o $(STATICLIB)
	$(CXX) -o $@ main.o $(STATICLIB)

$(TARGET_SHARED): main.o $(SHAREDLIB)
	$(CXX) -o $@ main.o -L. -l$(LIBNAME)

$(STATICLIB): $(LIBOBJ)
	ar -rcs $@ $(LIBOBJ)

$(SHAREDLIB): $(PICOBJ)
	$(CXX) -shared -o $@ $(PICOBJ)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

Merge.o: Merge.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

Mergesort.o: Mergesort.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

PrintArray.o: PrintArray.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

Merge.pic.o: Merge.cpp
	$(CXX) $(CXXFLAGS) $(PICFLAGS) -c $< -o $@

Mergesort.pic.o: Mergesort.cpp
	$(CXX) $(CXXFLAGS) $(PICFLAGS) -c $< -o $@

PrintArray.pic.o: PrintArray.cpp
	$(CXX) $(CXXFLAGS) $(PICFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a *.so main_static main_shared
