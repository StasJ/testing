#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sys/time.h>
#include <cassert>

#include <omp.h>

/* 
   This version differs from the previous ones in that this one uses a kdtree implementation 
   named nanoflann.
*/

#include "kdtree/nanoflann.hpp"

#define GRIDX 1024
#define GRIDY 1024
#define GRIDZ 1024

typedef unsigned int INT;
typedef float FLOAT;

const INT totalGridPts = (GRIDX) * (GRIDY) * (GRIDZ);

double GetElapsedSeconds( const struct timeval* begin, 
                          const struct timeval* end )
{
    return (end->tv_sec - begin->tv_sec) + ((end->tv_usec - begin->tv_usec)/1000000.0);
}

void ReadParticles2( const char* name,       // input
                     INT&        len,        // number of particles
                     float**     buf )
{
    FILE* f     = fopen( name, "r" );
    float tmp[2];
    size_t   rt = fread( tmp, sizeof(float), 2, f );
    len         = (INT)tmp[0];
    *buf        = new float[len*3];
    rt          = fread( *buf, sizeof(float), len*3, f );
    fclose( f );
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
};

template <typename T>
void FillPointCloud(PointCloud<T> &point, const INT N, const T* buf )
{
    point.pts.resize(N);
    for (INT i = 0; i < N; i++)
    {
        INT idx = i*3;
        point.pts[i].x = buf[idx];
        point.pts[i].y = buf[idx+1];
        point.pts[i].z = buf[idx+2];
    }
}

int main(int argc, char** argv )
{
    if( argc != 3 )
    {
        printf("Usage: ./a.out .lag(input) .float(output)i\n");
        exit(1);
    }
    struct timeval start, end;

    // Read in particles
    INT       nParticles;
    float*    ptcBuf = NULL;
    ReadParticles2( argv[1], nParticles, &ptcBuf );
    INT       nPtcToUse = nParticles;           // use a subset of particles for experiments

    // Put particles in a "PointCloud"
	PointCloud<float> cloud;
    FillPointCloud( cloud, nPtcToUse, ptcBuf );

	// construct a kd-tree index:
	typedef nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor<float, PointCloud<float> > ,
                                      PointCloud<float>, 3 >     my_kd_tree_t;
	my_kd_tree_t   kd_index(3, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(15 /* max leaf */) );
    gettimeofday( &start, NULL );
	kd_index.buildIndex();
    gettimeofday( &end, NULL );
    std::cerr << "kdtree construction takes " << GetElapsedSeconds(&start, &end) 
              << " seconds." << std::endl;

    // Each particle has a counter 
    INT* counter = new INT[ nPtcToUse ];
    for( INT i = 0; i < nPtcToUse; i++ )
        counter[i] = 0;

    // Each grid point keeps which particle is closest to it.
    INT* pcounter = new INT[ totalGridPts ];
    for( INT i = 0; i < totalGridPts; i++ )
        pcounter[i] = 0;

    // Find the closest particle for each grid point 
    INT    ret_index;
    float  out_dist_sqr;
    nanoflann::KNNResultSet<float, INT> resultSet(1);
    resultSet.init( &ret_index, &out_dist_sqr );
    gettimeofday( &start, NULL );
    #pragma omp parallel for
    for( INT z = 0; z < GRIDZ; z++ ) 
    {
        #ifndef NDEBUG
        struct timeval planeStart, planeEnd;
        gettimeofday( &planeStart, NULL );
        #endif
        INT zOffset = z * GRIDX * GRIDY;
        for( INT y = 0; y < GRIDY; y++ )
        {
            INT yOffset = y * GRIDX + zOffset;
            for( INT x = 0; x < GRIDX; x++ )
            {
                float g[3]  = {(float)x, (float)y, (float)z};
                bool rt = kd_index.findNeighbors(resultSet, g, nanoflann::SearchParams());
                assert( rt );
                pcounter[ x+yOffset ] = ret_index;
            }
        }
        #ifndef NDEBUG
        gettimeofday( &planeEnd, NULL );
        std::cerr << "kdtree retrieval plane " << z << " takes " << GetElapsedSeconds(&planeStart, &planeEnd) 
                  << " seconds." << std::endl;
        #endif
    }
    gettimeofday( &end, NULL );
    std::cerr << "total kdtree retrieval takes " << GetElapsedSeconds(&start, &end) 
              << " seconds." << std::endl;

    // Increase counters in serial
    for( INT i = 0; i < totalGridPts; i++ )
        counter[pcounter[i]]++;


    #ifndef NDEBUG  /**** print diagnostic info ****/
    // What's the total count all counters have?
    INT total = 0;
    for( INT i = 0; i < nPtcToUse; i++ )
        total += counter[i];
    std::cout << "Total count is : " << total << std::endl;

    // How many grid points state particle 0 is their closest particle?
    total = 0;
    for( INT i = 0; i < totalGridPts; i++ )
        if( pcounter[i] == 0 )
            total++;
    std::cout << "Total grid points live in the cell of particle 0: " << total << std::endl;

    // Print some random counters
    for( INT i = 0; i < 10; i++ )
    {
        INT idx = (float)rand() / RAND_MAX * nPtcToUse;
        std::cout << "A random counter value: " << counter[idx] << std::endl;
    }
    #endif  /**** finish printing diagnostic info ****/

    
    // Each grid point calculates its own density
    float* density = new float[totalGridPts];
    #pragma omp parallel for
    for( INT z = 0; z < GRIDZ; z++ )   
    {
        INT zOffset = z * GRIDX * GRIDY;
        for( INT y = 0; y < GRIDY; y++ )
        {
            INT yOffset = y * GRIDX + zOffset;
            for( INT x = 0; x < GRIDX; x++ )
            {
                INT   idx          = x+yOffset;
                INT   count        = counter[ pcounter[idx] ];
                density[ idx ]     = 1.0f/(float)count;
            }
        }
    }

    // How many Voronoi cells do not contain a grid point?
    INT emptyCellCount = 0;
    for( INT i = 0; i < nPtcToUse; i++ )
        if( counter[i] == 0 )
        {
            emptyCellCount++;

            long  grid[3];
            for( int j = 0; j < 3; j++ )
                grid[j] = std::lround( ptcBuf[i*3+j] );
            density[ grid[2] * GRIDX * GRIDY + grid[1] * GRIDY + grid[0] ] += 1.0f;
        }
    //std::cerr << "percentage of voronoi cell without a grid point: " << 100.0f * emptyCellCount / nPtcToUse << std::endl;
    std::cerr << "number of voronoi cell without a grid point: " << emptyCellCount << std::endl;
    
    // Output the density field
    FILE* f = fopen( argv[2], "w" );
    size_t rt = fwrite( density, sizeof(float), totalGridPts, f );
    fclose( f );
    
    delete[] density;
    delete[] pcounter;
    delete[] counter;
    delete[] ptcBuf;
}

