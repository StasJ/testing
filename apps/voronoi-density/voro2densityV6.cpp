#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sys/time.h>
#include <cassert>
#include <cstring>

#include <omp.h>

/* 
   This version is a copy of V5, but adapted to read input from Bipin's data.
*/

#include "kdtree/nanoflann.hpp"

#define GRIDX (512)
#define GRIDY (512)
#define GRIDZ (512)

using UINT = unsigned int;
const UINT totalGridPts = (GRIDX) * (GRIDY) * (GRIDZ);

double GetElapsedSeconds( const struct timeval* begin, 
                          const struct timeval* end )
{
    return (end->tv_sec - begin->tv_sec) + ((end->tv_usec - begin->tv_usec)/1000000.0);
}

float CalcDist2( const float* p, const UINT* q )  // two points
{
    return ( (p[0]-(float)q[0]) * (p[0]-(float)q[0]) + 
             (p[1]-(float)q[1]) * (p[1]-(float)q[1]) + 
             (p[2]-(float)q[2]) * (p[2]-(float)q[2]) );
}

// Specialized method to 1) read in, and 2) process Bipin's data.
void ReadBipin( const char* name,       // input:  filename
                UINT&       len,        // output: number of particles
                float**     buf )       // output: (x, y, z) of each particle
{
    FILE*  f  = fopen( name, "r" );
    fseek( f, 0, SEEK_END );
    long int totalByte = ftell( f );
    assert( totalByte % 20 == 0 );      // Every 20 byte represents one particle
    len =   totalByte / 20;
    rewind( f );

    // First, read in all the values, each particle is represented by (id, x, y, z, r)
    float* inputBuf = new float[ totalByte / 4 ];
    size_t rt = fread( inputBuf, 4, totalByte / 4, f );
    fclose( f );
    assert( rt == totalByte / 4 );

    // Second, extract all the (x, y, z) tuples
    // !! For a grid size of 512, we also multiply the coordinate (x, y, z) by 4 !!
    *buf = new float[ len * 3 ];
    for( UINT i = 0; i < len; i++ )
    {
        memcpy( (*buf) + i * 3, inputBuf + i * 5 + 1, sizeof(float) * 3 );
        for( int j = 0; j < 3; j++ )
            (*buf)[ i * 3 + j ] *= 5.0f;
    }
    
    delete[] inputBuf;
}

template <typename T>
class  PointCloud
{
public:
	struct Point
	{
		T  x,y,z;
	};

	std::vector<Point>  pts;

	// Must return the number of data points
	inline size_t kdtree_get_point_count() const { return pts.size(); }

	// Returns the dim'th component of the idx'th point in the class:
	// Since this is inlined and the "dim" argument is typically an immediate value, the
	//  "if/else's" are actually solved at compile time.
	inline T kdtree_get_pt(const size_t idx, int dim) const
	{
		if (dim == 0) return pts[idx].x;
		else if (dim == 1) return pts[idx].y;
		else return pts[idx].z;
	}

	// Optional bounding-box computation: return false to default to a standard bbox computation loop.
	//   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
	//   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
	template <class BBOX>
	bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }

    void FillByArray( const UINT N, const T* buf )
    {
        pts.resize(N);
        UINT idx = 0;
        for (UINT i = 0; i < N; i++)
        {
            pts[i].x = buf[idx++];
            pts[i].y = buf[idx++];
            pts[i].z = buf[idx++];
        }
    }
};


int main(int argc, char** argv )
{
    if( argc < 3 || argc > 4 )  // has to be either 3 or 4
    {
        printf("Usage: ./a.out .lag(input) .float(output) use_helicity(optional)\n");
        exit(1);
    }
    bool use_helicity = false;
    if( argc == 4 )
        use_helicity  = true;
    struct timeval start, end;

    // Read in particles
    UINT      nParticles;
    float*    ptcBuf = nullptr;
    ReadBipin( argv[1], nParticles, &ptcBuf );
    UINT      nPtcToUse = nParticles;           // use a subset of particles for experiments

    // Put particles in a "PointCloud"
	PointCloud<float> cloud;
    cloud.FillByArray( nPtcToUse, ptcBuf );

	// construct a kd-tree index:
	typedef nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor<float, PointCloud<float> > ,
                                      PointCloud<float>, 3 >     my_kd_tree_t;
	my_kd_tree_t   kd_index(3 /* dimension */, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(8 /* max leaf */) );
    gettimeofday( &start, NULL );
	kd_index.buildIndex();
    gettimeofday( &end, NULL );
    std::cerr << "kdtree construction takes " << GetElapsedSeconds(&start, &end) 
              << " seconds." << std::endl;

    // Each particle has a counter 
    UINT* counter = new UINT[ nPtcToUse ];
    for( UINT i = 0; i < nPtcToUse; i++ )
        counter[i] = 0;

    // Each grid point keeps which particle is closest to it.
    UINT* pcounter = new UINT[ totalGridPts ];
    for( UINT i = 0; i < totalGridPts; i++ )
        pcounter[i] = 0;

    // Find the closest particle for each grid point 
    gettimeofday( &start, NULL );
    #pragma omp parallel for
    for( UINT z = 0; z < GRIDZ; z++ ) 
    {
        nanoflann::KNNResultSet<float, UINT> resultSet(1);
        UINT   ret_index;
        float  out_dist_sqr;
        #ifdef DEBUG
        struct timeval planeStart, planeEnd;
        gettimeofday( &planeStart, NULL );
        #endif
        UINT zOffset = z * GRIDX * GRIDY;
        for( UINT y = 0; y < GRIDY; y++ )
        {
            UINT yOffset = y * GRIDX + zOffset;
            for( UINT x = 0; x < GRIDX; x++ )
            {
                resultSet.init( &ret_index, &out_dist_sqr );    // VERY IMPORTANT!!!
                float g[3]  = {(float)x, (float)y, (float)z};
                bool rt = kd_index.findNeighbors(resultSet, g, nanoflann::SearchParams());
                assert( rt );
                pcounter[ x+yOffset ] = ret_index;
            }
        }
        #ifdef DEBUG
        gettimeofday( &planeEnd, NULL );
        std::cerr << "kdtree retrieval plane " << z << " takes " << GetElapsedSeconds(&planeStart, &planeEnd) 
                  << " seconds." << std::endl;
        #endif
    }
    gettimeofday( &end, NULL );
    std::cerr << "total kdtree retrieval takes " << GetElapsedSeconds(&start, &end) 
              << " seconds." << std::endl;

    // Increase counters in serial
    for( UINT i = 0; i < totalGridPts; i++ )
        counter[pcounter[i]]++;


    #ifdef DEBUG  /**** print diagnostic info ****/
    // What's the total count all counters have?
    UINT total = 0;
    for( UINT i = 0; i < nPtcToUse; i++ )
        total += counter[i];
    std::cout << "Total count is : " << total << std::endl;

    // How many grid points state particle 0 is their closest particle?
    total = 0;
    for( UINT i = 0; i < totalGridPts; i++ )
        if( pcounter[i] == 0 )
            total++;
    std::cout << "Total grid points live in the cell of particle #0: " << total << std::endl;

    // Print some random counters
    for( UINT i = 0; i < 10; i++ )
    {
        UINT idx = (float)rand() / RAND_MAX * nPtcToUse;
        std::cout << "A random counter value: " << counter[idx] << std::endl;
    }
    #endif  /**** finish printing diagnostic info ****/

    
    // Each grid point calculates its own density from either a mass of 1.0 or its helicity.
    float* contribution = new float[nPtcToUse];
    if( use_helicity )
    {
        std::cout << "Distributing helicity, instead of mass, of the particles!" << std::endl;
        //FillHelicity( argv[1], nPtcToUse, contribution );
    }
    else
    {
        for( UINT i = 0; i < nPtcToUse; i++ )
            contribution[i] = 1.0f;
    }

    float* density = new float[totalGridPts];
    #pragma omp parallel for
    for( UINT z = 0; z < GRIDZ; z++ )   
    {
        UINT zOffset = z * GRIDX * GRIDY;
        for( UINT y = 0; y < GRIDY; y++ )
        {
            UINT yOffset = y * GRIDX + zOffset;
            for( UINT x = 0; x < GRIDX; x++ )
            {
                UINT   idx         = x+yOffset;
                UINT   count       = counter     [ pcounter[idx] ];
                density[ idx ]     = contribution[ pcounter[idx] ] / (float)count;
            }
        }
    }

    // How many Voronoi cells do not contain a grid point?
    UINT emptyCellCount = 0;
    for( UINT i = 0; i < nPtcToUse; i++ )
    {
        if( counter[i] == 0 )
        {
            emptyCellCount++;
            float c = contribution[i];

            // Distribute the contribution of this particle to its eight enclosing grid points.
            float ptc[3] = { ptcBuf[i*3], ptcBuf[i*3+1], ptcBuf[i*3+2] };
            UINT g0[3] = { (UINT)ptc[0], (UINT)ptc[1], (UINT)ptc[2] };  // grid indices
            if( g0[0] == GRIDX )   g0[0]--;
            if( g0[1] == GRIDY )   g0[1]--;
            if( g0[2] == GRIDZ )   g0[2]--;
            UINT g1[3] = { g0[0]+1, g0[1]  , g0[2]   };
            UINT g2[3] = { g0[0]  , g0[1]+1, g0[2]   };
            UINT g3[3] = { g0[0]+1, g0[1]+1, g0[2]   };
            UINT g4[3] = { g0[0]  , g0[1]  , g0[2]+1 };
            UINT g5[3] = { g0[0]+1, g0[1]  , g0[2]+1 };
            UINT g6[3] = { g0[0]  , g0[1]+1, g0[2]+1 };
            UINT g7[3] = { g0[0]+1, g0[1]+1, g0[2]+1 };
            float dist[8] = { CalcDist2( ptc, g0 ), CalcDist2( ptc, g1 ), 
                              CalcDist2( ptc, g2 ), CalcDist2( ptc, g3 ), 
                              CalcDist2( ptc, g4 ), CalcDist2( ptc, g5 ), 
                              CalcDist2( ptc, g6 ), CalcDist2( ptc, g7 ) };
            float total = 0.0f;
            for( int j = 0; j < 8; j++ )
                total += dist[j];

            density[ g0[2] * GRIDX * GRIDY + g0[1] * GRIDY + g0[0] ] += c * dist[0] / total;
            density[ g1[2] * GRIDX * GRIDY + g1[1] * GRIDY + g1[0] ] += c * dist[1] / total;
            density[ g2[2] * GRIDX * GRIDY + g2[1] * GRIDY + g2[0] ] += c * dist[2] / total;
            density[ g3[2] * GRIDX * GRIDY + g3[1] * GRIDY + g3[0] ] += c * dist[3] / total;
            density[ g4[2] * GRIDX * GRIDY + g4[1] * GRIDY + g4[0] ] += c * dist[4] / total;
            density[ g5[2] * GRIDX * GRIDY + g5[1] * GRIDY + g5[0] ] += c * dist[5] / total;
            density[ g6[2] * GRIDX * GRIDY + g6[1] * GRIDY + g6[0] ] += c * dist[6] / total;
            density[ g7[2] * GRIDX * GRIDY + g7[1] * GRIDY + g7[0] ] += c * dist[7] / total;
        }
    }
    std::cerr << "percentage of voronoi cell without a grid point: " 
              << 100.0f * emptyCellCount / nPtcToUse << std::endl;
    
    // Output the density field
    gettimeofday( &start, NULL );
    FILE* f = fopen( argv[2], "w" );
    size_t rt = fwrite( density, sizeof(float), totalGridPts, f );
    fclose( f );
    gettimeofday( &end, NULL );
    std::cerr << "writing density fields takes " << GetElapsedSeconds(&start, &end) 
              << " seconds." << std::endl;
    
    delete[] density;
    delete[] contribution;
    delete[] pcounter;
    delete[] counter;
    delete[] ptcBuf;
}

