if(WIN32)
    add_compile_options(-finput-charset=utf-8 -fexec-charset=utf-8)
    add_link_options(-finput-charset=utf-8 -fexec-charset=utf-8)
endif()


if(EPOLL_WEB_ENABLE_SSE4)
    add_compile_options(-march=core2)
    add_link_options(-march=core2)
endif()

if(EPOLL_WEB_ENABLE_AVX)
    add_compile_options(-march=sandybridge)
    add_link_options(-march=sandybridge)
endif()

if(EPOLL_WEB_ENABLE_AVX2)
    add_compile_options(-march=haswell)
    add_link_options(-march=haswell)
endif()

if(EPOLL_WEB_NATIVE_ARCH)
    add_compile_options(-march=native)
    add_link_options(-march=native)
endif()

add_compile_options(-fdiagnostics-color=always)
add_link_options(-fdiagnostics-color=always)

add_compile_options(-fvisibility=hidden)
add_link_options(-fvisibility=hidden)

# warnings
add_compile_options(-Wall -Wextra -pedantic -Wshadow -Wcast-qual -Wfloat-equal -Wredundant-decls -Wundef -Wpointer-arith -Werror=return-type -Wimplicit-fallthrough)
add_link_options(-Wall -Wextra -pedantic -Wshadow -Wcast-qual -Wfloat-equal -Wredundant-decls -Wundef -Wpointer-arith -Werror=return-type -Wimplicit-fallthrough)
# add_compile_options(-Wconversion -Wsign-conversion)
# add_link_options(-Wconversion -Wsign-conversion)

add_compile_options(-Werror)
add_link_options(-Werror)

# optimize

add_compile_options(-fopenmp)
add_link_options(-fopenmp)

# add_compile_options(-fno-semantic-interposition)
# add_link_options(-fno-semantic-interposition)

if(EPOLL_WEB_ENABLE_ADDRESS_SANITIZER)
    add_compile_options(-fsanitize=address)
    add_link_options(-fsanitize=address)
endif()

if(EPOLL_WEB_ENABLE_UNDEFINED_SANITIZER)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
endif()

if(EPOLL_WEB_ENABLE_THREAD_SANITIZER)
    add_compile_options(-fsanitize=thread)
    add_link_options(-fsanitize=thread)
endif()

if(EPOLL_WEB_ENABLE_MEMORY_SANITIZER)
    add_compile_options(-fsanitize=memory)
    add_link_options(-fsanitize=memory)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-ggdb3 -fno-omit-frame-pointer)
    add_link_options(-ggdb3 -fno-omit-frame-pointer)
endif()

if(EPOLL_WEB_ENABLE_MOLD)
    add_link_options(-fuse-ld=mold)
endif()


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-fgraphite-identity -fdevirtualize-at-ltrans -fipa-pta -fuse-linker-plugin)
    add_link_options(-fgraphite-identity -fdevirtualize-at-ltrans -fipa-pta -fuse-linker-plugin)
    # add_compile_options(-floop-nest-optimize)
    # add_link_options(-floop-nest-optimize)

    add_compile_options(
        $<$<COMPILE_LANGUAGE:CXX>:-fconstexpr-ops-limit=1000000000>
        $<$<COMPILE_LANGUAGE:CXX>:-fconstexpr-loop-limit=100000000>
    )
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-fcolor-diagnostics)
    add_link_options(-fcolor-diagnostics)
endif()
