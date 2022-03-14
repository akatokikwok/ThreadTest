#pragma once
#include "GenericPlatform/GenericPlatformProcess.h"

#if PLATFORM_WINDOWS
	#include "Windows/WindowsPlatformProcess.h"
#else
#if PLATFORM_LINUX
	#include "Linux/LinuxPlatformProcess.h"
#else
#if  PLATFORM_MAC
	#include "Mac/MacPlatformProcess.h"
#endif
#endif
#endif