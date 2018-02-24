CXX=icpc
CC=icc

GCCOPTIM=-march=haswell -O3
PGIOPTIM=-tp=haswell-64 -O3 
INTOPTIM=-march=haswell -O3 
GCCOMP=-fopenmp
PGIOMP=-mp
INTOMP=-qopenmp
FLAG=$(INTOPTIM)
OMP=$(INTOMP)

BINDIR=intel_bin

VORO_INSTALL=/home/shaomeng/Build/voro++-0.4.6/install

SRCV2=voro2densityV2.cpp
SRCV3=voro2densityV3.cpp


voro2density: kdtree.o $(SRCV3)
	$(CXX) -std=c++11 $(FLAG) -c -o $(BINDIR)/voro2density.o $(SRCV3) $(OMP)
	$(CXX) -o $(BINDIR)/voro2density $(BINDIR)/voro2density.o $(BINDIR)/kdtree.o -lm $(OMP)

kdtree.o: kdtree/kdtree.h kdtree/kdtree.c
	$(CC) -c $(FLAG) -o $(BINDIR)/kdtree.o kdtree/kdtree.c

voro2densityv2: $(SRCV3)
	$(CXX) -std=c++11 $(GCCFLAG) -I$(VORO_INSTALL)/include -L$(VORO_INSTALL)/lib -o bin/voro2density $(SRCV2) -lvoro++ -lm

