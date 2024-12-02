if(EPOLL_WEB_ENABLE_STRICT_STL_CHECKS)
    add_definitions(-D_GLIBCXX_DEBUG)
    add_definitions(-D_GLIBCXX_DEBUG_PEDANTIC)
    add_definitions(-D_FORTIFY_SOURCE=2)

    add_definitions(-D_LIBCPP_DEBUG=2)
endif()

if(WIN32)
    add_definitions(-DNOMINMAX)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()

if(WIN32)
    add_definitions(-DNOMINMAX)
    add_definitions(-D_USE_MATH_DEFINES)
endif()

if(BUILD_SHARED_LIBS)
    add_definitions(-DEPOLL_WEB_LIBRARY_SHARED)
else()
    add_definitions(-DEPOLL_WEB_LIBRARY_STATIC)
endif()
