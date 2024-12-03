add_subdirectory(common)
add_subdirectory(network)

set(EPOLL_WEB_TARGETS
    server
    client
)

foreach(name ${EPOLL_WEB_TARGETS})
    add_executable(${name} ${name}.c)
    target_link_libraries(${name}
    PUBLIC
        # src
        epoll_learn_common
        epoll_learn_network

        # package
        Threads::Threads

        # external
    )

endforeach(name ${EPOLL_WEB_TARGETS})

install(TARGETS ${EPOLL_WEB_TARGETS})
