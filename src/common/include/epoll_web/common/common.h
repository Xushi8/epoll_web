#pragma once

// clang-format off
#ifdef	__cplusplus
#   define EPOLL_WEB_EXTERN_C_BEGIN	extern "C" {
#   define EPOLL_WEB_EXTERN_C_END	}
#else
#   define EPOLL_WEB_EXTERN_C_BEGIN
#   define EPOLL_WEB_EXTERN_C_END
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

EPOLL_WEB_EXTERN_C_BEGIN

EPOLL_WEB_EXPORT
void epoll_web_no_use(void);

EPOLL_WEB_EXTERN_C_END
