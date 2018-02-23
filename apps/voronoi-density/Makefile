#CXX=g++
CXX=pgc++
CC=pgcc
GCCFLAG=-march=native -O3
PGIFLAG=-tp=haswell -g

VORO_INSTALL=/home/shaomeng/Build/voro++-0.4.6/install

SRCV2=voro2densityV2.cpp
SRCV3=voro2densityV3.cpp


voro2densityv3: $(SRCV3)
	$(CXX) -std=c++11 $(PGIFLAG) -o pgi_bin/voro2density $(SRCV3) pgi_bin/kdtree.o -lm -lpthread

voro2densityv2: $(SRCV3)
	$(CXX) -std=c++11 $(GCCFLAG) -I$(VORO_INSTALL)/include -L$(VORO_INSTALL)/lib -o bin/voro2density $(SRCV2) -lvoro++ -lm
