#pragma once

// clang-format off

#ifndef EPOLL_WEB_BEGIN_NAMESPACE
#define EPOLL_WEB_BEGIN_NAMESPACE \
	namespace epoll_web \
	{                         \
	inline namespace v0       \
	{

#define EPOLL_WEB_END_NAMESPACE   \
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
