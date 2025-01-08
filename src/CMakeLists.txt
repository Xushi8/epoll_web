add_subdirectory(common)
add_subdirectory(network)
add_subdirectory(game)

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
        epoll_web_game

        # package
        Threads::Threads
        Boost::headers

        NCURSES::ncurses

        # external
    )

    if(EPOLL_WEB_TEST)
        target_link_libraries(${name}
        PUBLIC
            benchmark::benchmark
            GTest::Gtest
        )
    endif()

endforeach(name ${EPOLL_WEB_TARGETS})

install(TARGETS ${EPOLL_WEB_TARGETS})
