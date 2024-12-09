find_package(Threads REQUIRED)

find_package(Boost 1.81.0 CONFIG REQUIRED COMPONENTS headers)
if(EPOLL_WEB_STATIC_EXTERNAL_LIB)
    set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(RE2)

find_package(benchmark)

if(EPOLL_WEB_ENABLE_TEST)
    find_package(GTest REQUIRED)
endif()

# Some package always dynamic link
# dynamic link begin
set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll" ".so")



if(EPOLL_WEB_STATIC_EXTERNAL_LIB)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".a")
endif()
# dynamic link end
