#pragma once
#include <string>

int GetAvailableTMSLODs( std::string file );

bool IsTMSFile( std::string path );

std::string TilePath( std::string file, size_t tileX, size_t tileY, int lod );
