CXX=g++
CC=gcc

GCCOPTIM=-march=haswell -O3 -DNDEBUG
PGIOPTIM=-tp=haswell-64 -O3 -DNDEBUG
INTOPTIM=-march=broadwell -O3 -DNDEBUG

GCCOMP=-fopenmp
PGIOMP=-mp
INTOMP=-qopenmp

BINDIR=bin
FLAG=$(GCCOPTIM)
OMP=$(GCCOMP)

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

v4: voro2densityV4.cpp
	$(CXX) -std=c++11 $(FLAG) -o $(BINDIR)/v4 voro2densityV4.cpp

v5: voro2densityV5.cpp
	$(CXX) -std=c++11 $(FLAG) -o $(BINDIR)/v5 voro2densityV5.cpp $(OMP)

v6: voro2densityV5.cpp
	$(CXX) -std=c++11 $(FLAG) -o $(BINDIR)/v5 voro2densityV5.cpp $(OMP)
