#ifndef UTILS_INCLUDE__H
#define UTILS_INCLUDE__H

#include <iostream>
#include <sstream>
#include <Windows.h>

namespace Utils
{
#define assert( condition ) if ( condition == false ) throw

#pragma region Log Debug
	template<typename T>
	inline void DebugLog(const T& message)
	{
		std::wostringstream os;
		os << L"[LOG]: " << message << L"\n";
		OutputDebugStringW(os.str().c_str());
	}

	template<typename T, typename... Args>
	inline void DebugLog(const T& first, const Args&... args)
	{
		std::wostringstream os;
		os << L"[LOG]: " << first;
		((os << args), ...);
		os << L"\n";
		OutputDebugStringW(os.str().c_str());
	}

	template<typename T>
	inline void DebugWarning(const T& message)
	{
		std::wostringstream os;
		os << L"[WARNING]: " << message << L"\n";
		OutputDebugStringW(os.str().c_str());
	}

	template<typename T, typename... Args>
	inline void DebugWarning(const T& first, const Args&... args)
	{
		std::wostringstream os;
		os << L"[WARNING]: " << first;
		((os << args), ...);
		os << L"\n";
		OutputDebugStringW(os.str().c_str());
	}

	template<typename T>
	inline void DebugError(const T& message)
	{
		std::wostringstream os;
		os << L"[ERROR]: " << message << L"\n";
		OutputDebugStringW(os.str().c_str());
	}

	template<typename T, typename... Args>
	inline void DebugError(const T& first, const Args&... args)
	{
		std::wostringstream os;
		os << L"[ERROR]: " << first;
		((os << args), ...);
		os << L"\n";
		OutputDebugStringW(os.str().c_str());
	}
#pragma endregion


#pragma region Mathematical Tools
	inline float Lerp(float start, float end, float time)
	{
		return start + time * (end - start);
	}
#pragma endregion

}


#endif // !UTILS_INCLUDE__H
