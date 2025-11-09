#ifndef FBXFACTORY_INCLUDED__H
#define FBXFACTORY_INCLUDED__H

#include <vector>
#include <string>

struct Geometry;
class Color;

class FbxFactory
{
public:
	static Geometry* LoadFbxFile(const char* filePath, Color color);
};


#endif // !FBXFACTORY_INCLUDED__H