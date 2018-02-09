#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define NX 128
#define NY 128
#define NZ 128

#define EXTENTX 550
#define EXTENTY 550
#define EXTENTZ 550

int main(int argc, char** argv )
{
	if( argc != 3 )
	{
		printf("Usage: ./a.out .lag(input) .float(output)i\n");
		exit(1);
	}

  FILE* f = fopen( argv[1], "r" );
  float tmp[2];
  fread( tmp, sizeof(float), 2, f );
  long int nVert = tmp[0];
  assert( nVert = 100000 );

	float* inBuf = (float*) malloc( sizeof(float) * nVert * 3 );
	long int rt = fread( inBuf, sizeof(float), nVert * 3, f );
  assert( rt == nVert * 3 );
  fclose( f );

	float* countBuf = (float*)malloc( sizeof(float) * NX*NY*NZ );
	for( long int i = 0; i < NX*NY*NZ; i++ )
		countBuf[i] = 0.0; 

	for( long int i = 0; i < nVert; i++ )
	{
		int x = (int)(inBuf[ i             ] * NX / EXTENTX);
		int y = (int)(inBuf[ nVert + i     ] * NY / EXTENTY);
		int z = (int)(inBuf[ 2 * nVert + i ] * NZ / EXTENTZ);
		countBuf[ z*NX*NY + y*NX + x ] += 1.0;
	}

	for( long int i = 0; i < NX*NY*NZ; i++ )
		countBuf[i] /= (float)nVert;

	f = fopen( argv[2], "w" );
	fwrite( countBuf, sizeof(float), NX*NY*NZ, f );
	fclose(f);

	free( countBuf );
  free( inBuf );
}

