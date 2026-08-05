HIPCC = hipcc
HIPCCFLAGS  = -O3 -std=c++11 --offload-arch=gfx1201

LDFLAGS = -lpng -lz

TARGETS = sobel-amd

sobel-amd: sobel.hip
	$(HIPCC) $(HIPCCFLAGS) $(LDFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -f $(TARGETS)

