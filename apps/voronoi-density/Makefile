CXX=g++
VORO_INSTALL=/home/shaomeng/Build/voro++-0.4.6/install

SRC=voro2densityV2.cpp

voro2density: $(SRC)
	$(CXX) -std=c++11 -O3 -march=native -I$(VORO_INSTALL)/include -L$(VORO_INSTALL)/lib -o bin/voro2density $(SRC) -lvoro++ -lm
