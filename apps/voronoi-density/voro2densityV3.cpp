#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <sys/time.h>
#include <omp.h>
#include <cassert>

#include "kdtree/kdtree.h"

/* 
   This version differs from V1 and V2 in that this one does not actually calculate voronoi cells.
   Instead, it find the nearest particle for each grid point, using a kdtree.
*/

#define GRIDX 1024
#define GRIDY 1024
#define GRIDZ 1024
#define EXTX  (M_PI*2.0)
#define EXTY  (M_PI*2.0)
#define EXTZ  (M_PI*2.0)

const float factorX = EXTX / (float)GRIDX;
const float factorY = EXTY / (float)GRIDY;
const float factorZ = EXTZ / (float)GRIDZ;

const long int totalGridPts = (GRIDX) * (GRIDY) * (GRIDZ);

double GetElapsedSeconds( const struct timeval* begin, 
                          const struct timeval* end )
{
    return (end->tv_sec - begin->tv_sec) + ((end->tv_usec - begin->tv_usec)/1000000.0);
}

void ReadParticles2( const char* name,       // input
                     long int&   len,        // number of particles
                     float**     buf )
{
    FILE* f     = fopen( name, "r" );
    float tmp[2];
    long int rt = fread( tmp, sizeof(float), 2, f );
    len         = tmp[0];
    *buf        = new float[len*3];
    rt          = fread( *buf, sizeof(float), len*3, f );
    fclose( f );
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
    long nParticles;
    float*   ptcBuf = NULL;
    ReadParticles2( argv[1], nParticles, &ptcBuf );
    long nPtcToUse = nParticles;           // use a subset of particles for experiments

    // Each particle has a counter 
    int* counter = new int[ nPtcToUse ];
    for( long i = 0; i < nPtcToUse; i++ )
        counter[i] = 0;

    // Each grid point has a pointer.
    // This pointer points to a counter.
    // This counter belongs to the voronoi cell this grid point resides in.
    int** pcounter = new int*[ totalGridPts ];
    for( long i = 0; i < totalGridPts; i++ )
        pcounter[i] = NULL;

    // Build a kd-tree for these particles
    gettimeofday( &start, NULL );
    struct kdtree* kd = kd_create(3);   
    for( long i = 0; i < nPtcToUse; i++ )
    {
        int rt = kd_insertf( kd, ptcBuf + i * 3, &counter[i] );
        assert( rt == 0 );
    }
    gettimeofday( &end, NULL );
    std::cerr << "kdtree construction takes " << GetElapsedSeconds(&start, &end) << " seconds." << std::endl;
    
    // Find the closest particle for each grid point (in parallel)
    gettimeofday( &start, NULL );
    for( long z = 0; z < GRIDZ; z++ ) //for( long z = 200; z < 400; z+=20 )
    {
        struct timeval planeStart, planeEnd;
        gettimeofday( &planeStart, NULL );
        long zOffset = z * GRIDX * GRIDY;
        #pragma omp parallel for
        for( long y = 0; y < GRIDY; y++ )
        {
            struct timeval lineStart, lineEnd;
            gettimeofday( &lineStart, NULL );
            long yOffset = y * GRIDX + zOffset;
            for( long x = 0; x < GRIDX; x++ )
            {
                //struct kdres* set = kd_nearest3f( kd, (float)x, (float)y, (float)z );
                struct kdres* set = kd_nearest3f( kd, x * factorX, y * factorY, z * factorZ );
                assert( kd_res_size( set ) == 1 );
                pcounter[ x+yOffset ] = (int*)kd_res_item_data( set );
                kd_res_free( set );
            }
            if( y % 10 == 0 )
            {
                gettimeofday( &lineEnd, NULL );
                std::cerr << "kdtree retrieval line " << y << " takes " << GetElapsedSeconds(&lineStart, &lineEnd) << " seconds." << std::endl;
            }
        }   // end parallel for
        gettimeofday( &planeEnd, NULL );
        std::cerr << std::endl << "kdtree retrieval plane " << z << " takes " << GetElapsedSeconds(&planeStart, &planeEnd) << " seconds." << std::endl << std::endl;
    }
    gettimeofday( &end, NULL );
    std::cerr << "total kdtree retrieval takes " << GetElapsedSeconds(&start, &end) << " seconds." << std::endl;

    // Increase counters in serial
    for( long i = 0; i < totalGridPts; i++ )
        if(    pcounter[i] != NULL )
            (*(pcounter[i]))++;

/**** print diagnostic info ****/
#if 0
    // What's the total count all counters have?
    int total = 0;
    for( long i = 0; i < nPtcToUse; i++ )
        total += counter[i];
    std::cout << "Total count is : " << total << std::endl;

    // How many pointers to counters are non-NULL?
    total = 0;
    for( int i = 0; i < totalGridPts; i++ )
        if( pcounter[i] != NULL )
            total++;
    std::cout << "Total non-NULL pointer is : " << total << std::endl;

    // Print some random counters
    for( int i = 0; i < 10; i++ )
    {
        int idx = (float)rand() / RAND_MAX * nPtcToUse;
        std::cout << "A random counter value: " << counter[idx] << std::endl;
    }
#endif 
/**** finish printing diagnostic info ****/

    
    // Each grid point calculates its own density
    float* density = new float[totalGridPts];
    #pragma omp parallel for
    for( long z = 0; z < GRIDZ; z++ )   //for( long int z = 200; z < 400; z+=20)
    {
        long int zOffset = z * GRIDX * GRIDY;
        for( long int y = 0; y < GRIDY; y++ )
        {
            long int yOffset = y * GRIDX + zOffset;
            for( long int x = 0; x < GRIDX; x++ )
            {
                long   idx          = x+yOffset;
                int    count        = *(pcounter[idx]); 
                density[ idx ]      = 1.0f/(float)count;
            }
        }
    }

    // How many Voronoi cells do not contain a grid point?
    long int emptyCellCount = 0;
    for( long int i = 0; i < nPtcToUse; i++ )
        if( counter[i] == 0 )
        {
            emptyCellCount++;

            float particle[3];
            long  grid[3];
            for( int j = 0; j < 3; j++ )
            {
                particle[j] = ptcBuf[i*3+j];
                grid[j]     = std::lround( particle[j] );
            }
            density[ grid[2] * GRIDX * GRIDY + grid[1] * GRIDY + grid[0] ] += 1.0f;
            
            //std::cerr << "Found a Voronoi cell without a grid point: " << std::endl;
            //for( int j = 0; j < 3; j++ )
            //    std::cerr << "Particle coord = " << particle[j] << ";  Grid coord = " << grid[j] << std::endl; 
        }
    std::cerr << "percentage of voronoi cell without a grid point: " << 100.0f * emptyCellCount / nPtcToUse << std::endl;
    
    // Output the density field
    FILE* f = fopen( argv[2], "w" );
    size_t rt = fwrite( density, sizeof(float), totalGridPts, f );
    fclose( f );
    
    kd_free( kd );
    delete[] density;
    delete[] pcounter;
    delete[] counter;
    delete[] ptcBuf;
}

