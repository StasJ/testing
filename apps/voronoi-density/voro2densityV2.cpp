#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <iostream>
#include <sys/time.h>

#include "voro++/voro++.hh"

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

float CalcWeight( const float* p, const float* q )  // two points
{
    float dist2 = (p[0]-q[0]) * (p[0]-q[0]) + 
                  (p[1]-q[1]) * (p[1]-q[1]) + 
                  (p[2]-q[2]) * (p[2]-q[2]);
    return 1.0f/std::sqrt( dist2 );
}

int main(int argc, char** argv )
{
    if( argc != 3 )
    {
        printf("Usage: ./a.out .lag(input) .float(output)i\n");
        exit(1);
    }

    // read in particles
    long int nParticles;
    float*   ptcBuf = NULL;
    ReadParticles2( argv[1], nParticles, &ptcBuf );


    // Use a Voro++ container 
    // A guide to choose the grid size is available at the bottom of this page:
    //   http://math.lbl.gov/voro++/doc/refman/
    voro::container con( 0.0,       (double)(GRIDX),
                         0.0,       (double)(GRIDY),
                         0.0,       (double)(GRIDZ),
                         64,        64,     64,
                         true,      true,   true, 12 );
    long int nPtcToUse = nParticles;           // use a subset of particles for experiments
    for( long int i = 0; i < nPtcToUse; i++ )
    {
        con.put(i, ptcBuf[i*3], ptcBuf[i*3+1], ptcBuf[i*3+2] );
    }

    std::cerr << "finish putting all particles, start calculating voronoi cells." << std::endl;
    struct timeval begin, end;
    gettimeofday( &begin, NULL );
    con.compute_all_cells();
    gettimeofday( &end, NULL );
    double sec = GetElapsedSeconds( &begin, &end );
    std::cerr << "finish calculating voronoi cells in " << sec << " Seconds." << std::endl;
    return 0;
    
    

    // What's the normalization factor for each voronoi cell?
    float* ptcWeight = new float[nPtcToUse];
    for( long int i = 0; i < nPtcToUse; i++ )
        ptcWeight[i] = 0.0;

    float* density = new float[ totalGridPts ];

    // Find the Voronoi cell for each grid point
    for( long int z = 0; z < GRIDZ; z++ )
    {
        long int zOffset = z * GRIDX * GRIDY;
        for( long int y = 0; y < GRIDY; y++ )
        {
            long int yOffset = y * GRIDX + zOffset;
            for( long int x = 0; x < GRIDX; x++ )
            {
                float  gridPt[3] = {(float)x, (float)y, (float)z};
                double rx, ry, rz;
                int    pid;             // particle ID
                //if( !con.find_voronoi_cell( gridPt[0], gridPt[1], gridPt[2], rx, ry, rz, pid ) )
                //    std::cerr << "didn't find a voronoi cell!" << std::endl;

                density[ x + yOffset ] = (float)pid;
                ptcWeight[ pid ] += CalcWeight( gridPt, ptcBuf + pid * 3 );
            }
        }
        //std::cerr << "finish z level " << z << std::endl;
    }

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
                density[ x+yOffset ] = myWeight / ptcWeight[pid];
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
    
    delete[] density;
    delete[] ptcWeight;
    if( ptcBuf )
        delete[] ptcBuf;
}

