CXX=g++
VORO_INSTALL=/home/shaomeng/Build/voro++-0.4.6/install

voro2density: voro2density.cpp
	$(CXX) -std=c++11 -O3 -I$(VORO_INSTALL)/include -L$(VORO_INSTALL)/lib -o bin/voro2density voro2density.cpp -lvoro++ -lm
