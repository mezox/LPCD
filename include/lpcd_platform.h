#ifndef LPCD_PLATFORM_H
#define LPCD_PLATFORM_H

#ifdef _WIN32
	#define LPCD_SYS_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>

	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
#elif defined (__linux__)
	#define LPCD_SYS_LINUX
#elif defined (__APPLE__)
	#define LPCD_SYS_MACOS
#else
	#error System not supported
#endif

#ifdef LPCD_SYS_WINDOWS
	#include <cstdint>
	typedef __int16 i16_t;
	typedef __int32 i32_t;
	
	//Visual c++ compiler warning C42251 disable
	#ifdef _MSC_VER
		#pragma warning(disable : 4251)
	
		#if __cplusplus <= 199711L
			#define CPP11_SUPPORTED
		#else
			#define CPP11_UNSUPPORTED
		#endif
			
	#endif
#elif defined(GE_SYS_LINUX) || defined(GE_SYS_MACOS)
	#ifdef __GNUC__ >= 4
		#include <cstdint>
		typedef int16_t i16_t;
		typedef int32_t i32_t;
	#endif
#endif

/*BYTE define for non Windows platforms*/
#ifndef _WIN32
	typedef unsigned char BYTE
#endif

#endif