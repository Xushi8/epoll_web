find_package(Threads REQUIRED)


# Some package always dynamic link
# dynamic link begin
set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll" ".so")


if(EPOLL_WEB_STATIC_EXTERNAL_LIB)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a")
endif()
# dynamic link end
