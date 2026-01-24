#ifndef UTILS_INCLUDE__H
#define UTILS_INCLUDE__H

#include <iostream>
#include <sstream>
#include <Windows.h>

#include <Tools/Header/PrimitiveTypes.h>

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
	
	template<typename T>
	inline void DebugInfo(const T& message)
	{
		std::wostringstream os;
		os << L"[INFO]: " << message << L"\n";
		OutputDebugStringW(os.str().c_str());
	}

	template<typename T, typename... Args>
	inline void DebugInfo(const T& first, const Args&... args)
	{
		std::wostringstream os;
		os << L"[INFO]: " << first;
		((os << args), ...);
		os << L"\n";
		OutputDebugStringW(os.str().c_str());
	}
#pragma endregion

#pragma region Debug
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			Utils::DebugError("HRESULT failed with code: ", static_cast<int>(hr));
			throw std::runtime_error("HRESULT failed");
		}
	}
#pragma endregion

#pragma region Mathematical Tools
	inline float Lerp(float start, float end, float time)
	{
		return start + time * (end - start);
	}
#pragma endregion

#pragma region Flags
	
	enum class PSOFlags : uint32
	{
		None = 0,
		DepthEnable = 1 << 0,
		DepthWrite = 1 << 1,
		AlphaBlend = 1 << 2,
		Wireframe = 1 << 3,
		CullNone = 1 << 4,
		CullFront = 1 << 5,
		PostProcess = 1 << 6
	};

	inline PSOFlags operator|(PSOFlags a, PSOFlags b)
	{
		return static_cast<PSOFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
	}

	inline PSOFlags operator&(PSOFlags a, PSOFlags b)
	{
		return static_cast<PSOFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
	}

	inline bool HasFlag(PSOFlags value, PSOFlags flag)
	{
		return (value & flag) != PSOFlags::None;
	}


#pragma endregion

}


#endif // !UTILS_INCLUDE__H
