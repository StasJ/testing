#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <iostream>

#include "voro++/voro++.hh"

#define GRIDX 256
#define GRIDY 256
#define GRIDZ 256

const long int totalGridPts = (GRIDX) * (GRIDY) * (GRIDZ);

const float ptcXMin = 0.0;
const float ptcXMax = 550.0;
const float ptcYMin = 0.0;
const float ptcYMax = 550.0;
const float ptcZMin = 0.0;
const float ptcZMax = 550.0;

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


int main(int argc, char** argv )
{
    if( argc != 3 )
    {
        printf("Usage: ./a.out .lag(input) .float(output)i\n");
        exit(1);
    }

    // read in particles
    long int nParticles;
    float*   ptcX;
    float*   ptcY;
    float*   ptcZ;
    ReadParticles( argv[1], nParticles, &ptcX, &ptcY, &ptcZ );


    // Use a Voro++ container 
    voro::container con( ptcXMin,       ptcXMax, 
                         ptcYMin,       ptcYMax,
                         ptcZMin,       ptcZMax,
                         10,     10,     10,
                         false, false, false, 100 );
    long int nPtcToUse = nParticles;           // use a subset of particles for experiments
    for( long int i = 0; i < nPtcToUse; i++ )
        con.put( i, ptcX[i], ptcY[i], ptcZ[i] );

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
                double gridX = (double)x * ptcXSpan / (double)(GRIDX) + ptcXMin;
                double gridY = (double)y * ptcYSpan / (double)(GRIDY) + ptcYMin;
                double gridZ = (double)z * ptcZSpan / (double)(GRIDZ) + ptcZMin;
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
        int idx = (int)density[i];
        density[i] = 1.0f / (double)gridPtCount[idx];
    }

    // How many Voronoi cells do not contain a grid point?
    long int emptyCellCount = 0;
    for( long int i = 0; i < nPtcToUse; i++ )
        if( gridPtCount[i] == 0 )
            emptyCellCount++;
    std::cerr << "Number of Voronoi cells without a grid point: " << emptyCellCount << std::endl;
        

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
    delete[] ptcX;
    delete[] ptcY;
    delete[] ptcZ;
}

