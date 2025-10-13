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

private:
	static std::vector<std::string> SplitString(const std::string& input, char separator);
};


#endif // !OBJFACTORY_INCLUDED__H