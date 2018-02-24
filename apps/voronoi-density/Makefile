CXX=clang++
CC=clang

GCCOPTIM=-march=haswell -O3
PGIOPTIM=-tp=haswell -O3
FLAG=$(GCCOPTIM)

BINDIR=bin

VORO_INSTALL=/home/shaomeng/Build/voro++-0.4.6/install

SRCV2=voro2densityV2.cpp
SRCV3=voro2densityV3.cpp


voro2density: kdtree.o $(SRCV3)
	$(CXX) -std=c++11 $(FLAG) -c -o $(BINDIR)/voro2density.o $(SRCV3) 
	$(CXX) -o $(BINDIR)/voro2density $(BINDIR)/voro2density.o $(BINDIR)/kdtree.o -lm -lpthread

kdtree.o: kdtree/kdtree.h kdtree/kdtree.c
	$(CC) -DUSE_LIST_NODE_ALLOCATOR -c $(FLAG) -o $(BINDIR)/kdtree.o kdtree/kdtree.c

voro2densityv2: $(SRCV3)
	$(CXX) -std=c++11 $(GCCFLAG) -I$(VORO_INSTALL)/include -L$(VORO_INSTALL)/lib -o bin/voro2density $(SRCV2) -lvoro++ -lm

