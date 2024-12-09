#pragma once

// clang-format off

#ifndef BASIC_PROJECT_UNROLL_LOOP
#	if defined(_MSC_VER) && !defined(__clang__)
#		define BASIC_PROJECT_UNROLL_LOOP(N) __pragma(loop(unroll, N))
#	else
#		define BASIC_PROJECT_STRINGIFY(X)	 #X
#		define BASIC_PROJECT_TOSTRING(X)	 BASIC_PROJECT_STRINGIFY(X)
#		define BASIC_PROJECT_UNROLL_LOOP(N) _Pragma(BASIC_PROJECT_TOSTRING(GCC unroll N))
#	endif
#endif

#ifndef BASIC_PROJECT_BEGIN_NAMESPACE
#define BASIC_PROJECT_BEGIN_NAMESPACE \
	namespace basic_namespace \
	{                         \
	inline namespace v0       \
	{

#define BASIC_PROJECT_END_NAMESPACE   \
	}                         \
	}
#endif

#ifndef EPOLL_WEB_EXPORT
#	if  defined(EPOLL_WEB_LIBRARY_STATIC)
#		define EPOLL_WEB_EXPORT
#	elif defined(EPOLL_WEB_LIBRARY_SHARED)
#		if defined(_WIN32)
#			define EPOLL_WEB_EXPORT __declspec(dllexport)
#		else
#			define EPOLL_WEB_EXPORT __attribute__((visibility("default")))
#		endif
#	else // import
#		if defined(_WIN32)
#			define EPOLL_WEB_EXPORT __declspec(dllimport)
#		else
#			define EPOLL_WEB_EXPORT
#		endif
#	endif
#endif

// clang-format on
