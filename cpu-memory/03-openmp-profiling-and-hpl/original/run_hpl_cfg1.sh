#!/bin/bash
mpirun -np 4 --host head:2,work1:2 --map-by ppr:2:node ./xhpl
