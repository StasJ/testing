#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sys/time.h>
#include <cassert>
#include <cstring>

#include <omp.h>

/* 
   This version differs from the previous ones in that this one uses a kdtree implementation 
   named nanoflann.
*/

#include "kdtree/nanoflann.hpp"

#define GRIDX (1024)
#define GRIDY (1024)
#define GRIDZ (1024)

typedef unsigned int INT;

const INT totalGridPts = (GRIDX) * (GRIDY) * (GRIDZ);
const float extX       = M_PI*2.0;
const float extY       = M_PI*2.0;
const float extZ       = M_PI*2.0;

double GetElapsedSeconds( const struct timeval* begin, 
                          const struct timeval* end )
{
    return (end->tv_sec - begin->tv_sec) + ((end->tv_usec - begin->tv_usec)/1000000.0);
}

float CalcDist2( const float* p, const INT* q )  // two points
{
    return ( (p[0]-(float)q[0]) * (p[0]-(float)q[0]) + 
             (p[1]-(float)q[1]) * (p[1]-(float)q[1]) + 
             (p[2]-(float)q[2]) * (p[2]-(float)q[2]) );
}

float CalcLen( const float* p )                 // one point
{
    return std::sqrt( p[0]*p[0] + p[1]*p[1] + p[2]*p[2] );
}

void ReadParticles2( const char* name,       // input:  filename
                     INT&        len,        // output: number of particles
                     float**     buf )       // output: data read from file
{
    FILE*  f    = fopen( name, "r" );
    float  tmp[2];
    size_t rt   = fread( tmp, sizeof(float), 2, f );
    len         = (INT)tmp[0];
    *buf        = new float[len*3];
    rt          = fread( *buf, sizeof(float), len*3, f );
    fclose( f );
}

void FillHelicity( const char* xname,      // input:  location filename, which starts with `xlg`
                   INT         len,        // input:  number of particles
                   float*      buf )       // output: data read from file
{
    char   vname[256], wname[256];
    strcpy( vname, xname );
    strcpy( wname, xname );
    char*  p    = strstr( vname, "xlg" );
    assert( p );
    *p          = 'v';
    p           = strstr( wname, "xlg" );
    assert( p );
    *p          = 'w';

    float* vbuf = new float[len*3];
    FILE*  vf   = fopen( vname, "r" );
    float  tmp[2];
    size_t rt   = fread( tmp, sizeof(float), 2, vf );
    assert( len == (INT)tmp[0] );
    rt          = fread( vbuf, sizeof(float), len*3, vf );
    fclose( vf );

    float* wbuf = new float[len*3];
    FILE*  wf   = fopen( wname, "r" );
    rt          = fread( tmp, sizeof(float), 2, wf );
    assert( len == (INT)tmp[0] );
    rt          = fread( wbuf, sizeof(float), len*3, wf );
    fclose( wf );

    for( INT i = 0; i < len; i++ )
    {
        buf[i] = vbuf[i*3  ] * wbuf[i*3  ] +
                 vbuf[i*3+1] * wbuf[i*3+1] +
                 vbuf[i*3+2] * wbuf[i*3+2];
        // uncomment the following line to normalize the helicity
        // buf[i] /= CalcLen( vbuf + i*3 ) * CalcLen( wbuf + i*3 );
    }
    
    delete[] wbuf;
    delete[] vbuf;
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

    void FillByArray( const INT N, const T* buf )
    {
        pts.resize(N);
        for (INT i = 0; i < N; i++)
        {
            INT idx  = i*3;
            pts[i].x = buf[idx]   / extX * (GRIDX-1);
            pts[i].y = buf[idx+1] / extY * (GRIDY-1);
            pts[i].z = buf[idx+2] / extZ * (GRIDZ-1); 
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
    INT       nParticles;
    float*    ptcBuf = NULL;
    ReadParticles2( argv[1], nParticles, &ptcBuf );
    INT       nPtcToUse = nParticles;           // use a subset of particles for experiments

    // Put particles in a "PointCloud"
	PointCloud<float> cloud;
    cloud.FillByArray( nPtcToUse, ptcBuf );

	// construct a kd-tree index:
	typedef nanoflann::KDTreeSingleIndexAdaptor< nanoflann::L2_Simple_Adaptor<float, PointCloud<float> > ,
                                      PointCloud<float>, 3 >     my_kd_tree_t;
	my_kd_tree_t   kd_index(3, cloud, nanoflann::KDTreeSingleIndexAdaptorParams(8 /* max leaf */) );
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
    gettimeofday( &start, NULL );
    #pragma omp parallel for
    for( INT z = 0; z < GRIDZ; z++ ) 
    {
        nanoflann::KNNResultSet<float, INT> resultSet(1);
        INT    ret_index;
        float  out_dist_sqr;
        #ifdef DEBUG
        struct timeval planeStart, planeEnd;
        gettimeofday( &planeStart, NULL );
        #endif
        INT zOffset = z * GRIDX * GRIDY;
        for( INT y = 0; y < GRIDY; y++ )
        {
            INT yOffset = y * GRIDX + zOffset;
            for( INT x = 0; x < GRIDX; x++ )
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
    for( INT i = 0; i < totalGridPts; i++ )
        counter[pcounter[i]]++;


    #ifdef DEBUG  /**** print diagnostic info ****/
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
    std::cout << "Total grid points live in the cell of particle #0: " << total << std::endl;

    // Print some random counters
    for( INT i = 0; i < 10; i++ )
    {
        INT idx = (float)rand() / RAND_MAX * nPtcToUse;
        std::cout << "A random counter value: " << counter[idx] << std::endl;
    }
    #endif  /**** finish printing diagnostic info ****/

    
    // Each grid point calculates its own density from either a mass of 1.0,
    //   or its helicity.
    float* contribution = new float[nPtcToUse];
    if( use_helicity )
    {
        std::cout << "Distributing helicity, instead of mass, of the particles!" << std::endl;
        FillHelicity( argv[1], nPtcToUse, contribution );
    }
    else
    {
        for( INT i = 0; i < nPtcToUse; i++ )
            contribution[i] = 1.0f;
    }

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
                INT   count        = counter     [ pcounter[idx] ];
                density[ idx ]     = contribution[ pcounter[idx] ] / (float)count;
            }
        }
    }

    // How many Voronoi cells do not contain a grid point?
    INT emptyCellCount = 0;
    for( INT i = 0; i < nPtcToUse; i++ )
    {
        if( counter[i] == 0 )
        {
            emptyCellCount++;
            float c = contribution[i];

            // Distribute the contribution of this particle to its eight enclosing grid points.
            float ptc[3] = { ptcBuf[i*3  ] / extX * (GRIDX-1),
                             ptcBuf[i*3+1] / extY * (GRIDY-1),
                             ptcBuf[i*3+2] / extZ * (GRIDZ-1) };
            INT g0[3] = { (INT)ptc[0], (INT)ptc[1], (INT)ptc[2] };  // grid indices
            if( g0[0] == GRIDX )   g0[0]--;
            if( g0[1] == GRIDY )   g0[1]--;
            if( g0[2] == GRIDZ )   g0[2]--;
            INT g1[3] = { g0[0]+1, g0[1]  , g0[2]   };
            INT g2[3] = { g0[0]  , g0[1]+1, g0[2]   };
            INT g3[3] = { g0[0]+1, g0[1]+1, g0[2]   };
            INT g4[3] = { g0[0]  , g0[1]  , g0[2]+1 };
            INT g5[3] = { g0[0]+1, g0[1]  , g0[2]+1 };
            INT g6[3] = { g0[0]  , g0[1]+1, g0[2]+1 };
            INT g7[3] = { g0[0]+1, g0[1]+1, g0[2]+1 };
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
    std::cerr << "percentage of voronoi cell without a grid point: " << 100.0f * emptyCellCount / nPtcToUse << std::endl;
    
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

