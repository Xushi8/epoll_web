add_subdirectory(common)

set(EPOLL_WEB_TARGETS
    main
)

foreach(name ${EPOLL_WEB_TARGETS})
    add_executable(${name} ${name}.cpp)
    target_link_libraries(${name}
    PUBLIC
        # src
        epoll_learn_common

        # package
        Threads::Threads

        # external
    )

endforeach(name ${EPOLL_WEB_TARGETS})

install(TARGETS ${EPOLL_WEB_TARGETS})
