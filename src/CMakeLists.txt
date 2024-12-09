add_subdirectory(common)
add_subdirectory(network)

set(EPOLL_WEB_TARGETS
    server
    client
    test
)

foreach(name ${EPOLL_WEB_TARGETS})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name}
    PUBLIC
        # src
        epoll_web_common
        epoll_web_network

        # package
        Threads::Threads
        mimalloc
        Boost::headers

        
        RE2::re2

        # external
    )

    if(EPOLL_WEB_ENABLE_TEST)
        target_link_libraries(${name}
        PUBLIC
            benchmark::benchmark
            GTest::Gtest
        )
    endif()

endforeach(name ${EPOLL_WEB_TARGETS})

install(TARGETS ${EPOLL_WEB_TARGETS})
