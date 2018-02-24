#CXX=g++
CXX=pgc++
CC=pgcc
GCCFLAG=-march=native -O3
PGIFLAG=-tp=haswell -O3

VORO_INSTALL=/home/shaomeng/Build/voro++-0.4.6/install

SRCV2=voro2densityV2.cpp
SRCV3=voro2densityV3.cpp


voro2densityv3: $(SRCV3)
	$(CXX) -std=c++11 $(PGIFLAG) -c -o pgi_bin/voro2density.o $(SRCV3) 
	$(CXX) -o pgi_bin/voro2density pgi_bin/voro2density.o pgi_bin/kdtree.o -lm -lpthread

voro2densityv2: $(SRCV3)
	$(CXX) -std=c++11 $(GCCFLAG) -I$(VORO_INSTALL)/include -L$(VORO_INSTALL)/lib -o bin/voro2density $(SRCV2) -lvoro++ -lm

kdtree: kdtree/kdtree.h kdtree/kdtree.c
	$(CC) -DUSE_LIST_NODE_ALLOCATOR -c -O3 -tp=haswell -o pgi_bin/kdtree.o kdtree/kdtree.c -lpthread
