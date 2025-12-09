#ifndef OBJFACTORY_INCLUDED__H
#define OBJFACTORY_INCLUDED__H

#include <vector>
#include <string>

struct ObjModel;
class GraphicEngine;

class ObjFactory
{
public:
	static ObjModel* LoadObjFile(const char* filePath, Color color);
};


#endif // !OBJFACTORY_INCLUDED__H