#include <iostream>
#include <sstream>
#include <sys/stat.h>

#include <vapor/TMSTools.h>

bool IsTMSFile( std::string path ) {
    if ( path.rfind(".tms", path.size()-4) != std::string::npos ) {
        return true;
    }
    return false;
}

std::string TilePath( std::string file, size_t tileX, size_t tileY, int lod ) {
    // If we're given a file instead of a directory, remove the .tms extension
    //
    if ( file.rfind(".tms", file.size()-4) != std::string::npos ) {
        file.erase( file.length()-4, 4 );
    }

    size_t tmsTileY = tileY;

    std::stringstream oss;
    oss << file;
    oss << "/";
    oss << lod;
    oss << "/";
    oss << tileX;
    oss << "/";
    oss << tmsTileY;

    std::string base = oss.str();

    std::string path = base + ".tif";

    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) == 0)  return(path);

    path = base + ".tiff";

    if (stat(path.c_str(), &statbuf) == 0) return (path);

    // Tile does not exist
    //
    return("");
}

int GetAvailableTMSLODs( std::string file ) {
    int lod = 0;
    while ( TilePath( file, 0, 0, lod ) != "" ) {
        lod++;
    }
    return lod;
}
