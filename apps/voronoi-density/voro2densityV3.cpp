#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sys/time.h>

#include "kdtree/kdtree.h"

/* 
   This version differs from V1 and V2 in that this one does not actually calculate voronoi cells.
   Instead, it find the nearest particle for each grid point, using a kdtree.
*/

#define GRIDX 1024
#define GRIDY 1024
#define GRIDZ 1024

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

    // read in particles
    long int nParticles;
    float*   ptcBuf = NULL;
    ReadParticles2( argv[1], nParticles, &ptcBuf );
    long int nPtcToUse = nParticles;           // use a subset of particles for experiments

    // each particle has a counter 
    long int* counter = new long int[ nPtcToUse ];
    for( long i = 0; i < nPtcToUse; i++ )
        counter[i] = 0;

    // build a kd-tree for these particles
    gettimeofday( &start, NULL );
    struct kdtree* kd = kd_create(3);   
    for( long i = 0; i < nPtcToUse; i++ )
    {
        int rt = kd_insertf( kd, ptcBuf + i * 3, &counter[i] );
        assert( rt == 0 );
    }
    gettimeofday( &end, NULL );
    std::cerr << "kdtree construction takes " << GetElapsedSeconds(&start, &end) << " seconds." << std::endl;
    
    // Find the closest particle for each grid point
    gettimeofday( &start, NULL );
    for( long int z = 0; z < GRIDZ; z++ )
    {
        long int zOffset = z * GRIDX * GRIDY;
        for( long int y = 0; y < GRIDY; y++ )
        {
            long int yOffset = y * GRIDX + zOffset;
            for( long int x = 0; x < GRIDX; x++ )
            {
                struct kdres* set = kd_nearest3f( kd, (float)x, (float)y, (float)z );
                assert( kd_res_size( set ) == 1 );
                long* pt = (long*)kd_res_item_data( set );
                (*pt)++;
                kd_res_free( set );
            }
        }
    }
    gettimeofday( &end, NULL );
    std::cerr << "kdtree retrieval takes " << GetElapsedSeconds(&start, &end) << " seconds." << std::endl;

    // Print some random counters
    for( int i = 0; i < 10; i++ )
    {
        int idx = (float)rand() / RAND_MAX * nPtcToUse;
        std::cout << "A random counter value: " << counter[idx] << std::endl;
    }

    

#if 0
    // Each grid point calculates its own weight
    for( long int z = 0; z < GRIDZ; z++ )
    {
        long int zOffset = z * GRIDX * GRIDY;
        for( long int y = 0; y < GRIDY; y++ )
        {
            long int yOffset = y * GRIDX + zOffset;
            for( long int x = 0; x < GRIDX; x++ )
            {
                float  gridPt[3]     = {(float)x, (float)y, (float)z};
                int pid              = (int)density[ x+yOffset ];
                float myWeight       = CalcWeight( gridPt, ptcBuf + pid * 3 );
            }
        }
    }

    // How many Voronoi cells do not contain a grid point?
    long int emptyCellCount = 0;
    for( long int i = 0; i < nPtcToUse; i++ )
        if( ptcWeight[i] == 0.0f )
        {
            emptyCellCount++;

            float particle[3];
            for( int j = 0; j < 3; j++ )
                particle[j] = ptcBuf[i*3+j];
            long int grid[3];
            for( int j = 0; j < 3; j++ )
                grid[j] = std::lround( particle[j] );
            density[ grid[2] * GRIDX * GRIDY + grid[1] * GRIDY + grid[0] ] += 1.0;
            
            std::cerr << "Found a Voronoi cell without a grid point: " << std::endl;
            for( int j = 0; j < 3; j++ )
                std::cerr << "Particle coord = " << particle[j] << ";  Grid coord = " << grid[j] << std::endl; 
        }

    
    // Output the density field
    FILE* f = fopen( argv[2], "w" );
    size_t rt = fwrite( density, sizeof(float), totalGridPts, f );
    fclose( f );
#endif
    
    kd_free( kd );
    delete[] counter;
    if( ptcBuf )
        delete[] ptcBuf;
}

