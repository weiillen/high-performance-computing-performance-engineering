HIPCC = amdclang++
HIPCCFLAGS = -O3 -std=c++11 -fopenmp --offload-arch=gfx1201

LDFLAGS = -lpng -lz

TARGETS = sobel-omp sobel-opt-omp

sobel-omp: sobel.cc
	$(HIPCC) $(HIPCCFLAGS) $(LDFLAGS) -o $@ $<

sobel-opt-omp: sobel.cc
	$(HIPCC) $(HIPCCFLAGS) $(LDFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGETS)
