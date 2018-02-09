#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <iostream>

#include "voro++/voro++.hh"

#define GRIDX 512
#define GRIDY 512
#define GRIDZ 512

const long int totalGridPts = (GRIDX) * (GRIDY) * (GRIDZ);

const float ptcXMin = 0.0;
const float ptcXMax = 512.0;
const float ptcYMin = 0.0;
const float ptcYMax = 512.0;
const float ptcZMin = 0.0;
const float ptcZMax = 512.0;

const float ptcXSpan = ptcXMax - ptcXMin;
const float ptcYSpan = ptcYMax - ptcYMin;
const float ptcZSpan = ptcZMax - ptcZMin;


void ReadParticles( const char* name,       // input
                    long int&   len,        // number of particles
                    float**     x,
                    float**     y,
                    float**     z )
{
    FILE* f     = fopen( name, "r" );
    float tmp[2];
    long int rt = fread( tmp, sizeof(float), 2, f );
    len         = tmp[0];

    *x          = new float[len];
    *y          = new float[len];
    *z          = new float[len];
    rt = fread( *x, sizeof(float), len, f );
    rt = fread( *y, sizeof(float), len, f );
    rt = fread( *z, sizeof(float), len, f );

    fclose( f );
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

    // read in particles
    long int nParticles;
    float*   ptcX   = NULL;
    float*   ptcY   = NULL;
    float*   ptcZ   = NULL;
    float*   ptcBuf = NULL;
    //ReadParticles( argv[1], nParticles, &ptcX, &ptcY, &ptcZ );
    ReadParticles2( argv[1], nParticles, &ptcBuf );


    // Use a Voro++ container 
    voro::container con( ptcXMin,       ptcXMax, 
                         ptcYMin,       ptcYMax,
                         ptcZMin,       ptcZMax,
                         10,     10,     10,
                         true,   true,   true, 100 );
    long int nPtcToUse = nParticles;           // use a subset of particles for experiments
    for( long int i = 0; i < nPtcToUse; i++ )
    {
        //con.put( i, ptcX[i], ptcY[i], ptcZ[i] );
        con.put(i, ptcBuf[i*3], ptcBuf[i*3+1], ptcBuf[i*3+2] );
    }

    // How many grid points in each Voronoi cell? 
    int* gridPtCount = new int[nPtcToUse];
    for( long int i = 0; i < nPtcToUse; i++ )
        gridPtCount[i] = 0;

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
                float  gridX = (float)x * ptcXSpan / (float)(GRIDX) + ptcXMin;
                float  gridY = (float)y * ptcYSpan / (float)(GRIDY) + ptcYMin;
                float  gridZ = (float)z * ptcZSpan / (float)(GRIDZ) + ptcZMin;
                double rx, ry, rz;
                int    pid;             // particle ID
                if( !con.find_voronoi_cell( gridX, gridY, gridZ, rx, ry, rz, pid ) )
                    std::cerr << "didn't find a voronoi cell!" << std::endl;

                gridPtCount[ pid ]++;
                density[ x + yOffset ] = (float)pid;
            }
        }
    }

    // Each grid point calculates its own weight
    for( long int i = 0; i < totalGridPts; i++ )
    {
        int ptcIdx = (int)density[i];
        density[i] = 1.0f / (float)gridPtCount[ptcIdx];
    }

    // How many Voronoi cells do not contain a grid point?
    long int emptyCellCount = 0;
    for( long int i = 0; i < nPtcToUse; i++ )
        if( gridPtCount[i] == 0 )
        {
            emptyCellCount++;

            float particle[3], grid[3];
            for( int j = 0; j < 3; j++ )
                particle[j] = ptcBuf[i*3+j];
            grid[0] = (particle[0] - ptcXMin) / ptcXSpan * (float)GRIDX;
            grid[1] = (particle[1] - ptcYMin) / ptcYSpan * (float)GRIDY;
            grid[2] = (particle[2] - ptcZMin) / ptcZSpan * (float)GRIDZ;
            long int lgrid[3];
            for( int j = 0; j < 3; j++ )
                lgrid[j] = std::lround( grid[j] );
            density[ lgrid[2] * GRIDX * GRIDY + lgrid[1] * GRIDY + lgrid[0] ] += 1.0;
            
            std::cerr << "Found a Voronoi cell without a grid point: " << std::endl;
            for( int j = 0; j < 3; j++ )
                std::cerr << "Particle coord = " << particle[j] << ";  Grid coord = " << lgrid[j] << std::endl; 
        }
        

    // Output for gnuplot. The gnuplot commands to use:
    //   splot 'output/density_c.gnu' w l t 'Voronoi cells', \
    //         'output/density_p.gnu' u 2:3:4 w p t 'Particles'
    // con.draw_particles("output/density_p.gnu");
    // con.draw_cells_gnuplot("output/density_c.gnu");

    
    // Output the density field
    FILE* f = fopen( argv[2], "w" );
    size_t rt = fwrite( density, sizeof(float), totalGridPts, f );
    fclose( f );
    
    delete[] density;
    delete[] gridPtCount;
    if( ptcX )
        delete[] ptcX;
    if( ptcY )
        delete[] ptcY;
    if( ptcZ )
        delete[] ptcZ;
    if( ptcBuf )
        delete[] ptcBuf;
}

