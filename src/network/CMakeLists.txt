add_library(epoll_web_network "")

file(GLOB_RECURSE sources CONFIGURE_DEPENDS src/*.c src/*.cpp)
file(GLOB_RECURSE header CONFIGURE_DEPENDS include/*.h include/*.hpp)

target_sources(epoll_web_network
PRIVATE
    ${sources}
PUBLIC
    ${header}
)

target_include_directories(epoll_web_network 
PUBLIC
./include
)

# 使用这个链接参数可以保证全局变量也被导出到静态库
# if(NOT BUILD_SHARED_LIBS)
#     target_compile_options(epoll_web_network PRIVATE -Wl,--whole-archive)
#     target_link_options(epoll_web_network PRIVATE -Wl,--whole-archive)
# endif()

if(BUILD_SHARED_LIBS OR EPOLL_WEB_INSTALL_STATIC_LIB)
    install(TARGETS epoll_web_network)
endif()

install(DIRECTORY ./include/
        DESTINATION include)
