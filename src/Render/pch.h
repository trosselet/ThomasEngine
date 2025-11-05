#ifndef RENDER_PCH_H
#define RENDER_PCH_H

#include "../Tools/pch.h"
//#include "../Engine/pch.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3dcompiler.lib")


//#include <fbxsdk.h>
//
//#ifdef _DEBUG
//#pragma comment(lib, "libfbxsdk-mt.lib")
//#pragma comment(lib, "zlib-mt.lib")
//#pragma comment(lib, "libxml2-mt.lib")
//#else
//#pragma comment(lib, "libfbxsdk-mt.lib")
//#pragma comment(lib, "zlib-mt.lib")
//#pragma comment(lib, "libxml2-mt.lib")
//#endif

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include "../Tools/Header/PrimitiveTypes.h"
#include "../Tools/Header/Utils.h"
#include "../Tools/Header/Color.h"

#endif