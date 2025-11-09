#ifndef OBJFACTORY_INCLUDED__H
#define OBJFACTORY_INCLUDED__H

#include <vector>
#include <string>

struct Geometry;
class Color;

class ObjFactory
{
public:
	static Geometry* LoadObjFile(const char* filePath, Color color);
};


#endif // !OBJFACTORY_INCLUDED__H